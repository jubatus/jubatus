FROM centos:6

RUN yum -y update && \
    yum -y groupinstall "Development Tools" && \
    yum -y install sudo git-core ruby rpm-build rpmdevtools spectool createrepo apr-devel apr-util-devel opencv-devel
