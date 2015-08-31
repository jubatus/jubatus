# ----------------------------- #
#  Test Environment Definition  #
# ----------------------------- #

###
### Jubatus Configuration
###
env.prefix('/opt/jubatus')

###
### Test Node Configuration
###
env.node('127.0.0.1', range(19199,19299))

###
### ZooKeeper Configuration
###
env.zookeeper('127.0.0.1', 2181)

###
### Miscellaneous Node Configuration
###
env.workdir('/tmp')
env.cluster_prefix('jubatus_client_test')

###
### Test Parameters
path_to_build_dir = './build'
env.param('CPP_GTEST', {
    ANOMALY:            path_to_build_dir + '/anomaly_test',
    CLASSIFIER :        path_to_build_dir + '/classifier_test',
    CLUSTERING:         path_to_build_dir + '/clustering_test',
    GRAPH:              path_to_build_dir + '/graph_test',
    RECOMMENDER:        path_to_build_dir + '/recommender_test',
    REGRESSION:         path_to_build_dir + '/regression_test',
    NEAREST_NEIGHBOR:   path_to_build_dir + '/nearest_neighbor_test',
    STAT:               path_to_build_dir + '/stat_test',
})
