# jubatus base image
FROM ubuntu:14.04
MAINTAINER PFN & NTT <jubatus-team@googlegroups.com>

# install the latest jubatus
RUN echo "deb http://download.jubat.us/apt/ubuntu/trusty binary/" >> /etc/apt/sources.list.d/jubatus.list && \
    apt-get -y update && \
    apt-get --force-yes -y install jubatus && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# set environment variables from /opt/jubatus/profile
ENV JUBATUS_HOME /opt/jubatus
ENV PATH ${JUBATUS_HOME}/bin:${PATH}
ENV LD_LIBRARY_PATH ${JUBATUS_HOME}/lib:${LD_LIBRARY_PATH}
ENV LDFLAGS -L${JUBATUS_HOME}/lib ${LDFLAGS}
ENV CPLUS_INCLUDE_PATH ${JUBATUS_HOME}/include:${CPLUS_INCLUDE_PATH}
ENV PKG_CONFIG_PATH ${JUBATUS_HOME}/lib/pkgconfig:${PKG_CONFIG_PATH}
