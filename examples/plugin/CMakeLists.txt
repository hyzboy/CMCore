# P1 插件管理回归测试:微型插件 DLL + 验证程序
# 验证程序须在 bad_plugin_dir 目录下运行(cd 后执行),该目录含"坏插件文件"(非 PE 文本),
# 使 findpath[0] 命中坏文件、程序目录命中真 DLL,回归 LoadPlugin 多路径查找 Bug。

# 微型插件 DLL(与 CMAudio 插件同构:导出 InitPlugIn)
add_library(CMP.Test.Good SHARED TestPlugIn.cpp)
set_target_properties(CMP.Test.Good PROPERTIES FOLDER "Examples/CMCore/plugin")

# 验证程序
cm_example_project(plugin plugin_p1_verify main.cpp)
add_dependencies(plugin_p1_verify CMP.Test.Good)

# 生成"坏插件文件"(文本内容,LoadLibraryW 必然失败),与真 DLL 同名
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bad_plugin_dir)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/bad_plugin_dir/CMP.Test.Good.DLL "not a valid PE binary\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/bad_plugin_dir/CMP.Test.Bad.DLL "not a valid PE binary\n")
