cm_example_project_base(
    PROJECT_NAME MapBenchmark
    FOLDER_PATH "Examples/CMCore/DataType/Collection/Map"
    SOURCES MapBenchmark.cpp
    PRIVATE_LIBRARIES CMCore
)

cm_example_project_base(
    PROJECT_NAME UnorderedMapTest
    FOLDER_PATH "Examples/CMCore/DataType/Collection/Map"
    SOURCES UnorderedMapTest.cpp
    PRIVATE_LIBRARIES CMCore
)

cm_example_project("DataType/Collection/Map" MapTest                MapTest.cpp)
cm_example_project("DataType/Collection/Map" OrderedMapTest         OrderedMapTest.cpp)
cm_example_project("DataType/Collection/Map" FlatOrderedMapTest     FlatOrderedMapTest.cpp)
