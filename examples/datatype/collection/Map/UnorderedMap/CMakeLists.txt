# UnorderedMap<K,V> tests (Hash-based using ankerl::unordered_dense::map, O(1) average operations)
cm_datatype_test("DataType/Collection/Map/UnorderedMap" UnorderedMapBasicOperations        UnorderedMapBasicOperations.cpp)
cm_datatype_test("DataType/Collection/Map/UnorderedMap" UnorderedMapFind                   UnorderedMapFind.cpp)
cm_datatype_test("DataType/Collection/Map/UnorderedMap" UnorderedMapEnum                   UnorderedMapEnum.cpp)
cm_datatype_test("DataType/Collection/Map/UnorderedMap" UnorderedMapArraySerialization     UnorderedMapArraySerialization.cpp)
cm_datatype_test("DataType/Collection/Map/UnorderedMap" UnorderedMapEdgeCases              UnorderedMapEdgeCases.cpp)
cm_datatype_test("DataType/Collection/Map/UnorderedMap" UnorderedMapComplexTypes           UnorderedMapComplexTypes.cpp)
cm_example_project_base(
    PROJECT_NAME UnorderedMapSerializationAndBenchmark
    FOLDER_PATH "Examples/CMCore/DataType/Collection/Map/UnorderedMap"
    SOURCES UnorderedMapSerializationAndBenchmark.cpp
    PRIVATE_LIBRARIES CMCore
)
if(BUILD_TESTING)
    add_test(NAME UnorderedMapSerializationAndBenchmark COMMAND $<TARGET_FILE:UnorderedMapSerializationAndBenchmark>)
endif()

# Move Semantics Test
cm_datatype_test("DataType/Collection/Map/UnorderedMap" UnorderedMapMoveSemantics         UnorderedMapMoveSemantics.cpp)
