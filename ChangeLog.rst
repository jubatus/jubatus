
Release 0.4.3 - 2013/04/19
--------------------------

* Improvements
    * Support Python 2.4 for building (#296)
    * Code generator 'jenerator' supports Ruby and Python Client
    * Divide server into ML module and RPC server (#264, #311)
    * Fixed Naming rule on some machine learning algorithms (#257, #299)
    * #95, #249, #290, #293, #303, #304, #307, #312, #313, #314, #318

* Bugfix
    * anomaly: doesn't use weight_manager in fv_converter (#309)
    * classifier: PA2 doesn't use config (#302)
    * #301


Release 0.4.2 - 2013/03/15
--------------------------

* Improvements
    * Feature extraction configration supports exception rules (#253)
    * Add 'clear' method for classifier, regression and stat (#200, #279)
    * Keeper supports configuration of session pool (#266)
    * Add 'get_client' method for each clients (related #244)
    * Check if thread local variables are enable in configure (#283)
    * Output logs when server started (#258)
    * Output logs when server received signal (#208)

* Bugfix
    * fv_converter may revert wrong feature (#247)
    * Fix PA update equation (#254)
    * Fix CW update equation (#277)
    * Segfaults when point-to-point (tap) interface is installed (#273)
    * recommender/anomaly: does not call clear() for weight manager (#282)
    * ux_splitter plug-in does not raise error when directory is specified for dict_path (#286)
    * #229, #263, #267, #271, #278


Release 0.4.1 - 2013/02/15
--------------------------

* Improvements
    * New code generator 'jenerator' for framework users (#240)
    * Applied coding rules based on Google C++ Style Guide
    * jubaconv accepts v0.4.x configuration format (#223, #241)
    * jubaconfig validates JSON syntax of configuration (#226, #245)
    * server/keeper behaves more stably in distributed mode when failed to start listening (#201)
    * pkg-config manifest for Jubatus clients (#228)

* Bugfix
    * jubastat returns min/max from values only the current window (#238, #242)
    * #230, #231, #233, #234, #235, #237, #248


Release 0.4.0 - 2013/01/11
--------------------------

* New machine learning service
    * Distributed Anomaly Detection (jubaanomaly)
        * Calculate anomaly measure value

* Improvements
    * Change RPC implementation to msgpack-rpc based (#27)
        * [ATTENTION] Requires jubatus-mpio and jubatus-msgpack-rpc to install
        * Add mpidl's output converter 'mpidlconv' (#210)
    * Remove set_config API
        * Standalone mode: specify config file using command option '-f, --configpath'
        * Multinode mode: store config data to Zookeeper using jubaconfig (#164)
    * More flexible configration (#38)
        * Support hyper-parameters (#197)
    * Commonize name of variables and functions (#203)
    * Add or Change command options
        * Add bind IP address option '-b, --listen_addr' (#152)
        * Add bind network interfance option '-B, --listen_if' (#152, #214)
        * Check if logging directory specified by '-l, --logdir' is writeable (#206)
        * Add verbosity of log messages option '-e, --loglevel' (#207)
        * Add configuration file option '-f, --configpath'
        * Change '-d, --tmpdir' to '-d, --datadir' (#198)
    * Improve logging
        * Not separate log file for each log level (#207)
        * Add start save/load logs (#195)
        * Add many debug logs

* Bugfix
    * Client symbols may conflict because of using same namespace (#84)
    * Fix shortest_path when target is found before reaching landmark (#177)
    * Commonize RPC return types and values (#193)
    * Segfaults when plugin throws JUBATUS_EXCEPTION (#211)
    * Fix classifier AROW hyper-parameter conversion (#225)
    * #86, #184, #204, #213, #219

* Refactoring
    * #186, #187, #191, #192, #199


Release 0.3.4 - 2012/11/26
--------------------------

* Improvements
    * Add --logdir option
    * Add --enable-debug option to configure (#130)
    * Catch bad cast of fv_converter (#170)

* Bugfix
    * Plugin basename specification does not work correctly (#57)
    * MIX put_diff/get_diff and update/analyze RPC can be executed on the same time (#171)
    * #121, #131, #136, #150, #155, #156, #157, #160, #163, #165, #167, #172, #179, #180


Release 0.3.3 - 2012/10/29
--------------------------

* Improvements
    * deb package (#14)
    * Jubatus loads plugin from default directory (#57)
    * Add hash_max_size option to learn in fixed-size memory (#67)
    * OS X Homebrew packaging (#116)
    * GCC compilation version <= 4.2 when zookeeper enabled (#60)
    * Experimental support for Clang compilation (#100)
    * Make the timeout smaller in unittest
    * libmecab_splitter works well in multi-thread environment, and now only support mecab ver. 0.99 or later
    * word_splitter::split method is now constant
    * global_id_generator(standalone-mode) for graph, that supports 32 bit environment (#135)
    * Use (document_frequency + 1) to calculate IDF weight to avoid inifinity

* Bugfix
    * #94, #104, #106, #108, #110, #111, #113, #114, #117, #118, #124, #126, #129, #133, #138, #139, #146, #148


Release 0.3.2 - 2012/09/21
--------------------------

* Improvements
    * jubavisor/jubactl support jubatus server options (#75)
    * jubadoc: document generator (.idl => .rst) (#88)
    * jenerator: enabled -o option to specify output directory
    * yum (RPM) packaging (#40)

* Bugfix
    * #51, #76, #77, #79, #83, #90, #91, #96, #98, #99, #100, #101, #102, #103


Release 0.3.1 - 2012/07/20
--------------------------

* Improvements
    * RPC enhances to many exceptions and provide new error handling interface (#49)
    * JSON interface for set_config APIs (#44)
    * jubavisor close zk connection correctly (#74)

* Bugfix
    * #73, #69, #66, #65


Release 0.3.0 - 2012/06/29
--------------------------

* Improvements
    * Distributed Graph Processing
        * Centrality computation
        * Shortest path search
    * New neighbor-search algorithm for recommender: minhash
    * installs pkgconfig file (#42, jubatus.pc)
    * jubavisor handling signal (#53)
    * Exception handling and error outputs (#55)

* Bugfix
    * internal API get_storage implemented again (#21)
    * Forgot ignoring signal in keeper #47
    * #54, #45, #15


Release 0.2.3 - 2012/06/08
--------------------------

* Improvements
    * Asynchronous call to multiple servers at once, both keeper and mix - common/mprpc
    * Refactor generator
    * Error message improve
    * Timeout in unittest expanded

* Bugfix
    * jubactl doesn't work ver 0.2 and later #13
    * jubavisor doesn't work on daemon mode #5
    * Asynchronous mprpc client critical bug
    * #47, #50, #34, #36, #37, #31, #19, and other small bugfix


Release 0.2.2 - 2012/04/06
--------------------------

* Improvements
    * Simpler interfaces at classifier, regression and recommender
        * Clients are *NOT COMPATIBLE* with previous releases
    * Now mix works concurrently in multiple threads (except tf-idf counting)
    * Asynchronous RPC to multiple servers at once
    * Add --version option
    * Interface description language changed from C++-like to Annotated MessagePack-IDL
    * Minor error handling
    * A bit more tested than previous releases

* Bugfix
    * #30, #29, #22


Release 0.2.1 - 2012/03/13
--------------------------

* Bugfix
    * #28


Release 0.2.0 - 2012/02/16
--------------------------

* New Features
    * recommender
        * support fast similar item search, real-time update, distributed data management
        * inverted index : exact result, fast search
        * locality sensitive hash : approximate result, fast search, small working space
    * regression
        * online SVR using passive agressive algorithm
        * as fast as current classifier
    * stat
        * a Key(string)-Value(queue<double>)
        * O(1) cost of getting sum, standard deviation, max, min, statistic moments for each queue
    * server framework
        * less-tightly coupled distributed processing framework with each ML implementation
        * idl & code generator - make it easy to write own jubatus system
        * removed public release of client libraries (so easy to generate!)
        * multiple mix - mutiple data objects can be mixed in one jubatus system

* Bugfix
    * duplicate key entry in fv_converter breaks the parameter


Release 0.1.1 - 2011/11/15
--------------------------

Bugfix release


Release 0.1.0 - 2011/10/26
--------------------------

Hello Jubatus!

First release: including classifier, and mix operation
