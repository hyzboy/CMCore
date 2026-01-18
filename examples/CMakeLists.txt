# CMCore Examples
# Lightweight wrapper for cm_example_project with module-specific settings

macro(cm_example_project sub_folder project_name)
    cm_example_project_base(
        PROJECT_NAME ${project_name}
        FOLDER_PATH "Examples/CMCore/${sub_folder}"
        SOURCES ${ARGN}
        PRIVATE_LIBRARIES CMCore
    )
endmacro()

# Add subdirectories
add_subdirectory(datatype)
add_subdirectory(filesystem)
add_subdirectory(misc)
add_subdirectory(system)
add_subdirectory(time)
