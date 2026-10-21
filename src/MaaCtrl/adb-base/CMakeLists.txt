# adb-base — static library shared by the adb / minitouch / maatouch plugins.
add_library(adb-base STATIC AdbController.cpp MinitouchController.cpp)
set_target_properties(adb-base PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(adb-base PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/MaaCore
    ${PROJECT_SOURCE_DIR}/src/MaaCore/Controller
)

target_link_libraries(adb-base PUBLIC MaaUtils ${OpenCV_LIBS} HeaderOnlyLibraries)
