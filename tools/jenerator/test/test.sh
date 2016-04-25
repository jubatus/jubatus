#!/bin/bash -e

mkdir -p testwork
pushd testwork
../../src/jenerator -l server -t ../sample.idl -n toolong::name::space
mv sample_serv.tmpl.cpp sample_serv.cpp
mv sample_serv.tmpl.hpp sample_serv.hpp
g++ -Wall -pthread -o sample_server sample_serv.cpp sample_impl.cpp `pkg-config --cflags --libs jubatus_core jubatus`
g++ -Wall -pthread -o small_proxy sample_proxy.cpp `pkg-config --cflags --libs jubatus_core jubatus`
popd
