add_library(maa-ctrl-adb SHARED exports.cpp)

target_include_directories(maa-ctrl-adb PRIVATE ${MAA_CTRL_INCLUDE_DIR})

target_link_libraries(maa-ctrl-adb PRIVATE adb-base MaaCore MaaUtils ${OpenCV_LIBS} HeaderOnlyLibraries ZLIB::ZLIB Boost::system)

if(UNIX)
    target_link_libraries(maa-ctrl-adb PRIVATE dl pthread)
endif()
