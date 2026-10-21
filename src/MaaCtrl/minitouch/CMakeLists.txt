add_library(maa-ctrl-minitouch SHARED exports.cpp)

target_include_directories(maa-ctrl-minitouch PRIVATE
    ${MAA_CTRL_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/src/MaaCore
    ${PROJECT_SOURCE_DIR}/src/MaaCore/Controller
)

target_link_libraries(maa-ctrl-minitouch PRIVATE adb-base MaaCore MaaUtils ${OpenCV_LIBS} HeaderOnlyLibraries ZLIB::ZLIB Boost::system)

if(UNIX)
    target_link_libraries(maa-ctrl-minitouch PRIVATE dl pthread)
endif()
