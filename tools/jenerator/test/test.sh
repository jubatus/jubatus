#!/bin/bash

mkdir -p testwork
pushd testwork
../../src/jenerator -l server -t ../sample.idl -n toolong::name::space || exit 1
mv sample_serv.tmpl.cpp sample_serv.cpp
mv sample_serv.tmpl.hpp sample_serv.hpp
g++ -Wall -o sample_server sample_serv.cpp sample_impl.cpp `pkg-config --cflags --libs jubatus`
g++ -Wall -o small_keeper sample_keeper.cpp `pkg-config --cflags --libs jubatus`
popd
