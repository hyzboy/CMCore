# OrderedSet tests (BTree implementation, O(log n) operations)
cm_datatype_test("DataType/Collection/OrderedSet" OrderedSetBasicOperations				OrderedSetBasicOperations.cpp)
cm_datatype_test("DataType/Collection/OrderedSet" OrderedSetBatchOperations				OrderedSetBatchOperations.cpp)
cm_datatype_test("DataType/Collection/OrderedSet" OrderedSetIterators					OrderedSetIterators.cpp)
cm_datatype_test("DataType/Collection/OrderedSet" OrderedSetComparison					OrderedSetComparison.cpp)
cm_datatype_test("DataType/Collection/OrderedSet" OrderedSetComplexTypes					OrderedSetComplexTypes.cpp)
cm_datatype_test("DataType/Collection/OrderedSet" OrderedSetEdgeCases					OrderedSetEdgeCases.cpp)

# FlatOrderedSet tests (trivial types, contiguous memory, serialization-friendly)
cm_datatype_test("DataType/Collection/OrderedSet" FlatOrderedSetBasicOperations			FlatOrderedSetBasicOperations.cpp)
cm_datatype_test("DataType/Collection/OrderedSet" FlatOrderedSetBatchAndDuplicates		FlatOrderedSetBatchAndDuplicates.cpp)
cm_datatype_test("DataType/Collection/OrderedSet" FlatOrderedSetComparisonAndCopy		FlatOrderedSetComparisonAndCopy.cpp)
cm_datatype_test("DataType/Collection/OrderedSet" FlatOrderedSetConstCorrectness		FlatOrderedSetConstCorrectness.cpp)

# Performance comparison between OrderedSet (BTree) and FlatOrderedSet (Vector)
cm_example_project("DataType/Collection/OrderedSet" OrderedSetVsFlatOrderedSetPerformance	OrderedSetVsFlatOrderedSetPerformance.cpp)

# Move Semantics Test (covers both OrderedSet and FlatOrderedSet)
cm_datatype_test("DataType/Collection/OrderedSet" OrderedSetMoveSemantics               OrderedSetMoveSemantics.cpp)

