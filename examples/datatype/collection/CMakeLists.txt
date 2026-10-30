

cm_datatype_test("DataType/Collection" CollectionTest                 CollectionTest.cpp)
cm_datatype_test("DataType/Collection" CollectionTestComprehensive    CollectionTestComprehensive.cpp)
cm_datatype_test("DataType/Collection" QuickTest                      QuickTest.cpp)
cm_datatype_test("DataType/Collection" MinimalTest                    MinimalTest.cpp)

add_subdirectory(stack)
add_subdirectory(OrderedSet)
add_subdirectory(UnorderedSet)
add_subdirectory(Array)
add_subdirectory(Queue)
add_subdirectory(Map)
add_subdirectory(IndexedList)
add_subdirectory(String)

cm_datatype_test("DataType/Collection" StringSetTest          StringSetTest.cpp)
cm_datatype_test("DataType/Collection" StringListTest         StringListTest.cpp)
cm_datatype_test("DataType/Collection" GetLastTest            GetLastTest.cpp)
cm_datatype_test("DataType/Collection" BlockAllocatorTest     BlockAllocatorTest.cpp)
cm_datatype_test("DataType/Collection" BlockAllocatorTest2    BlockAllocatorTest2.cpp)
cm_datatype_test("DataType/Collection" MonotonicIDListTest    MonotonicIDListTest.cpp)
cm_datatype_test("DataType/Collection" FlatTreeTest           FlatTreeTest.cpp)
cm_datatype_test("DataType/Collection" FlatTreeContainerTest  FlatTreeContainerTest.cpp)

cm_datatype_test("DataType/Collection" FixedValuePoolTest     FixedValuePoolTest.cpp)
cm_datatype_test("DataType/Collection" PointerObjectPoolTest  PointerObjectPoolTest.cpp)
cm_datatype_test("DataType/Collection" SimpleSeriesPoolTest   SimpleSeriesPoolTest.cpp)
cm_datatype_test("DataType/Collection" SeriesPoolTest         SeriesPoolTest.cpp)

cm_datatype_test("DataType/Collection" ObjectManagerTest      ObjectManagerTest.cpp)

cm_datatype_test("DataType/Collection" RefObjectPoolTest      RefObjectPoolTest.cpp)
cm_datatype_test("DataType/Collection" ActiveObjectManagerTest ActiveObjectManagerTest.cpp)
cm_datatype_test("DataType/Collection" SmartTest               SmartTest.cpp)

cm_datatype_test("DataType/Collection" ValueKVMapTest          ValueKVMapTest.cpp)
cm_datatype_test("DataType/Collection" BitsArrayTest           BitsArrayTest.cpp)
cm_datatype_test("DataType/Collection" GradientTest            GradientTest.cpp)
cm_datatype_test("DataType/Collection" VersionDataTest         VersionDataTest.cpp)
cm_datatype_test("DataType/Collection" AccumMemoryManagerTest  AccumMemoryManagerTest.cpp)
cm_datatype_test("DataType/Collection" BlockPoolTest           BlockPoolTest.cpp)
cm_datatype_test("DataType/Collection" PtrArrayTest            PtrArrayTest.cpp)

cm_datatype_test("DataType/Collection" TypedCollectionTest    TypedCollectionTest.cpp)

# Move Semantics Tests (comprehensive test suite for all container move semantics)
cm_datatype_test("DataType/Collection" ComprehensiveMoveSemanticsTest  ComprehensiveMoveSemanticsTest.cpp)