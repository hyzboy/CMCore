cm_example_project_base(
    PROJECT_NAME MapBenchmark
    FOLDER_PATH "Examples/CMCore/DataType/Collection/Map"
    SOURCES MapBenchmark.cpp
    PRIVATE_LIBRARIES CMCore
)

cm_example_project_base(
    PROJECT_NAME UnorderedMapSerializationAndBenchmark
    FOLDER_PATH "Examples/CMCore/DataType/Collection/Map"
    SOURCES UnorderedMapSerializationAndBenchmark.cpp
    PRIVATE_LIBRARIES CMCore
)

# OrderedMap<K,V> tests (BTree implementation, O(log n) operations)
cm_example_project("DataType/Collection/Map" OrderedMapBasicOperations         OrderedMapBasicOperations.cpp)
cm_example_project("DataType/Collection/Map" OrderedMapAddUpdate               OrderedMapAddUpdate.cpp)
cm_example_project("DataType/Collection/Map" OrderedMapIndexAccess             OrderedMapIndexAccess.cpp)
cm_example_project("DataType/Collection/Map" OrderedMapFind                    OrderedMapFind.cpp)
cm_example_project("DataType/Collection/Map" OrderedMapIterators               OrderedMapIterators.cpp)
cm_example_project("DataType/Collection/Map" OrderedMapEnum                    OrderedMapEnum.cpp)
cm_example_project("DataType/Collection/Map" OrderedMapEdgeCases               OrderedMapEdgeCases.cpp)

# FlatOrderedMap<K,V> tests (Vector-based, contiguous memory, serialization-friendly)
cm_example_project("DataType/Collection/Map" FlatOrderedMapBasicOperations     FlatOrderedMapBasicOperations.cpp)
cm_example_project("DataType/Collection/Map" FlatOrderedMapFind                FlatOrderedMapFind.cpp)
cm_example_project("DataType/Collection/Map" FlatOrderedMapEnum                FlatOrderedMapEnum.cpp)
cm_example_project("DataType/Collection/Map" FlatOrderedMapSerialization       FlatOrderedMapSerialization.cpp)
cm_example_project("DataType/Collection/Map" FlatOrderedMapIndexAccess         FlatOrderedMapIndexAccess.cpp)
cm_example_project("DataType/Collection/Map" FlatOrderedMapEdgeCases           FlatOrderedMapEdgeCases.cpp)
cm_example_project_base(
    PROJECT_NAME FlatOrderedMapComprehensive
    FOLDER_PATH "Examples/CMCore/DataType/Collection/Map"
    SOURCES FlatOrderedMapTest.cpp
    PRIVATE_LIBRARIES CMCore
)

# UnorderedMap<K,V> tests (Hash-based using ankerl::unordered_dense::map, O(1) average operations)
cm_example_project("DataType/Collection/Map" UnorderedMapBasicOperations        UnorderedMapBasicOperations.cpp)
cm_example_project("DataType/Collection/Map" UnorderedMapFind                   UnorderedMapFind.cpp)
cm_example_project("DataType/Collection/Map" UnorderedMapEnum                   UnorderedMapEnum.cpp)
cm_example_project("DataType/Collection/Map" UnorderedMapArraySerialization     UnorderedMapArraySerialization.cpp)
cm_example_project("DataType/Collection/Map" UnorderedMapEdgeCases              UnorderedMapEdgeCases.cpp)
cm_example_project("DataType/Collection/Map" UnorderedMapComplexTypes           UnorderedMapComplexTypes.cpp)
cm_example_project_base(
    PROJECT_NAME UnorderedMapSerializationAndBenchmark_Comprehensive
    FOLDER_PATH "Examples/CMCore/DataType/Collection/Map"
    SOURCES UnorderedMapSerializationAndBenchmark.cpp
    PRIVATE_LIBRARIES CMCore
)

# BidirectionalMap<K,V> tests (Bidirectional lookup with O(1) both directions)
cm_example_project("DataType/Collection/Map" BidirectionalMapBasicOperations   BidirectionalMapBasicOperations.cpp)
cm_example_project("DataType/Collection/Map" BidirectionalMapFind              BidirectionalMapFind.cpp)
cm_example_project("DataType/Collection/Map" BidirectionalMapIterators         BidirectionalMapIterators.cpp)
cm_example_project("DataType/Collection/Map" BidirectionalMapEnum              BidirectionalMapEnum.cpp)
cm_example_project("DataType/Collection/Map" BidirectionalMapModify            BidirectionalMapModify.cpp)
cm_example_project("DataType/Collection/Map" BidirectionalMapEdgeCases         BidirectionalMapEdgeCases.cpp)

cm_example_project("DataType/Collection/Map" OrderedMapFunctionalOrdering       OrderedMapFunctionalOrdering.cpp)
