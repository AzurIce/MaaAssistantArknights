# maa-ctrl-minitouch — Minitouch controller DLL

file(GLOB_RECURSE minitouch_src *.h *.hpp *.cpp)

add_library(maa-ctrl-minitouch SHARED ${minitouch_src})

target_include_directories(maa-ctrl-minitouch
    PRIVATE ${MAA_CTRL_INCLUDE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/src/MaaCore
)

target_link_libraries(maa-ctrl-minitouch maa-ctrl-adb-base HeaderOnlyLibraries MaaUtils)
