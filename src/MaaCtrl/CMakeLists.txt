# MaaCtrl — independent controller plugins (maa-ctrl-*).
#
# 迁移进度：
#   - autoplay / adb / minitouch / maatouch：已插件化
#   - playtools / maafw / mumu / win32：后续迁入
set(MAA_CTRL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(adb-base)
add_subdirectory(adb)
add_subdirectory(minitouch)
add_subdirectory(maatouch)
add_subdirectory(playtools)
add_subdirectory(autoplay)
