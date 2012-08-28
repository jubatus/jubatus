#!/bin/bash -ue

################################################################################
# Build RPMs for Jubatus & Dependencies
################################################################################

# Packages to be built, in order of dependencies
PKGS_DEPENDS="msgpack pficommon zookeeper-client glog re2 ux mecab mecab-ipadic"
PKGS_JUBATUS="jubatus jubatus-client jubatus-release"

# Directories
PACKAGER="$(basename "${0}")"
PACKAGER_DIR="$(cd $(dirname ${0}) && echo ${PWD})"
PACKAGER_RPM_DIR="${PACKAGER_DIR}/rpmbuild"
PACKAGER_DOWNLOAD_DIR="${PACKAGER_DIR}/download"

# Commands
RUN_AS_ROOT="sudo"

# Includes
. ${PACKAGER_DIR}/package-config
. ${PACKAGER_DIR}/package-prebuild

# Usage
usage() {
	cat << _EOF_

Usage:
	${PACKAGER} [ -i ] { -a | -j | -p package1 [package2 ...] }
	${PACKAGER} -u
	${PACKAGER} { -c | -C }

Options:
	-i	Install built packages.
			If you don't have build-requirement packages (pficommon-devel,
			msgpack-devel, etc.) installed, use this option to automatically
			install built packages for each time, before going onto next 
			package build process.
			In general, you only need to use this option for the first time.
			When using this option, you need to run this command as root, or
			sudo command needs to be available. When using sudo, you may need
			to type in your password; in this case, the build process will
			become interactive.

	-a	Build all packages (Jubatus and its dependencies).
	-j	Build Jubatus packages (${PKGS_JUBATUS}) only.
	-p	Build the specified package(s). Available packages are:
			${PKGS_JUBATUS}
			${PKGS_DEPENDS}

	-u	Uninstall all installed packages.
	-c	Clean the rpmbuild directory, but preserve downloaded archives.
	-C	Completely clean the rpmbuild directory.

_EOF_
}

# "cleanroom" - run the given command in a cleanroom for environment-sensitive jobs
cleanroom() {
	/usr/bin/env -i - HOME="$(echo ~$(whoami))" PATH="/sbin:/bin:/usr/sbin:/usr/bin" "$@"
}

# Clean (minimal)
clean_minimal() {
	rm -rf "${PACKAGER_RPM_DIR}"/{BUILD,BUILDROOT,RPMS,SRPMS}
	rm -rf "${PACKAGER_RPM_DIR}"/SOURCES/jubatus
	rm -rf "${PACKAGER_RPM_DIR}"/SOURCES/pficommon
	rm -rf "${PACKAGER_RPM_DIR}"/SOURCES/re2
}

# Clean (all)
clean_all() {
	clean_minimal
	rm -f "${PACKAGER_RPM_DIR}"/SOURCES/*.{gz,bz2}
	rm -f "${PACKAGER_RPM_DIR}"/SPECS/*.spec
}

# Uninstall All
uninstall_all() {
	REMOVE_PKGS=""
	for PACKAGE in ${PKGS_DEPENDS} ${PKGS_JUBATUS}; do
		REMOVE_PKGS="${REMOVE_PKGS} ${PACKAGE} ${PACKAGE}-debuginfo"
	done
	${RUN_AS_ROOT} yum -y remove ${REMOVE_PKGS}
}

# Main
main() {
	if [ $# -lt 1 ]; then
		usage
		exit 1
	fi

	AUTO_INSTALL="no"
	BUILD_ALL="no"
	BUILD_JUBATUS="no"
	BUILD_PKGS="no"
	UNINSTALL_MODE="no"
	CLEAN_MODE="no"
	while getopts iajpucC OPT; do
		case "${OPT}" in
			"i" )
				AUTO_INSTALL="yes" ;;
			"a" )
				BUILD_ALL="yes" ;;
			"j" )
				BUILD_JUBATUS="yes" ;;
			"p" )
				BUILD_PKGS="yes" ;;
			"u" )
				UNINSTALL_MODE="yes" ;;
			"c" )
				CLEAN_MODE="minimal" ;;
			"C" )
				CLEAN_MODE="all" ;;
			* )
				usage
				exit 1
				;;
		esac
	done
	shift $((${OPTIND} - 1))

	# uninstall options
	if [ "${UNINSTALL_MODE}" = "yes" ]; then
		uninstall_all
	fi

	# clean options
	if [ "${CLEAN_MODE}" = "minimal" ]; then
		echo "Cleaning..."
		clean_minimal
	elif [ "${CLEAN_MODE}" = "all" ]; then
		echo "Cleaning Everything..."
		clean_all
	fi

	# build options
	TARGET_PKGS=""
	if [ "${BUILD_ALL}" = "yes" ]; then
		TARGET_PKGS="${PKGS_DEPENDS} ${PKGS_JUBATUS}"
	elif [ "${BUILD_JUBATUS}" = "yes" ]; then
		TARGET_PKGS="${PKGS_JUBATUS}"
	elif [ "${BUILD_PKGS}" = "yes" ]; then
		TARGET_PKGS="${@}"
	fi

	if [ -n "${TARGET_PKGS}" ]; then
		pushd "${PACKAGER_RPM_DIR}"
		for PACKAGE in ${TARGET_PKGS}; do
			prepare_rpmbuild "${PACKAGE}"
			cleanroom rpmbuild --define "%_topdir "${PACKAGER_RPM_DIR}"" -ba SPECS/${PACKAGE}.spec
			[ "${AUTO_INSTALL}" = "yes" ] && "${RUN_AS_ROOT}" yum -y install RPMS/*/*.rpm
		done
		popd
	fi

	cat <<- _EOF_
	===============================================
	 SUCCESS (${SECONDS} seconds)
	===============================================
	_EOF_
}

[ ${#BASH_SOURCE[@]} = 1 ] && main "$@"
