Jenerator: a user-defined machine learning system
=================================================

Problem description
-------------------

Jubatus is going to be a framework to implement `online machine learning which scales automatically`.
But currently it has only a set of direct implementations of classifier, regression and recommender.
And there are 9 x N declarations or implemenations for eash learning machine's API, where N is a number
of API. For example, classifier currently has 7 APIs (train, classify, set_config, get_config, save,
load and get_status). Thus there're 9 x 7 = 63 reputations of function declarations or implementations.
Recommender has 12 APIs therefore we'd 84 reputations. (M variety of client libraries will increase
the number: (M + 8) x N )

A developer of a new machine learning algorithm would have seen a misery of Do Repeat Yourself.


Moreover, composable feature conversion and machine learning must be available in a near future.
With direct implementation of C++ language it is very difficult to minimize the cost of user-
implementation. Deciding a specification of Domain Specific Language will enable us a
under-the-wood optimization and interface improvement.


There are several preliminary condition - first of all, for all developers C++ should be available
because they have many existing software implementation of there exellent ML algorithms.
Secondly, the new system must have high usability to write, compile, deploy, test and use it. The
more difficult it is, the less developers will come.


Solution
--------

Simple msgpack-idl parser and a bit complex C++ code generator. Input specification of IDL is
very simple and a extension of msgpack-idl. Users should annotate 3 decorators on each RPC-API
of each services, which are about routing, R/W feature and aggregator.

routing
  - cht
  - broadcast
  - random

R/W feature
  - update   - this does changes the server state, guarded by writer lock.
  - analysis - does not change the server state, so that threads can work in parallel.

 

Users can use the input jubatus-IDL file. Output is a set of C++ files that is to be assembled
to the Jubatus servers: main server-kickstart program, proxy server to be the gateway and C++
server template (different from C++ 'template').

- Input: 'xxx.idl' which implements msgpack-idl based extended spec
- Output0: 'xxx_impl.cpp', 'xxx_server.hpp' which implements developer's application in server-side including 'main()'
- Output1: 'xxx_proxy.cpp' -- proxy implementation includes 'main()'
- Output2: 'xxx_client.hpp' client implementation
- Output3: 'xxx_types.hpp' type definitions which are used in server, proxy and client
- Output4: optional: 'xxx_serv.tmpl.(h|c)pp' -- server template for users


Licence of generated codes
--------------------------

Jenerator itself is distributed under LGPL, but you can distribute all generated codes under terms of your choice.


Reference
---------

- msgpack

  - http://msgpack.org

- msgpack-idl spec

  - https://github.com/msgpack/msgpack-haskell/blob/master/msgpack-idl/Specification.md

Install
-------

Use omake::

  $ omake
  $ omake install

Use `PREFIX` environment variable to choose installation direcotry::

  $ PREFIX=/where/you/want/to/install omake install


Things left
-----------

- parse version number in service
- other file inclusion
- comment parsing ('#\n' cannot be parsed)
- stop using polimorphic variant
