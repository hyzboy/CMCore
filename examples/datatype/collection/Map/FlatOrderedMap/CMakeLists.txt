# FlatOrderedMap<K,V> tests (Vector-based, contiguous memory, serialization-friendly)
cm_datatype_test("DataType/Collection/Map/FlatOrderedMap" FlatOrderedMapBasicOperations     FlatOrderedMapBasicOperations.cpp)
cm_datatype_test("DataType/Collection/Map/FlatOrderedMap" FlatOrderedMapFind                FlatOrderedMapFind.cpp)
cm_datatype_test("DataType/Collection/Map/FlatOrderedMap" FlatOrderedMapEnum                FlatOrderedMapEnum.cpp)
cm_datatype_test("DataType/Collection/Map/FlatOrderedMap" FlatOrderedMapSerialization       FlatOrderedMapSerialization.cpp)
cm_datatype_test("DataType/Collection/Map/FlatOrderedMap" FlatOrderedMapIndexAccess         FlatOrderedMapIndexAccess.cpp)
cm_datatype_test("DataType/Collection/Map/FlatOrderedMap" FlatOrderedMapEdgeCases           FlatOrderedMapEdgeCases.cpp)
cm_example_project_base(
    PROJECT_NAME FlatOrderedMapComprehensive
    FOLDER_PATH "Examples/CMCore/DataType/Collection/Map/FlatOrderedMap"
    SOURCES FlatOrderedMapTest.cpp
    PRIVATE_LIBRARIES CMCore
)
if(BUILD_TESTING)
    add_test(NAME FlatOrderedMapComprehensive COMMAND $<TARGET_FILE:FlatOrderedMapComprehensive>)
endif()

# Move Semantics Test
cm_datatype_test("DataType/Collection/Map/FlatOrderedMap" FlatOrderedMapMoveSemantics      FlatOrderedMapMoveSemantics.cpp)
