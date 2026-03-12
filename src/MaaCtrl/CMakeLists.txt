# MaaCtrl — Independent controller DLLs
# Each subdirectory produces a separate shared library (DLL/so).

# The public C-ABI header is shared by all DLLs
set(MAA_CTRL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# adb-base is a static library shared by adb/minitouch/maatouch DLLs
add_subdirectory(adb-base)

add_subdirectory(adb)
add_subdirectory(minitouch)
add_subdirectory(maatouch)
add_subdirectory(playtools)

if(WIN32)
    add_subdirectory(win32)
endif()

add_subdirectory(autoplay)
