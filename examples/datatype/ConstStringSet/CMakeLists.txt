# ConstStringSet Tests
# 提供三个不同规模的测试程序

# 基础功能测试
cm_example_project("DataType/ConstStringSet" ConstStringSetDebug ConstStringSetDebug.cpp)

# 综合功能测试（10个测试项）
cm_example_project("DataType/ConstStringSet" ConstStringSetTest ConstStringSetTest.cpp)

# 大规模性能测试（10,000+ strings）
cm_example_project("DataType/ConstStringSet" ConstStringSetLargeScaleTest ConstStringSetLargeScaleTest.cpp)