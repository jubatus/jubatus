C++ client test
============

test_gtest.py is a C++ Jubatus client test(gtest) runner for jubatest.
In envdef.py, specify jubatest.constants as a key and set path to value.

``` envdef.py
path_to_build_dir = '/path/to/cpp/build'
env.param('CPP_GTEST', {
    ANOMALY: path_to_build_dir + '/anomaly_test',
    CLASSIFIER : path_to_build_dir + '/classifier_test',
    CLUSTERING: path_to_build_dir + '/clustering_test',
    GRAPH: path_to_build_dir + '/graph_test',
    RECOMMENDER: path_to_build_dir + '/recommender_test',
    REGRESSION: path_to_build_dir + '/regression_test',
    NEAREST_NEIGHBOR: path_to_build_dir + '/nearest_neighbor_test',
    STAT: path_to_build_dir + '/stat_test',
})
```

