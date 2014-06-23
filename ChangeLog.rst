Release 0.6.0 - 2014/06/23
--------------------------

* New Features
    * Add unlearning feature with sticky ID handling (#741, #756)
    * Extend string_rules plug-in interface to support arbitrary values (#658, #703)
    * classifier: Add delete_class RPC method (#364)
    * recommender: Support clear_row RPC method when using nearest_neighbor_recommender (#797)
* Improvements
    * Separate algorithm components as jubatus_core repository
    * Change logging library to log4cxx (#746, #807, #821)
    * Improve availability when ZooKeeper failover occurs (#52, #776)
    * Strict validation of configuration parameter (#547, #627, #754, #755)
    * Improve error handling regarding RPC connection errors (#734, #782, #783)
    * Default number of threads for proxies changed to 4 (#753, #810)
    * Name of saved model files now contain engine name (#709, #765)
    * Log message improvements (#791, #822)
    * Log path of loaded plugins (#713, #771)
    * More example config files added (#795, #818)
    * nearest_neighbor: method name changed from {similar,neighbor}_row_from_data to {similar,neighbor}_row_from_datum to be consistent with recommender (#564, #809)
    * anomaly: add RPC method now works with both lof and light_lof (#737)
    * #645, #742, #767, #772, #773, #774, #777, #784, #789, #794, #823, #830
* Bug fixes
    * binary_rule now accept ``except`` key (#792)
    * Calling do_mix RPC method for standalone mode now returns error correctly (#798, #808)
    * Jubatus servers now leave cluster before RPC server shutdown (#593, #813)

Release 0.5.4 - 2014/04/28
--------------------------

* New Features  
    * clustering: Add clear RPC method (#579)
* Improvements  
    * server: Fix loading order of plugin (#743)
    * server: Result of get_status RPC now contains absolute path (#749)
    * clustering: Remove unneeded debug code(#671)
* Bug fixes  
    * server: Linear mixer did not terminated in correct order (#732)
    * clustering: Fix forgetting factor's behavior (#704)

Release 0.5.3 - 2014/03/31
--------------------------

* New Features
    * classifier: Getter/setter methods to labelset of classifier (#655)
    * client: Add accessors to get/set cluster name in C++ client (#668)
    * client: Add do_mix RPC method in client (#700)

* Improvements
    * core: Eliminate cmath (#677)
    * fv_converter: Avoid using types depends on CPU arch (64-bit/32-bit) for serialized members (#692)
    * jenerator: Use relative import for python 3 (#712)
    * server: Disable checking ID when loading models (#679)
    * server: Detect IO errors in save (#698)
    * server: Refine mix counting message (#552)
    * server: MIX runs only updated model (#181)
    * server: Remove unused annotations in IDL (#714)
    * proxy: Print log when error occurred during proxy is forwarding requests (#733)

* Bugfix
    * clustering: Fix clustering test (#690)
    * clustering: Eliminate random number without seed in clustering tests (#718)
    * recommender: Fix recommender get_all_rows distribution mode to random (#683)
    * recommender: Fix bit_index_storage to erase rows with empty vector on MIX (#684)
    * recommender: Fix bit_index and inverted_index erase rows locally (#659)
    * recommender: Modify behavior of remove_row of lsh_index_storage before/after MIX (#681)
    * server: Fix mixer can go to infinite loop (#711)
    * server: Fix dead-lock caused by re-entrant lock acquisition over put_diff RPC (#723)
    * server: Fix RPC error handling in linear_mixer (#729)
    * server: Fix load RPC breaks model  (#721)


Release 0.5.2 - 2014/02/24
--------------------------

* Improvements
    * Fault-tolerant weight-manager (#660)
    * Add do_mix RPC to manually trigger MIX in distributed mode (#653)
    * Allow config file more than 1 KiB in distributed mode (#617, #624)
    * Print model version on MIX (#479, #649)
    * ``classifier`` does not return results whose scores are zero (#216)
    * Remove unused fields and functions (#639, #643)
    * More test code added for nearest neighbor (#529, #652)
    * More test code added for clustering (#650)
    * Code style and portability fix (#632, #647)
    * Automatically test if all the IDL elements exist in API reference (#175)

* Bug fix
    * IDs are mistakenly reused after save/load in jubaanomaly (#613, #661)
    * Fix forgetting parameter in jubaclustering (#673)
    * Fix misuse of C math functions (#663, #664)
    * Some header files are not installed (#666, #674, #678)
    * jubatus_clustering library is missing in pkg-config (#631, #665)
    * Fix JSON unit test failure under 32-bit Linux environment (#636)
    * Jubatus logo is broken (#635)


Release 0.5.1 - 2014/01/27
--------------------------

* Improvements
    * Add get_status to proxy (#78, #612)
    * Daemon option (#409)
    * Add a build option to disable including Eigen (#542)
    * Fix compile error in Mac OS X (#553)
    * online model copy in classifier (#590)
    * Check permission of directory specified by --datadir (#599)
    * Raise exception if unsupported mix strategy name is specified (#611, #600)
    * Disabling mix if parameter 0 is given (#623)
    * Validate range of command-line option (#626)
    * #270, #477, #574, #622

* Bugfix
    * Segfault related with ZooKeeper connection (#483, #486, #484)
    * fixed_size_heap may cause access violation (#573)
    * save/load of nearest_neighbor does not work correctly (#567)
    * nearest_neighbor_num is set to reverse_nearest_neighbor_num in lof (#563)
    * Not work correctly when specified --rpc-port=0 (#605)


Release 0.5.0 - 2013/11/22
--------------------------

* New machine learning service
    * Distributed Nearest Neighbor (jubanearest_neighbor)
    * Distributed Clustering (jubaclustering)

* New Features
    * Load model data on server startup (#222, #65)
    * Binary feature in datum (#137, #473)
    * Provide jubadump command (#178)

      * https://github.com/jubatus/jubadump

* Backward Incompatible Changes
    * New model data format (#400, #475)
    * Rename jubakeeper to jubaproxy (#260, #451)
    * Unify config parameter name to "hash_num", old names were bit_num, bitnum and lsh_num (#446, #454)
    * New client interface

* Improvements
    * Replace re2 with oniguruma regular expression library (#361, #465, #471, #494)
    * Remove pficommon dependency (#467)
    * Activate options for distributed mode in standalone mode (#445)
    * Report redundant configuration as error (#484, #489)
    * Optional configuration (#134, #491)
    * Action that actor node is deleted from zookeeper should means server shutdown (#492, #499)
    * Show cause of error when MIX is failed (#478)
    * Stop loading invalid model file (#468)
    * Output logs that server was stopped by signal (#505, #515)
    * Jubatus servers should be terminated gracefully when ZK session expired (#504, #519)
    * Do not install proxy related headers when disabled zookeeper built (#517, #526)
    * jenerator (#174, #205, #306, #357, #359, #378, #399, #402, #431, #432, #434, #453, #456, #466, #501)
    * #412, #450, #447, #438, #426

* Bugfix
    * Make complete_row not weight similarities (#413, #452)
    * Fix corrupted DLOG output in zk::remove (#423, #424)
    * C++ client round down the timeout (#269)
    * Fix invalid use of iterator in inverted_index_storage (#498)
    * Check if a plugin dose not return null (#44


Release 0.4.5 - 2013/07/22
--------------------------

* Improvements
    * Add options to specify ZooKeeper/Interconnect timeout to server/keeper (#344)
    * Test failure on 32bit ubuntu (#349, #396)
    * Fix jenerate install path (#355, #404)
    * Change error message when mecab.h is not found (#408)
    * Fix include file (#169, #410)
    * #416

* Bugfix
    * Implement graceful exit  (#317, #411, #419)
    * Fix mix interval_{sec,count} added by 1 (#340)
    * Fix get_status returns uninitialized values issue (#377, #406)
    * Generate random vectors in make_random. fixes (#398, #401)
    * Fix insufficient -l flags in jubatus.pc (#403)
    * Fix linear_mixer.ticktime returns not integer  (#418)
    * #394


Release 0.4.4 - 2013/06/21
--------------------------

* Improvements
    * Refactor (#323, #332, #367)
    * Make juba.*keeper to allow 30 seconds or over as timeout time (#330)
    * New jenerator (#373)
    * Add CONTRIBUTING.md (#376, #387)
    * #188, #322, #333, #354, #390

* Bugfix
    * Fix jubavisor to detect child process' exec failure (#215, #362)
    * Eliminate deadlock (#329, #331, #375)
    * Fix jubactl's save and load (#342, #383, #384)
    * Fix jubactl's segfault when using -N option (#363, #388)
    * #315, #319, #320, #327, #348, #358, #381, #385, #386

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
