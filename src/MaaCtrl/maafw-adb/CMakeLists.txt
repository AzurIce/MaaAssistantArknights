add_library(maa-ctrl-maafw-adb SHARED MaaFwAdbController.cpp exports.cpp)

target_include_directories(maa-ctrl-maafw-adb PRIVATE
    ${MAA_CTRL_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/src/MaaCore
    ${PROJECT_SOURCE_DIR}/src/MaaCore/Controller
)

target_link_libraries(maa-ctrl-maafw-adb PRIVATE MaaCore MaaUtils ${OpenCV_LIBS} HeaderOnlyLibraries ZLIB::ZLIB Boost::system)

if(UNIX)
    target_link_libraries(maa-ctrl-maafw-adb PRIVATE dl pthread)
endif()
