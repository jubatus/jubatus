Jubatus
=======

The Jubatus library is a online machine learning framework which runs in distributed environment.

See http://jubat.us for details.

LICENSE
-------

LGPL 2.1

Update history
--------------

Release 0.3.0 2012/6/1
----------------------

Improvements
  - Distributed Graph Processing

    - Centrality computation
    - Shortest path search

  - libevent support (very partially)

Bugfix:
  - #19, #31, #37, #36, #34

Release 0.2.2 2012/4/6
~~~~~~~~~~~~~~~~~~~~~~

Improvements
  - Simpler interfaces at classifier, regression and recommender

    - Clients are *NOT COMPATIBLE* with previous releases

  - Now mix works concurrently in multiple threads (except tf-idf counting)
  - Asynchronous RPC to multiple servers at once
  - Add --version option
  - Interface description language changed from C++-like to Annotated MessagePack-IDL
  - Minor error handling
  - A bit more tested than previous releases

Bugfix
  - #30, #29, #22

Release 0.2.1 2012/3/13
~~~~~~~~~~~~~~~~~~~~~~~

Bugfix release: #28

Release 0.2.0 2012/2/16
~~~~~~~~~~~~~~~~~~~~~~~

New Features
  - recommender

    - support fast similar item search, real-time update, distributed data management
    - inverted index : exact result, fast search
    - locality sensitive hash : approximate result, fast search, small working space

  - regression

    - online SVR using passive agressive algorithm
    - as fast as current classifier

  - stat

    - a Key(string)-Value(queue<double>)
    - O(1) cost of getting sum, standard deviation, max, min, statistic moments for each queue

  - server framework

    - less-tightly coupled distributed processing framework with each ML implementation
    - idl & code generator - make it easy to write own jubatus system
    - removed public release of client libraries (so easy to generate!)
    - multiple mix - mutiple data objects can be mixed in one jubatus system

Bugfix
  - duplicate key entry in fv_converter breaks the parameter

Release 0.1.1 2011/11/15
~~~~~~~~~~~~~~~~~~~~~~~~

Bugfix release

Release 0.1.0 2011/10/26
~~~~~~~~~~~~~~~~~~~~~~~~

Hello Jubatus!

First release: including classifier, and mix operation
