Jubatus All-in-One Installer & Packager
=======================================

Building Debian Package with Docker
----------------------------------------------

You can build DEB packages in Docker.

First create Docker images for building package:

```
docker build -t jubatus-debbuild-ubuntu12 -f docker/Dockerfile.ubuntu12 .
docker build -t jubatus-debbuild-ubuntu14 -f docker/Dockerfile.ubuntu14 .
docker build -t jubatus-debbuild-ubuntu16 -f docker/Dockerfile.ubuntu16 .
```

Modify `jubapkg_version` as appropriate.

Then build the package:

```
docker run --rm -v $PWD:/pkg jubatus-debbuild-ubuntu12 sh -c 'cd pkg && ./jubapkg -f -c -d -b -p deb'
docker run --rm -v $PWD:/pkg jubatus-debbuild-ubuntu14 sh -c 'cd pkg && ./jubapkg -f -c -d -b -p deb'
docker run --rm -v $PWD:/pkg jubatus-debbuild-ubuntu16 sh -c 'cd pkg && ./jubapkg -f -c -d -b -p deb'
```

Do not run these commands simultaneously!
These 3 `docker run` commands share the current directory.

Building Debian Package without Docker
----------------------------------------------

On Ubuntu 12.04 LTS and 14.04 LTS, the following packages are required.

```
sudo apt-get install ssh git build-essential ruby1.9.3 pkg-config autoconf libtool devscripts debhelper liblog4cxx10-dev libopencv-dev
```

On Ubuntu 16.04, use `ruby` instead of `ruby1.9.3`.

Now you can build the package as follows:

```
export DEBEMAIL="jubatus-team@googlegroups.com"
export DEBFULLNAME="PFN & NTT"
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
