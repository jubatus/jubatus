FROM ubuntu:18.04

ENV DEBEMAIL="jubatus-team@googlegroups.com"
ENV DEBFULLNAME="PFN & NTT"
ENV DEBIAN_FRONTEND="nointeractive"

RUN apt-get update && \
    apt-get install -y ssh git build-essential ruby pkg-config autoconf libtool devscripts debhelper apt-utils liblog4cxx-dev libopencv-dev python python3-dev
