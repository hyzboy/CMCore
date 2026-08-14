include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ============================================================================
# datatype 测试专用宏：
#   在 cm_example_project 基础上注册 CTest 测试项（正确性测试用）。
#   benchmark / demo 目标仍使用 cm_example_project，不注册进 CTest。
# ============================================================================
macro(cm_datatype_test sub_folder project_name)
    cm_example_project(${sub_folder} ${project_name} ${ARGN})
    if(BUILD_TESTING)
        add_test(NAME ${project_name} COMMAND $<TARGET_FILE:${project_name}>)
    endif()
endmacro()

cm_datatype_test("DataType" UbyteColor            color_test_example.cpp)
cm_datatype_test("DataType" SplitStringTest       SplitStringTest.cpp)
cm_datatype_test("DataType" StrChrTest            strchr_test.cpp)
cm_datatype_test("DataType" Size2Test             Size2Test.cpp)
cm_datatype_test("DataType" MemcmpTest            MemcmpTest.cpp)
cm_datatype_test("DataType" IDNameTest            IDNameTest.cpp)
cm_datatype_test("DataType" IDNameStressTest      IDNameStressTest.cpp)
cm_datatype_test("DataType" IDObjectManagerTest   IDObjectManagerTest.cpp)
cm_datatype_test("DataType" TypedBlockAllocatorTest TypedBlockAllocatorTest.cpp)

cm_datatype_test("DataType/ActiveManager" 1_ActiveIDManagerTest           ActiveIDManagerTest.cpp)
cm_datatype_test("DataType/ActiveManager" 2_ActiveIDManagerGetIdleViewTest ActiveIDManagerGetIdleViewTest.cpp)
cm_datatype_test("DataType/ActiveManager" 3_ActiveMemoryBlockManagerTest  ActiveMemoryBlockManagerTest.cpp)
cm_datatype_test("DataType/ActiveManager" 4_ActiveDataManagerTest2Staged  ActiveDataManagerTest2Staged.cpp)
cm_datatype_test("DataType/ActiveManager" 5_ActiveDataManagerTest         ActiveDataManagerTest.cpp)
cm_datatype_test("DataType/ActiveManager" 6_ActiveIDManagerEnhancedTest   ActiveIDManagerEnhancedTest.cpp)

add_subdirectory(collection)
add_subdirectory(ConstStringSet)
