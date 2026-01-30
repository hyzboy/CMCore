# ============================================================================
# 基础功能测试
# ============================================================================

cm_example_project("DataType/Collection/Array" ValueArrayBasicTest                  ValueArrayBasicTest.cpp)
cm_example_project("DataType/Collection/Array" ValueArrayComprehensiveTest         ValueArrayComprehensiveTest.cpp)
cm_example_project("DataType/Collection/Array" ManagedArrayComprehensiveTest       ManagedArrayComprehensiveTest.cpp)
cm_example_project("DataType/Collection/Array" IndexedValueArrayBasicTest          IndexedValueArrayBasicTest.cpp)

# ============================================================================
# 性能和比较测试
# ============================================================================

cm_example_project("DataType/Collection/Array" ValueArrayPerformanceTest                    ValueArrayPerformanceTest.cpp)
cm_example_project("DataType/Collection/Array" IndexedValueArrayVsListComparisonTest      IndexedValueArrayVsListComparisonTest.cpp)

# ============================================================================
# 新增的关键测试 (数组类型、互操作性、压力测试)
# ============================================================================

cm_example_project("DataType/Collection/Array" ArrayTypeSpecializationTest                 ArrayTypeSpecializationTest.cpp)
cm_example_project("DataType/Collection/Array" ContainerInteroperabilityTest              ContainerInteroperabilityTest.cpp)
cm_example_project("DataType/Collection/Array" ArrayStressAndBoundaryTest                 ArrayStressAndBoundaryTest.cpp)
