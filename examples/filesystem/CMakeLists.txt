cm_example_project("FileSystem" FixFilenameTest    FixFilenameTest.cpp)
cm_example_project("FileSystem" EnumFileTest       EnumFileTest.cpp)

IF(WIN32)
cm_example_project("FileSystem" EnumVolumeTest     EnumVolumeTest.cpp)
ENDIF(WIN32)
