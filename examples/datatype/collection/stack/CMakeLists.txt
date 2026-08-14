# Stack 相关测试项目
# 原有核心测试
cm_datatype_test("DataType/Collection/Stack" StackTest              StackTest.cpp)
cm_datatype_test("DataType/Collection/Stack" StackPoolTest          StackPoolTest.cpp)

# 重组的综合测试 (包含18个拆散测试的所有用例)
cm_datatype_test("DataType/Collection/Stack" StackBasicOperations      StackBasicOperations.cpp)
cm_datatype_test("DataType/Collection/Stack" StackAdvancedFeatures     StackAdvancedFeatures.cpp)
cm_datatype_test("DataType/Collection/Stack" StackMemoryManagement     StackMemoryManagement.cpp)
