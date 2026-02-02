# OrderedSet tests (BTree implementation, O(log n) operations)
cm_example_project("DataType/Collection/OrderedSet" OrderedSetBasicOperations				OrderedSetBasicOperations.cpp)
cm_example_project("DataType/Collection/OrderedSet" OrderedSetBatchOperations				OrderedSetBatchOperations.cpp)
cm_example_project("DataType/Collection/OrderedSet" OrderedSetIterators					OrderedSetIterators.cpp)
cm_example_project("DataType/Collection/OrderedSet" OrderedSetComparison					OrderedSetComparison.cpp)
cm_example_project("DataType/Collection/OrderedSet" OrderedSetComplexTypes					OrderedSetComplexTypes.cpp)
cm_example_project("DataType/Collection/OrderedSet" OrderedSetEdgeCases					OrderedSetEdgeCases.cpp)

# FlatOrderedSet tests (trivial types, contiguous memory, serialization-friendly)
cm_example_project("DataType/Collection/OrderedSet" FlatOrderedSetBasicOperations			FlatOrderedSetBasicOperations.cpp)
cm_example_project("DataType/Collection/OrderedSet" FlatOrderedSetBatchAndDuplicates		FlatOrderedSetBatchAndDuplicates.cpp)
cm_example_project("DataType/Collection/OrderedSet" FlatOrderedSetComparisonAndCopy		FlatOrderedSetComparisonAndCopy.cpp)
cm_example_project("DataType/Collection/OrderedSet" FlatOrderedSetConstCorrectness		FlatOrderedSetConstCorrectness.cpp)

# Performance comparison between OrderedSet (BTree) and FlatOrderedSet (Vector)
cm_example_project("DataType/Collection/OrderedSet" OrderedSetVsFlatOrderedSetPerformance	OrderedSetVsFlatOrderedSetPerformance.cpp)

# Move Semantics Test (covers both OrderedSet and FlatOrderedSet)
cm_example_project("DataType/Collection/OrderedSet" OrderedSetMoveSemantics               OrderedSetMoveSemantics.cpp)

