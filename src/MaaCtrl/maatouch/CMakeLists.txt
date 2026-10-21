add_library(maa-ctrl-maatouch SHARED exports.cpp)

target_include_directories(maa-ctrl-maatouch PRIVATE
    ${MAA_CTRL_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/src/MaaCore
    ${PROJECT_SOURCE_DIR}/src/MaaCore/Controller
)

target_link_libraries(maa-ctrl-maatouch PRIVATE adb-base MaaCore MaaUtils ${OpenCV_LIBS} HeaderOnlyLibraries ZLIB::ZLIB Boost::system)

if(UNIX)
    target_link_libraries(maa-ctrl-maatouch PRIVATE dl pthread)
endif()
