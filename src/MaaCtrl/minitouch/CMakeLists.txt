# maa-ctrl-minitouch — Minitouch controller DLL

# Only compile the C-ABI exports, not the full implementation
add_library(maa-ctrl-minitouch SHARED exports.cpp)

target_include_directories(maa-ctrl-minitouch
    PRIVATE ${MAA_CTRL_INCLUDE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/src/MaaCore
)

target_link_libraries(maa-ctrl-minitouch maa-ctrl-adb-base HeaderOnlyLibraries MaaUtils MaaCore)
