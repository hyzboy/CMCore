########################################
# ValueBuffer 综合测试套件
# 
# 本目录包含 ValueBuffer 模板的全面测试：
# 1. ComprehensiveTest - 全面功能测试
# 2. NonTrivialTypeTest - 非平凡类型测试
# 3. BoundaryAndPerformanceTest - 边界和性能测试
########################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

cm_example_project("DataType/Collection/ValueBuffer" ValueBufferNewTest             ValueBufferNewTest.cpp)
cm_example_project("DataType/Collection/ValueBuffer" ValueBufferTest                ValueBufferTest.cpp)
cm_example_project("DataType/Collection/ValueBuffer" ValueBufferTestEnhanced        ValueBufferTestEnhanced.cpp)
cm_example_project("DataType/Collection/ValueBuffer" ValueBufferBugDemo             ValueBufferBugDemo.cpp)

# 综合功能测试
cm_example_project("DataType/Collection/ValueBuffer" ValueBufferComprehensiveTest   ComprehensiveTest.cpp)


# 边界案例和性能测试
cm_example_project("DataType/Collection/ValueBuffer" ValueBufferBoundaryTest        BoundaryAndPerformanceTest.cpp)
