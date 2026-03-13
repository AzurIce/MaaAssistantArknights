# maa-ctrl-adb — ADB-only controller DLL (pure adb shell input)

add_library(maa-ctrl-adb SHARED exports.cpp)

target_include_directories(maa-ctrl-adb
    PRIVATE ${MAA_CTRL_INCLUDE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/src/MaaCore
)

target_link_libraries(maa-ctrl-adb maa-ctrl-adb-base HeaderOnlyLibraries MaaUtils MaaCore)
