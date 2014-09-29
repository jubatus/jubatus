RPM Packaging
=============

1. On RHEL 6, install the following packages.

::

  sudo yum groupinstall 'Development Tools'
  sudo yum install git-core ruby rpm-build rpmdevtools spectool createrepo
  sudo yum install oniguruma-devel apr-devel apr-util-devel

2. Modify ``package-config`` to specify which version to build. See below for details.

3. Now build the package:

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

