Release 1.0.3 - 2017/04/24
--------------------------

* Changes to Supported Systems
    * Deprecate Ubuntu 12.04 LTS support (#1189)

* Improvements
    * Validate save/load RPC arguments (#1188, #1192)
    * Model files generated in earlier versions of Jubatus can now be loaded (#1179, #1187)
    * Improve packaging process (#1185, #1186)

* Bug fixes
    * classifier/regression: fix performance regression in standalone mode (#1190, #1193)

Release 1.0.2 - 2017/02/27
--------------------------

* Changes to Supported Systems
    * Ubuntu 16.04 LTS is now officially supported (#1180, #1181)

Release 1.0.1 - 2016/12/26
--------------------------

* Improvements
    * Support OpenCV 2.0 in image extraction plugin (#1168, #1174, #1175)
    * fv_converter plugin loading code moved to Jubatus Core (#1171)
    * Improve Dockerfile (#1170, #1172, #1173)

Release 1.0.0 - 2016/10/31
--------------------------

* New Features
    * Add image extraction plug-in (#843, #1150)
    * Add configuration validation feature (#973, #1144)

* Improvements
    * clustering: change APIs to accept ID-based datum (#1155)
    * Update Dockerfile to use Ubuntu 14.04 (#1146)
    * Improve ``--daemon`` option of server and proxy to detach from console (#1072, #1148)
    * classifier: models are now compatible in standalone and distributed mode (#469, #1153)
    * Change the default log level to INFO instead of DEBUG (#1152)
    * Update example configurations (#1165)
    * Update manpages (#1167)
    * Update packaging tools (#1157)

* Bug fixes
    * graph: fix IDs being reused when loading model file to other server instance in standalone mode (#1149, #1156)

Release 0.9.4 - 2016/08/29
--------------------------

* Improvements
    * Improve error detection of log4cxx config file (#891, #1142)
    * Print DEBUG logs by default (#947, #1143)
    * Support printing logs in UTF-8 (#1078, #1140)
    * Improve comment in wscript (#1138, #1139)

* Bug fixes
    * Fix missing locks in mixer (#1092, #1135)
    * Fix RPM packaging tool to work on RHEL 6.x (#1137)

Release 0.9.3 - 2016/07/25
--------------------------

* Improvements
    * Improve behavior when ZooKeeper log files cannot be reopened (#1026, #1133)
    * Remove Glibc C++ debug flag in debug mode (#910, #1136)
    * Improve packaging tools to remove dependency to Google Code (#980, #1130)

* Bug fixes
    * weight: fix to display config on startup (#1131, #1132)
    * Fix missing lock in get_status of linear_mixer (#1093, #1134)

Release 0.9.2 - 2016/06/27
--------------------------

* Improvements
    * Update README.rst (#1126, #1129)

* Bug fixes
    * Fix `weight_client.hpp` not included in `client.hpp` (#1127, #1128)

Release 0.9.1 - 2016/05/30
--------------------------

* New feature
    * weight: server that returns raw results of fv_converter (#1105, #1116)

* Improvements
    * Add `default.json` config files (#1117, #1120)
    * Add example config file for Anomaly (lof with unlearner) (#1118)
    * Update manpage (#1123)

* Bug fixes
    * Fix thread safetiness issue in save/load RPC (#1113, #1115)
    * Fix `dl` link error on some distributions (#1121, #1122)

Release 0.9.0 - 2016/04/25
--------------------------

* New feature
    * `jubakit <https://github.com/jubatus/jubakit>`_: Python module to easily evaluate precision of models (#1044)

* Changes to Supported Systems
    * Ubuntu 14.04 LTS is now officially supported (#1071)

* Improvements
    * classifier: improve get_labels API to return number of trained instances (#1109)
    * Add example configuration file for Nearest Neighbor (multi-threaded LSH) (#1111)
    * Add example configuration file for Classifier (cosine / euclidean) (#1108)
    * Add example configuration file for Recommender (euclid_lsh with unlearner) (#1108)
    * Configuration in model is used when loading model file on startup (#1081, #1107)
    * Improve compatibility with FreeBSD 9 (#1099)
    * Update manpages (#1112)
    * Code cleanup (#1102)

* Bug fixes
    * Fix jenerator test (#1103)

Release 0.8.9 - 2016/03/28
--------------------------

* Improvements
    * Support extracting specific part-of-speech in MeCab plugin (#1073, #1098)
    * Improved proxy to log timeout errors (#1048, #1095)

* Bug fixes
    * Fix "jubactl -c save" not working (#1094)

Release 0.8.8 - 2016/02/29
--------------------------

* Improvements
    * Support extracting base form in MeCab plugin (#1074, #1085)
    * Compare configuration string semantically on load (#1080, 1087)
    * Add example configuration file for Recommender (inverted_index_euclid with unlearner) (#1090)
    * Fix email address of DEB package (#1082, #1088)

* Bug fixes
    * Fix printing logs multiple times after receiving SIGHUP (#962, #1084, #1086)

Release 0.8.7 - 2016/01/25
--------------------------

* Improvements
    * Add example configuration file for Recommender (MinHash with unlearner) (#1077)
    * Add example configuration file for Recommender and Anomaly (inverted_index_euclid) (#1079)

Release 0.8.6 - 2015/12/25
--------------------------

* Improvements
    * Support morpheme n-gram in mecab plugin (#1054, #1070)

* Bug fixes
    * Fix typos in exception and log messages (#1068, #1069)

Release 0.8.5 - 2015/11/30
--------------------------

* Improvements
    * improve client tests to work on Python 3 (#1043, #1063)
    * improve build script (#1064)
    * recommender: fix typo of calc_similarity function name (#1065)
    * recommender: add example config for LSH with unlearner (#1066)
    * update man pages (#1067)

* Bug fixes
    * Fix RPM package errors (#1059, #1060, #1061, #1062)

Release 0.8.4 - 2015/10/26
--------------------------

* Improvements
    * nearest_neighbor: improve analysis methods performance by eliminating giant lock (#1051)

* Bug fixes
    * Fix loading model files via command line option not working (#1055, #1057)

Release 0.8.3 - 2015/09/28
--------------------------

* Improvements
    * clustering: add seed option to example configuration files (#1047)
    * Fix packaging tools to use new oniguruma URL (#1045, #1046)

* Bug fixes
    * nearest_neighbor: fix argument type (#1042, #1049, #1050)

Release 0.8.2 - 2015/08/31
--------------------------

* Improvements
    * Add Dockerfile (#904)
    * Support add aggregator in jenerator (#1022, #1040)
    * Migrate ux-trie packaging from Google Code to GitHub (#1037)

* Bug fixes
    * Fix proxies incorrectly create negative cache (#1035, #1036)
    * Fix save RPC creating empty file when empty ID is given (#1038, #1039)
    * clustering: fix ID conflict in distributed mode (#1041)
    * Fix C++ client test conditions (#1019, #1034)

Release 0.8.1 - 2015/07/27
--------------------------

* Improvements
    * Expose status information of linear_mixer (#1030)
    * Support building with Python 3.x (#841, #1025)
    * Add example config file for recommender inverted_index with unlearner (#1031)
    * Code cleanup (#1032)

* Bug fixes
    * classifier: fix MIX not triggered by update count (#1023, #1024)

Release 0.8.0 - 2015/06/29
--------------------------

* Changes to Supported Systems
    * Red Hat Enterprise Linux (RHEL) 7 is now officially supported (#940, #1004)

* Improvements
    * save RPC now returns the path to the saved model file (#562, #971, #1015, #1020)
    * Support non-commutative functions in combination feature plug-in (#999)
    * classifier: improve performance by removing giant lock (#968, #976)
    * Improve push mixer performance (#989, #1008)
    * Improve error handling of proxies (#985, #993)
    * Add man pages for commands (#1005, #1011)
    * Code cleanup (#1003)

* Bug fixes
    * bandit: fix reset API returns false when called before MIX (#994, #1007)
    * bandit: CHT replication level changed to 1 to avoid double counting (#1017)
    * classifier: set_label is now merged using all_and in distributed environment (#1002, #1006)
    * nearest_neighbor: fix to print config on startup (#984, #986)
    * bandit: fix bandit client is not included in client.hpp (#996)
    * Fix push mixer not mixing rows correctly (#990, #991)
    * Fix skip mixer not choosing peers correctly (#915, #995, #997)
    * Fix error reporting of failures occured before initializing logger (#982, #987)
    * Fix sigwait thread race condition (#988, #992)
    * Fix cleanup race condition when ZooKeeper session expiry (#967, #998)
    * Fix IDL files missing from installation (#1009, #1010)
    * Fix C++ client tests not working (#959, #1018)
    * Fix RPM packages does not declare clear version dependency (#933, #1001)
    * Fix RPM packages to correctly generate debuginfo packages (#970, #1000)

Release 0.7.2 - 2015/04/27
--------------------------

* Improvements
    * Improve jubaconv to accept configurations using plug-ins (#963)
    * Add example configuration files for combination feature (#953, #983)
    * bandit: update example configuration files to use assume_unrewarded option (#972)
    * classifier: fix set_label to use broadcast distribution (#850, #979)
    * Improve ZooKeeper connection log on reconnection (#913, #977)

* Bug fixes
    * nearest_neighbor: fix uninitialized status values (#883, #978)
    * anomaly: fix get_all_rows API returning duplicated rows (#974, #975)
    * Fix server does not stop when MIX protocol version mismatch was detected (#955, #961)

Release 0.7.1 - 2015/03/30
--------------------------

* Improvements
    * burst: Add clear API (#919, #946)
    * ZooKeeper logs can be rotated on SIGHUP (#876, #896)
    * get_status and get_diff can work in parallel (#912, #941)
    * jenerator refactoring (#929)
    * Change default ports used for RPC tests (#934)
    * Codes cleanup (#917, #922, #938, #942, #965)
    * Add language declarations to wscript (#925, #930)
    * Update to the latest waf-unittest (#952)

* Bug fixes
    * Fix segmentation fault on MIX failure (#949)
    * Fix RPC server to shutdown gracefully when ZK session expired (#954, #960)
    * Fix signal handling to always accept SIGTERM / SIGINT (#956, #966)
    * Fix proxies to exit with correct status (#928, #936)
    * Fix "stopping mixer thread" log displayed in standalone mode (#916, #935)
    * Fix jenerator test to work (#937)

Release 0.7.0 - 2015/02/23
--------------------------

* New machine learning service
    * Distributed Multi-Armed Bandit (jubabandit) (#932)

* Improvements
    * Add golang client (experimental) (#870, #907, #909, #923, )
    * Add support for combination feature (#925)
    * nearest_neighbor: Implement get_all_rows API (#918)
    * classifier: Add example file for NN-based classifier (#921)
    * Upgrade to MessagePack 0.5.9 (#926)
    * Update copyright (#906, #920, #927)

* Bug fixes
    * Fix memory leak when doing MIX (#914)
    * Fix RPC tests occasionally fails (#905)

Release 0.6.6 - 2014/12/22
--------------------------

* Improvements
    * Detect logger configuration error on startup (#895, #902)

Release 0.6.5 - 2014/11/25
--------------------------

* Improvements
    * Enable libstdc++ debug mode when configured using `--enable-debug` (#887, #889)

* Buf fixes
    * Fix thread ID in logs not printed properly on OS X (#874, #894)
    * Remove unused `--disable-eigen` configure option (#890, #893)

Release 0.6.4 - 2014/10/20
--------------------------

* New machine learning service
    * Distributed Burst Detection (jubaburst)

* Improvements
    * Compatibility verification when joining to cluster (#864, #882)

Release 0.6.3 - 2014/09/29
--------------------------

* Improvements
    * Add jubadump to RPM / DEB package (#856)

* Bug fixes
    * Fix jubactl to work without specifying "-X" option (#872)
    * Fix log4cxx error message displayed on startup (#873)

Release 0.6.2 - 2014/08/25
--------------------------

* Improvements
    * Reload logging configuration on SIGHUP (#838, #861)
    * Display plugin version when loaded (#740, #865)
    * Register itself as active after server is started (#854, #863)
    * Push mixers now excludes himself from MIX peer candidates (#842, #860)
* Bug fixes
    * anomaly: Fix add requets returing error when using light_lof (#849, #853)
    * Fix proxies does not work with push_mixer (#803)
    * Do not block update RPC request while acquiring ZooKeeper lock (#618, #862)
    * Fix IDL errors (#851, #858, #859)
    * Remove unused core_headers reference (#811, #848)

Release 0.6.1 - 2014/07/22
--------------------------

* Improvements
    * Support for OS X Mavericks (#833)
    * Add sanity check configure option (#835)
    * Fix linear_mixer to use interconnect_timeout (#720)
    * Add ABI version number to object file (#812)
    * #825, #832, #847
* Bug fixes
    * Fix timed-out connection instances are left in the pool (#836, #837)

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
