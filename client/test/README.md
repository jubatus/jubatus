C++ client test
============

test_gtest.py is a C++ Jubatus client test(gtest) runner for jubatest.
In envdef.py, specify jubatest.constants as a key and set path to value.

``` envdef.py
env.param('CPP_GTEST', {
    'anomaly': '/path/to/cpp/build/anomaly_test',
    'classifier': '/path/suma/cpp/build/classifier_test',
    'clustering': '/path/suma/cpp/build/clustering_test',
    'graph': '/path/suma/cpp/build/graph_test',
    'recommender': '/path/suma/cpp/build/recommender_test',
    'regression': '/path/suma/cpp/build/regression_test',
    'nearest_neighbor': '/path/suma/cpp/build/nearest_neighbor_test',
    'stat': '/path/suma/cpp/build/stat_test',
    })
```

