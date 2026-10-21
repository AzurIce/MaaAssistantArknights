# MaaCtrl — independent controller plugins (maa-ctrl-*).
set(MAA_CTRL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(adb-base)
add_subdirectory(adb)
add_subdirectory(minitouch)
add_subdirectory(maatouch)
add_subdirectory(playtools)
add_subdirectory(maafw-adb)
add_subdirectory(maafw-android)
add_subdirectory(mumu)
add_subdirectory(autoplay)
