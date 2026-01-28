
cm_example_project_base(
    PROJECT_NAME MapBenchmark
    FOLDER_PATH "Examples/CMCore/DataType/Collection"
    SOURCES MapBenchmark.cpp
    PRIVATE_LIBRARIES CMCore
)

cm_example_project("DataType/Collection" CollectionTest                 CollectionTest.cpp)
cm_example_project("DataType/Collection" CollectionTestComprehensive    CollectionTestComprehensive.cpp)
cm_example_project("DataType/Collection" QuickTest                      QuickTest.cpp)
cm_example_project("DataType/Collection" MinimalTest                    MinimalTest.cpp)
cm_example_project("DataType/Collection" FixVerificationTest            FixVerificationTest.cpp)

add_subdirectory(stack)
add_subdirectory(ValueBuffer)
add_subdirectory(OrderedSet)
add_subdirectory(UnorderedSet)
add_subdirectory(Array)
add_subdirectory(Queue)

cm_example_project("DataType/Collection" QueueTest              QueueTest.cpp)
cm_example_project("DataType/Collection" ValuePoolTest          ValuePoolTest.cpp)
cm_example_project("DataType/Collection" ManagedPoolTest        ManagedPoolTest.cpp)
cm_example_project("DataType/Collection" MapTest                MapTest.cpp)
cm_example_project("DataType/Collection" OrderedMapTest         OrderedMapTest.cpp)
cm_example_project("DataType/Collection" StringSetTest          StringSetTest.cpp)
cm_example_project("DataType/Collection" StringListTest         StringListTest.cpp)
cm_example_project("DataType/Collection" GetLastTest            GetLastTest.cpp)
cm_example_project("DataType/Collection" BlockAllocatorTest     BlockAllocatorTest.cpp)
cm_example_project("DataType/Collection" BlockAllocatorTest2    BlockAllocatorTest2.cpp)
cm_example_project("DataType/Collection" MonotonicIDListTest    MonotonicIDListTest.cpp)
cm_example_project("DataType/Collection" FlatTreeTest           FlatTreeTest.cpp)
