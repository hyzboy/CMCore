# ConstStringSet Tests
# 提供三个不同规模的测试程序

# 基础功能测试
cm_datatype_test("DataType/ConstStringSet" ConstStringSetDebug ConstStringSetDebug.cpp)

# 综合功能测试（10个测试项）
cm_datatype_test("DataType/ConstStringSet" ConstStringSetTest ConstStringSetTest.cpp)

# 大规模性能测试（10,000+ strings）
cm_datatype_test("DataType/ConstStringSet" ConstStringSetLargeScaleTest ConstStringSetLargeScaleTest.cpp)