Jubatus
=======

The Jubatus library is an online machine learning framework which runs in distributed environment.

See http://jubat.us for details.

QuickStart
----------

Jubatus officially supports Ubuntu >= 12.04 , Red Hat Enterprise Linux >= 6.2 . On other platforms, some may work if they have gcc >= 4.4 .

For minimal standalone mode, Jubatus requires libevent >= 1.4 , pficommon >= 1.3.1 , google-glog , libmsgpack, python >= 2.6 . Please install these software with headers before installation.

::

  $ git clone git://github.com/jubatus/jubatus.git
  $ cd jubatus
  $ ./waf configure --disable-re2
  $ ./waf build
  $ sudo ./waf install
  $ jubaclassifier

Here Jubatus classifier server has started. Enjoy!

License
-------

LGPL 2.1

Update history
--------------

Release 0.3.2 2012/9/21
~~~~~~~~~~~~~~~~~~~~~~~

Improvements
  - jubavisor/jubactl support jubatus server options (#75)
  - jubadoc: document generator (.idl => .rst) (#88)
  - jenerator: enabled -o option to specify output directory
  - yum (RPM) packaging (#40)

Bugfix
  - #51, #76, #77, #79, #83, #90, #91, #96, #98, #99, #100, #101, #102, #103

Release 0.3.1 2012/7/20
~~~~~~~~~~~~~~~~~~~~~~~

Improvements
  - RPC enhances to many exceptions and provide new error handling interface (#49)
  - JSON interface for set_config APIs (#44)
  - jubavisor close zk connection correctly (#74)

Bugfix
  - #73, #69, #66, #65

Release 0.3.0 2012/6/29
~~~~~~~~~~~~~~~~~~~~~~~

Improvements
  - Distributed Graph Processing

    - Centrality computation
    - Shortest path search

  - New neighbor-search algorithm for recommender: minhash
  - installs pkgconfig file (#42, jubatus.pc)
  - jubavisor handling signal (#53)
  - Exception handling and error outputs (#55)

Bugfix
  - internal API get_storage implemented again (#21)
  - Forgot ignoring signal in keeper #47
  - #54, #45, #15

Release 0.2.3 2012/6/8
~~~~~~~~~~~~~~~~~~~~~~~

Improvements
  - Asynchronous call to multiple servers at once, both keeper and mix - common/mprpc
  - Refactor generator
  - Error message improve
  - Timeout in unittest expanded

Bugfix
  - jubactl doesn't work ver 0.2 and later #13
  - jubavisor doesn't work on daemon mode #5
  - Asynchronous mprpc client critical bug
  - #47, #50, #34, #36, #37, #31, #19, and other small bugfix

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
