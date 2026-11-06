# CMCore Hash Examples
#
# 哈希算法本体在 CMHash，文件/流的封装在 CMCore —— 示例放在这里，
# 因为它们要演示的正是「算法 + IO」这一层。

macro(cm_example_project sub_folder project_name)
    cm_example_project_base(
        PROJECT_NAME ${project_name}
        FOLDER_PATH "Examples/CMCore/${sub_folder}"
        SOURCES ${ARGN}
        PRIVATE_LIBRARIES CMCore
    )
endmacro()

cm_example_project("Hash" HashFileTest   HashFileTest.cpp)
cm_example_project("Hash" HashStreamTest HashStreamTest.cpp)