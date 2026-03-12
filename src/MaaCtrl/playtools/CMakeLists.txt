# maa-ctrl-playtools — PlayTools (macOS/iOS) controller DLL

file(GLOB_RECURSE playtools_src *.h *.hpp *.cpp)

if(WITH_MAC_SCK AND APPLE)
    list(APPEND playtools_src MacSCKHelper.mm)
endif()

add_library(maa-ctrl-playtools SHARED ${playtools_src})

target_include_directories(maa-ctrl-playtools
    PRIVATE ${MAA_CTRL_INCLUDE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/src/MaaCore
)

target_compile_definitions(maa-ctrl-playtools PRIVATE ASST_WITH_MAC_SCK=$<BOOL:${WITH_MAC_SCK}>)

target_link_libraries(maa-ctrl-playtools HeaderOnlyLibraries MaaUtils Boost::system)

if(APPLE AND WITH_MAC_SCK)
    target_link_libraries(maa-ctrl-playtools "-framework Accelerate"
                                              "-framework Foundation"
                                              "-framework CoreGraphics"
                                              "-framework CoreMedia"
                                              "-framework CoreVideo"
                                              "-framework ScreenCaptureKit")
endif()
