# adb-base — Static library shared by adb/minitouch/maatouch DLLs.
# Contains AdbController core code, Platform IO, SwipeHelper, emulator extras.

file(GLOB_RECURSE adb_base_src *.h *.hpp *.cpp)

add_library(maa-ctrl-adb-base STATIC ${adb_base_src})

target_include_directories(maa-ctrl-adb-base
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${MAA_CTRL_INCLUDE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/src/MaaCore    # for Common/, Config/, Utils/ etc.
)

if(WITH_EMULATOR_EXTRAS)
    if(EXISTS ${PROJECT_SOURCE_DIR}/3rdparty/EmulatorExtras/Mumu)
        target_include_directories(maa-ctrl-adb-base PUBLIC ${PROJECT_SOURCE_DIR}/3rdparty/EmulatorExtras)
    endif()
endif()

target_compile_definitions(maa-ctrl-adb-base PRIVATE ASST_WITH_EMULATOR_EXTRAS=$<BOOL:${WITH_EMULATOR_EXTRAS}>)

target_link_libraries(maa-ctrl-adb-base HeaderOnlyLibraries MaaUtils Boost::system)

if(WIN32)
    target_link_libraries(maa-ctrl-adb-base ws2_32)
endif()

if(LINUX)
    target_link_libraries(maa-ctrl-adb-base pthread dl)
endif()
