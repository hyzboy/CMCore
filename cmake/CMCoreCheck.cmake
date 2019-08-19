set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(system_bit)
include(version)
include(compiler)
include(output_path)

check_system_bits()
check_system_version()
set_compiler_param()
set_output_directory()
