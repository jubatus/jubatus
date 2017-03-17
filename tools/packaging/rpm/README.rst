RPM Packaging
=============

Building RPM Packages with Docker
---------------------------------

You can build RPM packages in Docker.

First create Docker images for building package:

::

  docker build -t jubatus-rpmbuild-centos6 -f docker/Dockerfile.centos6 .
  docker build -t jubatus-rpmbuild-centos7 -f docker/Dockerfile.centos7 .

Modify ``package-config`` as appropriate (see below).

Then build packages:

::

  docker run --rm -v $PWD:/pkg jubatus-rpmbuild-centos6 sh -c "cd pkg && chown root: -R rpmbuild && ./package.sh -cai"
  docker run --rm -v $PWD:/pkg jubatus-rpmbuild-centos7 sh -c "cd pkg && chown root: -R rpmbuild && ./package.sh -cai"

Do not run these commands simultaneously!
These two ``docker run`` commands share the current directory.

Building RPM Packages without Docker
------------------------------------

1. On RHEL 6 or 7, install the following packages.

::

  sudo yum groupinstall 'Development Tools'
  sudo yum install git-core ruby rpm-build rpmdevtools spectool createrepo
  sudo yum install apr-devel apr-util-devel opencv-devel

2. Modify ``package-config`` to specify which version to build. See below for details.

3. Now build the package. Note that this command can be run only by users who have passwordless sudo privilege.

::

 ./package.sh -ca

Run ``./package.sh`` to see the detailed usage.


Common Usage
------------

Uninstall, clean, build all packages and install:

::

 ./package.sh -ucai


package-config Rules
====================

General
-------

* ``*_VERSION``

  - Product version.

* ``*_RELEASE``

  - Package version.
  - Must be incremented to release new RPM with the same ``*_VERSION``.

For jubatus-release
-------------------

* ``*_VERSION``

  - This value is computed automatically to match with the RHEL major version (i.e., 6 or 7) of the build environment.

For jubatus, jubatus-core, jubatus-mpio, jubatus-msgpack-rpc, jubadump
---------------------------------------------------------------------------------

* ``*_IS_SNAPSHOT``

  - When set to ``yes``, date and ID of the commit is appended to the package version (e.g, ``jubatus-0.3.1-1.20120827git431a92.el6.x86_64.rpm``). It is intended for "nightly" builds.
  - When set to ``no``, use simple package version (e.g., ``jubatus-0.3.1-1.el6.x86_64.rpm``). This is for release.

* ``*_CHECKOUT``

  - Tag, commit ID or branch of the VCS (Git/Mercurial) to use for this build.

Note that, these parameters will NOT be expanded in SPEC templates (``*.spec.in``).
Never try to write any logic inside SPEC files!

For mecab-ipadic
----------------

* ``*_ARCHIVE``

  - The archive name to download.

* ``*_ARCHIVE_DIR``

  - The name of the directory extracted from ``*_ARCHIVE``.

