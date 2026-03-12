# maa-ctrl-autoplay — AutoPlay framework controller DLL

file(GLOB_RECURSE autoplay_src *.h *.hpp *.cpp)

add_library(maa-ctrl-autoplay SHARED ${autoplay_src})

target_include_directories(maa-ctrl-autoplay
    PRIVATE ${MAA_CTRL_INCLUDE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/src/MaaCore
)

target_link_libraries(maa-ctrl-autoplay HeaderOnlyLibraries MaaUtils)

if(WIN32)
    # For LoadLibrary/GetProcAddress
elseif(UNIX)
    target_link_libraries(maa-ctrl-autoplay dl)
endif()
