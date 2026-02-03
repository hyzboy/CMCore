## FlatUnorderedSet / UnorderedManagedSet tests (aligned with other examples)

cm_example_project("DataType/Collection/UnorderedSet" FlatUnorderedSetSimple                    FlatUnorderedSetSimple.cpp)
cm_example_project("DataType/Collection/UnorderedSet" FlatUnorderedSetBasicOperations           FlatUnorderedSetBasicOperations.cpp)
cm_example_project("DataType/Collection/UnorderedSet" FlatUnorderedSetSetOperations             FlatUnorderedSetSetOperations.cpp)
cm_example_project("DataType/Collection/UnorderedSet" FlatUnorderedSetComparisonOperators       FlatUnorderedSetComparisonOperators.cpp)

cm_example_project("DataType/Collection/UnorderedSet" FlatUnorderedSetDualHashSimple            FlatUnorderedSetDualHashSimple.cpp)
cm_example_project("DataType/Collection/UnorderedSet" FlatUnorderedSetOpenAddressSimple         FlatUnorderedSetOpenAddressSimple.cpp)
cm_example_project("DataType/Collection/UnorderedSet" FlatUnorderedSetShardedSimple             FlatUnorderedSetShardedSimple.cpp)

cm_example_project("DataType/Collection/UnorderedSet" FlatUnorderedSetVariantsBenchmark         FlatUnorderedSetVariantsBenchmark.cpp)
cm_example_project("DataType/Collection/UnorderedSet" FlatUnorderedSetVariantsCorrectnessTest   FlatUnorderedSetVariantsCorrectnessTest.cpp)

## FlatUnorderedSet compact/performance tests
cm_example_project("DataType/Collection/UnorderedSet" FlatUnorderedSetCompactAndPerformance     FlatUnorderedSetCompactAndPerformance.cpp)

## UnorderedSet tests and FlatUnorderedSet vs UnorderedSet comparison
cm_example_project("DataType/Collection/UnorderedSet" UnorderedSetBasicAndComparison            UnorderedSetBasicAndComparison.cpp)

## UnorderedSet independent tests
cm_example_project("DataType/Collection/UnorderedSet" UnorderedSetSimple                       UnorderedSetSimple.cpp)
cm_example_project("DataType/Collection/UnorderedSet" UnorderedSetBasicOperations              UnorderedSetBasicOperations.cpp)
cm_example_project("DataType/Collection/UnorderedSet" UnorderedSetSetOperations                UnorderedSetSetOperations.cpp)
cm_example_project("DataType/Collection/UnorderedSet" UnorderedSetComparisonOperators          UnorderedSetComparisonOperators.cpp)

# Move Semantics Test (covers both UnorderedSet and FlatUnorderedSet)
cm_example_project("DataType/Collection/UnorderedSet" UnorderedSetMoveSemantics                UnorderedSetMoveSemantics.cpp)
