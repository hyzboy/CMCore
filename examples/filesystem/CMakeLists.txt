cm_example_project("FileSystem" FixFilenameTest    FixFilenameTest.cpp)
cm_example_project("FileSystem" NormalizeFilenameTest NormalizeFilenameTest.cpp)
cm_example_project("FileSystem" EnumFileTest       EnumFileTest.cpp)

if(BUILD_TESTING)
    add_test(NAME NormalizeFilenameTest COMMAND $<TARGET_FILE:NormalizeFilenameTest>)
endif()

IF(WIN32)
cm_example_project("FileSystem" EnumVolumeTest     EnumVolumeTest.cpp)
ENDIF(WIN32)
