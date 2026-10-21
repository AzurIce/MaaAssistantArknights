add_library(maa-ctrl-playtools SHARED PlayToolsController.cpp exports.cpp)

if(APPLE AND WITH_MAC_SCK)
    target_sources(maa-ctrl-playtools PRIVATE MacSCKHelper.mm)
endif()

target_include_directories(maa-ctrl-playtools PRIVATE
    ${MAA_CTRL_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/src/MaaCore
    ${PROJECT_SOURCE_DIR}/src/MaaCore/Controller
)

target_link_libraries(maa-ctrl-playtools PRIVATE MaaCore MaaUtils ${OpenCV_LIBS} HeaderOnlyLibraries ZLIB::ZLIB Boost::system)

if(APPLE AND WITH_MAC_SCK)
    target_link_libraries(maa-ctrl-playtools PRIVATE "-framework ScreenCaptureKit" "-framework Foundation")
endif()

if(UNIX)
    target_link_libraries(maa-ctrl-playtools PRIVATE dl pthread)
endif()
