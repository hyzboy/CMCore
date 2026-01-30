# Stack 相关测试项目
# 原有核心测试
cm_example_project("DataType/Collection/Stack" StackTest              StackTest.cpp)
cm_example_project("DataType/Collection/Stack" StackPoolTest          StackPoolTest.cpp)

# 重组的综合测试 (包含18个拆散测试的所有用例)
cm_example_project("DataType/Collection/Stack" StackBasicOperations      StackBasicOperations.cpp)
cm_example_project("DataType/Collection/Stack" StackAdvancedFeatures     StackAdvancedFeatures.cpp)
cm_example_project("DataType/Collection/Stack" StackMemoryManagement     StackMemoryManagement.cpp)
