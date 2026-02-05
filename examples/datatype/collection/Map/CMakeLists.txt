cm_example_project_base(
    PROJECT_NAME MapBenchmark
    FOLDER_PATH "Examples/CMCore/DataType/Collection/Map"
    SOURCES MapBenchmark.cpp
    PRIVATE_LIBRARIES CMCore
)

# Include subdirectories for each Map type
add_subdirectory(BidirectionalMap)
add_subdirectory(FlatOrderedMap)
add_subdirectory(OrderedMap)
add_subdirectory(UnorderedMap)
