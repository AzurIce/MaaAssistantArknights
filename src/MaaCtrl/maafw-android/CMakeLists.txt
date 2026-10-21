if(NOT ANDROID)
    return()
endif()

add_library(maa-ctrl-maafw-android SHARED MaaFwAndroidNativeController.cpp exports.cpp)

target_include_directories(maa-ctrl-maafw-android PRIVATE
    ${MAA_CTRL_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/src/MaaCore
    ${PROJECT_SOURCE_DIR}/src/MaaCore/Controller
)

target_link_libraries(maa-ctrl-maafw-android PRIVATE MaaCore MaaUtils ${OpenCV_LIBS} HeaderOnlyLibraries ZLIB::ZLIB Boost::system)
