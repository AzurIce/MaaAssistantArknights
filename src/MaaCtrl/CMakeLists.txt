# MaaCtrl — independent controller plugins (maa-ctrl-*).
#
# 迁移进度：
#   - autoplay：已实现（加载 ap_ffi，走 auto-play 后端）
#   - adb / minitouch / maatouch / playtools / maafw / mumu / win32：后续按控制器逐个迁入
#
# 公共 C-ABI 头文件由所有插件共享。
set(MAA_CTRL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(autoplay)
