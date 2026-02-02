

cm_example_project("DataType/Collection" CollectionTest                 CollectionTest.cpp)
cm_example_project("DataType/Collection" CollectionTestComprehensive    CollectionTestComprehensive.cpp)
cm_example_project("DataType/Collection" QuickTest                      QuickTest.cpp)
cm_example_project("DataType/Collection" MinimalTest                    MinimalTest.cpp)

add_subdirectory(stack)
add_subdirectory(OrderedSet)
add_subdirectory(UnorderedSet)
add_subdirectory(Array)
add_subdirectory(Queue)
add_subdirectory(Map)
add_subdirectory(IndexedList)

cm_example_project("DataType/Collection" StringSetTest          StringSetTest.cpp)
cm_example_project("DataType/Collection" StringListTest         StringListTest.cpp)
cm_example_project("DataType/Collection" GetLastTest            GetLastTest.cpp)
cm_example_project("DataType/Collection" BlockAllocatorTest     BlockAllocatorTest.cpp)
cm_example_project("DataType/Collection" BlockAllocatorTest2    BlockAllocatorTest2.cpp)
cm_example_project("DataType/Collection" MonotonicIDListTest    MonotonicIDListTest.cpp)
cm_example_project("DataType/Collection" FlatTreeTest           FlatTreeTest.cpp)

cm_example_project("DataType/Collection" FixedValuePoolTest     FixedValuePoolTest.cpp)
cm_example_project("DataType/Collection" PointerObjectPoolTest  PointerObjectPoolTest.cpp)
cm_example_project("DataType/Collection" SimpleSeriesPoolTest   SimpleSeriesPoolTest.cpp)
cm_example_project("DataType/Collection" SeriesPoolTest         SeriesPoolTest.cpp)

cm_example_project("DataType/Collection" ObjectManagerTest      ObjectManagerTest.cpp)

# Move Semantics Tests (comprehensive test suite for all container move semantics)
cm_example_project("DataType/Collection" ComprehensiveMoveSemanticsTest  ComprehensiveMoveSemanticsTest.cpp)