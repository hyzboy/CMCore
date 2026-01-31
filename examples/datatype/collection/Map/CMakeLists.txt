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

cm_example_project("DataType/Collection/Map" UnorderedMapBasicOperations        UnorderedMapBasicOperations.cpp)
cm_example_project("DataType/Collection/Map" OrderedMapFunctionalOrdering       OrderedMapFunctionalOrdering.cpp)
cm_example_project("DataType/Collection/Map" FlatOrderedMapTest                 FlatOrderedMapTest.cpp)
