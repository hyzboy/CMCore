## FlatUnorderedSet / UnorderedManagedSet tests (aligned with other examples)

cm_datatype_test("DataType/Collection/UnorderedSet" FlatUnorderedSetSimple                    FlatUnorderedSetSimple.cpp)
cm_datatype_test("DataType/Collection/UnorderedSet" FlatUnorderedSetBasicOperations           FlatUnorderedSetBasicOperations.cpp)
cm_datatype_test("DataType/Collection/UnorderedSet" FlatUnorderedSetSetOperations             FlatUnorderedSetSetOperations.cpp)
cm_datatype_test("DataType/Collection/UnorderedSet" FlatUnorderedSetComparisonOperators       FlatUnorderedSetComparisonOperators.cpp)

cm_datatype_test("DataType/Collection/UnorderedSet" FlatUnorderedSetDualHashSimple            FlatUnorderedSetDualHashSimple.cpp)
cm_datatype_test("DataType/Collection/UnorderedSet" FlatUnorderedSetOpenAddressSimple         FlatUnorderedSetOpenAddressSimple.cpp)
cm_datatype_test("DataType/Collection/UnorderedSet" FlatUnorderedSetShardedSimple             FlatUnorderedSetShardedSimple.cpp)

cm_example_project("DataType/Collection/UnorderedSet" FlatUnorderedSetVariantsBenchmark         FlatUnorderedSetVariantsBenchmark.cpp)
cm_datatype_test("DataType/Collection/UnorderedSet" FlatUnorderedSetVariantsCorrectnessTest   FlatUnorderedSetVariantsCorrectnessTest.cpp)

## FlatUnorderedSet compact/performance tests
cm_datatype_test("DataType/Collection/UnorderedSet" FlatUnorderedSetCompactAndPerformance     FlatUnorderedSetCompactAndPerformance.cpp)

## UnorderedSet tests and FlatUnorderedSet vs UnorderedSet comparison
cm_datatype_test("DataType/Collection/UnorderedSet" UnorderedSetBasicAndComparison            UnorderedSetBasicAndComparison.cpp)

## UnorderedSet independent tests
cm_datatype_test("DataType/Collection/UnorderedSet" UnorderedSetSimple                       UnorderedSetSimple.cpp)
cm_datatype_test("DataType/Collection/UnorderedSet" UnorderedSetBasicOperations              UnorderedSetBasicOperations.cpp)
cm_datatype_test("DataType/Collection/UnorderedSet" UnorderedSetSetOperations                UnorderedSetSetOperations.cpp)
cm_datatype_test("DataType/Collection/UnorderedSet" UnorderedSetComparisonOperators          UnorderedSetComparisonOperators.cpp)

# Move Semantics Test (covers both UnorderedSet and FlatUnorderedSet)
cm_datatype_test("DataType/Collection/UnorderedSet" UnorderedSetMoveSemantics                UnorderedSetMoveSemantics.cpp)
