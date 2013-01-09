Jubatus All-in-One Installer & Packager
=======================================

Installing to Local File System
-------------------------------

```
./jubapkg -P ~/local -f -c -d -b -i
```

Building RPM Package
--------------------

```
./jubapkg -f -c -d -b -p rpm
```

Building Debian Package
-----------------------

```
export DEBEMAIL="jubatus@googlegroups.com"
export DEBFULLNAME="PFI & NTT"
./jubapkg -f -c -d -b -p deb
```
