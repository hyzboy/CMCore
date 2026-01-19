########################################
# DataArray 综合测试套件
# 
# 本目录包含 DataArray 模板的全面测试：
# 1. ComprehensiveTest - 全面功能测试
# 2. NonTrivialTypeTest - 非平凡类型测试
# 3. BoundaryAndPerformanceTest - 边界和性能测试
########################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

cm_example_project("DataType/Collection/DataArray" DataArrayTest          DataArrayTest.cpp)
cm_example_project("DataType/Collection/DataArray" DataArrayTestEnhanced  DataArrayTestEnhanced.cpp)
cm_example_project("DataType/Collection/DataArray" DataArrayBugDemo       DataArrayBugDemo.cpp)

# 综合功能测试
cm_example_project("DataType/Collection/DataArray" DataArrayComprehensiveTest  ComprehensiveTest.cpp)

# 非平凡类型测试
cm_example_project("DataType/Collection/DataArray" DataArrayNonTrivialTest     NonTrivialTypeTest.cpp)

# 边界案例和性能测试
cm_example_project("DataType/Collection/DataArray" DataArrayBoundaryTest      BoundaryAndPerformanceTest.cpp)
