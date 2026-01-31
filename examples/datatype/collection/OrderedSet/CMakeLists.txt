# OrderedManagedSet tests (non-trivial types with lifecycle management)
cm_example_project("DataType/Collection/OrderedSet" OrderedManagedSetSimpleExample		OrderedManagedSetSimpleExample.cpp)
cm_example_project("DataType/Collection/OrderedSet" OrderedManagedSetLifecycleOwnership	OrderedManagedSetLifecycleOwnership.cpp)

# FlatOrderedValueSet tests (trivial types, contiguous memory, serialization-friendly)
cm_example_project("DataType/Collection/OrderedSet" FlatOrderedValueSetBasicOperations		FlatOrderedValueSetBasicOperations.cpp)
cm_example_project("DataType/Collection/OrderedSet" FlatOrderedValueSetBatchAndDuplicates	FlatOrderedValueSetBatchAndDuplicates.cpp)
cm_example_project("DataType/Collection/OrderedSet" FlatOrderedValueSetComparisonAndCopy	FlatOrderedValueSetComparisonAndCopy.cpp)
cm_example_project("DataType/Collection/OrderedSet" FlatOrderedValueSetConstCorrectness	FlatOrderedValueSetConstCorrectness.cpp)

# Performance comparison between OrderedSet (BTree) and FlatOrderedValueSet (Vector)
cm_example_project("DataType/Collection/OrderedSet" OrderedSetPerformanceComparison OrderedSetPerformanceComparison.cpp)

