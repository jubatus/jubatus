#!/bin/bash -ue

################################################################################
# Build RPMs for Jubatus & Dependencies
################################################################################

# Packages to be built, in order of dependencies
PKGS_DEPENDS="msgpack jubatus-mpio jubatus-msgpack-rpc zookeeper-client log4cxx oniguruma ux mecab mecab-ipadic"
PKGS_JUBATUS="jubatus-core jubatus jubadump jubatus-release"

# Directories
PACKAGER="$(basename "${0}")"
PACKAGER_DIR="$(cd $(dirname ${0}) && echo ${PWD})"
PACKAGER_RPM_DIR="${PACKAGER_DIR}/rpmbuild"

# Commands
RUN_AS_ROOT="sudo"
YUM="yum --disablerepo=*"

# Includes
. ${PACKAGER_DIR}/package-config
. ${PACKAGER_DIR}/package-prebuild

# Usage
usage() {
	cat << _EOF_

Usage:
	${PACKAGER} [-u] [-c|-C] [[-i] [-a|-j|-p package...]]

Options:
	-a	Build all packages (Jubatus and its dependencies).
	-j	Build Jubatus packages (${PKGS_JUBATUS}) only.
	-p	Build the specified package(s) only. Available packages are:
			${PKGS_JUBATUS}
			${PKGS_DEPENDS}

	-i	Install built packages.
			If you don't have build-requirement packages (msgpack-devel,
			ux-devel, etc.) installed, use this option to automatically
			install built packages for each time, before going onto next
			package build process.
			In general, you only need to use this option for the first time.
			When using this option, you need to run this command as root, or
			sudo command needs to be available. When using sudo, you may need
			to type in your password; in this case, the build process will
			become interactive.
			You must specify "-c" or "-C" (see below) together with this option.

	-u	Uninstall all installed packages.

	-c	Clean the rpmbuild directory, but preserve downloaded archives.
	-C	Completely clean the rpmbuild directory.

Note:
	When "-u", "-c" or "-C" is specified together with one of "-a", "-j" or "-p",
	uninstall/clean operation runs prior to building packages.

_EOF_
}

# Test if the given command(s) exists in PATH
test_command_exists() {
	for CMD in "$@"; do
		if ! type "${CMD}" > /dev/null 2>&1; then
			echo "ERROR: Cannot detect '$CMD' command."
			echo "Try \`yum provides '*/$CMD'\` to find package."
			return 1
		fi
	done
}

# "cleanroom" - run the given command in a cleanroom for environment-sensitive jobs
cleanroom() {
	/usr/bin/env -i - HOME="$(echo ~$(whoami))" PATH="/sbin:/bin:/usr/sbin:/usr/bin" "$@"
}

# Clean but preserve downloaded archives
clean_minimal() {
	rm -rf "${PACKAGER_RPM_DIR}"/*/{BUILD,BUILDROOT,RPMS,SRPMS}
	rm -f "${PACKAGER_RPM_DIR}"/*/SPECS/*.spec
}

# Clean everything
clean_all() {
	clean_minimal
	rm -f "${PACKAGER_RPM_DIR}"/*/SOURCES/*.{gz,bz2}
}

# Uninstall all packages
uninstall_all() {
	REMOVE_PKGS=""
	echo "Uninstall packages..."
	for PACKAGE in ${PKGS_DEPENDS} ${PKGS_JUBATUS}; do
		REMOVE_PKGS="${REMOVE_PKGS} ${PACKAGE} ${PACKAGE}-debuginfo"
	done
	${RUN_AS_ROOT} ${YUM} -y remove ${REMOVE_PKGS}
}

# Build packages
build_package() {
	for PACKAGE in ${@}; do
		echo "Building package for ${PACKAGE}..."
		pushd "${PACKAGER_RPM_DIR}/${PACKAGE}"
		prepare_rpmbuild "${PACKAGE}"
		cleanroom rpmbuild \
			--define "%dist .el${JUBATUS_RELEASE_VERSION}" \
			--define "%_topdir "${PACKAGER_RPM_DIR}"/"${PACKAGE}"" \
			-ba SPECS/${PACKAGE}.spec
		[ "${AUTO_INSTALL}" = "yes" ] && "${RUN_AS_ROOT}" ${YUM} -y install RPMS/*/*.rpm
		popd
	done
}

# Main
main() {
	[ $# -lt 1 ] && ( usage; exit 1 );

	# Check if commands required to run this script exist.
	# Commands not included in coreutils package must be listed here.
	test_command_exists git tar perl yum rpmbuild spectool sudo wget || exit 1

	# See how we're called.
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

	# When using "-i" mode, "-c" or "-C" must be specified together,
	# to ensure that previously-built RPMs are not globbed by wildcard.
	if [ "${AUTO_INSTALL}" = "yes" -a "${CLEAN_MODE}" = "no" ]; then
		echo "ERROR: '-c' or '-C' must be specified when using '-i'"
		exit 1
	fi

	# Clean task
	if [ "${CLEAN_MODE}" = "minimal" ]; then
		echo "Cleaning..."
		clean_minimal
	elif [ "${CLEAN_MODE}" = "all" ]; then
		echo "Cleaning Everything..."
		clean_all
	fi

	# Uninstall task
	if [ "${UNINSTALL_MODE}" = "yes" ]; then
		uninstall_all
	fi

	# Build task
	if [ "${BUILD_ALL}" = "yes" ]; then
		build_package ${PKGS_DEPENDS} ${PKGS_JUBATUS}
	elif [ "${BUILD_JUBATUS}" = "yes" ]; then
		build_package ${PKGS_JUBATUS}
	elif [ "${BUILD_PKGS}" = "yes" ]; then
		build_package ${@}
	fi

	cat <<- _EOF_
	===============================================
	 SUCCESS (${SECONDS} seconds)
	===============================================
	_EOF_
}

[ ${#BASH_SOURCE[@]} = 1 ] && main "$@"
