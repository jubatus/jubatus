RPM Packaging
=============

1. Setup the build tools.

::

 yum -y install rpmdevtools

2. Modify `package-config` to specify which version to build.

3. Now build the package:

::

 ./package.sh -ca

Run `./package.sh` to see the detailed usage.

Requirements
------------

* RHEL 6

Common Usage
------------

Uninstall, clean, build all packages and install:

::

 ./package.sh -ucai
