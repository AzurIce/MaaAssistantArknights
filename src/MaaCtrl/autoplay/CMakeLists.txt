# maa-ctrl-autoplay — AutoPlay framework controller DLL

# Only compile AutoPlayLoader and exports, not the full controller
file(GLOB autoplay_src AutoPlayLoader.h AutoPlayLoader.cpp exports.cpp)

add_library(maa-ctrl-autoplay SHARED ${autoplay_src})

target_include_directories(maa-ctrl-autoplay
    PRIVATE ${MAA_CTRL_INCLUDE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/src/MaaCore
)

target_link_libraries(maa-ctrl-autoplay HeaderOnlyLibraries MaaUtils ${OpenCV_LIBS} MaaCore)

if(WIN32)
    # For LoadLibrary/GetProcAddress
elseif(UNIX)
    target_link_libraries(maa-ctrl-autoplay dl)
endif()
