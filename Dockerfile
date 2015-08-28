# jubatus base image
FROM ubuntu:12.04

# install the latest jubatus
RUN echo "deb http://download.jubat.us/apt binary/" >> /etc/apt/sources.list.d/jubatus.list
RUN apt-get -y update
RUN apt-get --force-yes -y install jubatus

# set environment variables from /opt/jubatus/profile
ENV JUBATUS_HOME /opt/jubatus
ENV PATH ${JUBATUS_HOME}/bin:${PATH}
ENV LD_LIBRARY_PATH ${JUBATUS_HOME}/lib:${LD_LIBRARY_PATH}
ENV LDFLAGS -L${JUBATUS_HOME}/lib ${LDFLAGS}
ENV CPLUS_INCLUDE_PATH ${JUBATUS_HOME}/include:${CPLUS_INCLUDE_PATH}
ENV PKG_CONFIG_PATH ${JUBATUS_HOME}/lib/pkgconfig:${PKG_CONFIG_PATH}
