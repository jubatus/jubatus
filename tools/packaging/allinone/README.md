Jubatus All-in-One Installer & Packager
=======================================

Building Debian Package
-----------------------

On Ubuntu 12.04 LTS (x86\_64), the following packages are required.

```
sudo apt-get install ssh git build-essential ruby1.9.3 pkg-config autoconf libtool devscripts debhelper liblog4cxx10-dev
```

Now you can build the package as follows:

```
export DEBEMAIL="jubatus@googlegroups.com"
export DEBFULLNAME="PFI & NTT"
./jubapkg -f -c -d -b -p deb
```

Building All-in-One RPM Package
--------------------------------------

```
./jubapkg -f -c -d -b -p rpm
```

Advanced Usecase: Installing to Local File System
----------------------------------------------------

```
./jubapkg -P ~/local -f -c -d -b -i
```
