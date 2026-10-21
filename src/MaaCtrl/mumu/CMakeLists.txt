if(NOT WITH_EMULATOR_EXTRAS)
    return()
endif()

add_library(maa-ctrl-mumu SHARED MumuController.cpp exports.cpp)

target_include_directories(maa-ctrl-mumu PRIVATE
    ${MAA_CTRL_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/src/MaaCore
    ${PROJECT_SOURCE_DIR}/src/MaaCore/Controller
)

target_link_libraries(maa-ctrl-mumu PRIVATE adb-base MaaCore MaaUtils ${OpenCV_LIBS} HeaderOnlyLibraries ZLIB::ZLIB Boost::system)

if(UNIX)
    target_link_libraries(maa-ctrl-mumu PRIVATE dl pthread)
endif()
