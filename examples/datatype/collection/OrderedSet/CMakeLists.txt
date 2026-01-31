# OrderedManagedSet tests (non-trivial types with lifecycle management)
cm_example_project("DataType/Collection/OrderedSet" OrderedManagedSetSimpleExample		OrderedManagedSetSimpleExample.cpp)
cm_example_project("DataType/Collection/OrderedSet" OrderedManagedSetLifecycleOwnership	OrderedManagedSetLifecycleOwnership.cpp)

# FlatOrderedSet tests (trivial types, contiguous memory, serialization-friendly)
cm_example_project("DataType/Collection/OrderedSet" FlatOrderedSetBasicOperations			FlatOrderedSetBasicOperations.cpp)
cm_example_project("DataType/Collection/OrderedSet" FlatOrderedSetBatchAndDuplicates		FlatOrderedSetBatchAndDuplicates.cpp)
cm_example_project("DataType/Collection/OrderedSet" FlatOrderedSetComparisonAndCopy		FlatOrderedSetComparisonAndCopy.cpp)
cm_example_project("DataType/Collection/OrderedSet" FlatOrderedSetConstCorrectness		FlatOrderedSetConstCorrectness.cpp)

# Performance comparison between OrderedSet (BTree) and FlatOrderedSet (Vector)
cm_example_project("DataType/Collection/OrderedSet" OrderedSetVsFlatOrderedSetPerformance	OrderedSetVsFlatOrderedSetPerformance.cpp)

