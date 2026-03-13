# maa-ctrl-maatouch — Maatouch controller DLL (thin wrapper over minitouch with m_use_maa_touch=true)

add_library(maa-ctrl-maatouch SHARED exports.cpp)

target_include_directories(maa-ctrl-maatouch
    PRIVATE ${MAA_CTRL_INCLUDE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/src/MaaCore
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../minitouch  # for MinitouchImpl headers
)

target_link_libraries(maa-ctrl-maatouch maa-ctrl-adb-base HeaderOnlyLibraries MaaUtils MaaCore)
