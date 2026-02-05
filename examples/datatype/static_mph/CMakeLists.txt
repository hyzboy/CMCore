# Static Perfect Hash Map Tests

cm_example_project("DataType/StaticPerfectHash" StaticPerfectHashMapPrototype StaticPerfectHashMapPrototype.cpp)
cm_example_project("DataType/StaticPerfectHash" StaticPerfectHashMapScaleTest StaticPerfectHashMapScaleTest.cpp)
cm_example_project("DataType/StaticPerfectHash" StringSetMPHTest StringSetMPHTest.cpp)
cm_example_project("DataType/StaticPerfectHash" ProgressiveMPHTest ProgressiveMPHTest.cpp)
cm_example_project("DataType/StaticPerfectHash" RetryMechanismTest RetryMechanismTest.cpp)

# Static Perfect Hash Set Tests

cm_example_project("DataType/StaticPerfectHash" StaticPerfectHashSetBasicTest StaticPerfectHashSetBasicTest.cpp)
cm_example_project("DataType/StaticPerfectHash" StringSetFileTest StringSetFileTest.cpp)

# Large Scale and Stress Tests

cm_example_project("DataType/StaticPerfectHash" LargeScaleMapTest LargeScaleMapTest.cpp)
cm_example_project("DataType/StaticPerfectHash" LargeScaleSetTest LargeScaleSetTest.cpp)
cm_example_project("DataType/StaticPerfectHash" StressTest StressTest.cpp)
cm_example_project("DataType/StaticPerfectHash" PerformanceBenchmark PerformanceBenchmark.cpp)
