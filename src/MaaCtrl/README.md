# MaaCtrl — controller 插件

MaaCore 的 controller 层以 `maa-ctrl-*` 共享库形式加载：`MaaCtrlLoader` 负责 dlopen，
`DllController`（实现 `ControllerAPI`）把调用转发给插件。

## 插件一览

| 插件 | 说明 | 条件 |
|---|---|---|
| `maa-ctrl-adb` | ADB input | 全平台 |
| `maa-ctrl-minitouch` | minitouch | 全平台 |
| `maa-ctrl-maatouch` | MaaTouch | 全平台 |
| `maa-ctrl-playtools` | PlayCover / MaaTools | MacSCK 需 `APPLE && WITH_MAC_SCK` |
| `maa-ctrl-maafw-adb` | MaaFramework ADB control unit | 全平台 |
| `maa-ctrl-maafw-android` | MaaFramework Android native | 仅 `ANDROID` |
| `maa-ctrl-mumu` | MuMu external renderer（降级回 minitouch） | `WITH_EMULATOR_EXTRAS` |
| `maa-ctrl-autoplay` | auto-play / `ap_ffi` 后端 | 需要 `libap_ffi.so` |

`adb-base` 是把 `AdbController` + `MinitouchController` 编成的静态库，被 adb / minitouch /
maatouch / mumu 四个插件共用。

## ABI 与实现方式

- `include/maa_ctrl_api.h`：插件 C ABI（create/destroy/screencap/get_image/click/swipe/
  touch_*/key_*/start_game/back_to_home/…）。`MaaCtrlCreateInfo` 携带 adb 路径、地址、
  config、`platform_type`、回调（`MaaCtrlCallback`）与 user_data。
- `include/maa_ctrl_controller_plugin.hpp`：给基于 `ControllerAPI` 的控制器用的共享实现。
  插件只需要：
  1. `#define MAA_CTRL_PLUGIN_VERSION ...`
  2. include 该头文件
  3. 实现 `asst::ControllerAPI* maa_ctrl_plugin_create_controller(const MaaCtrlCreateInfo*)`
- `MaaCtrlLoader` 同时兼容 `<name>.so` 与 `lib<name>.so` 命名。

## 加载注意

- 插件按名字 dlopen（`maa-ctrl-xxx` → `libmaa-ctrl-xxx.so`），`build/bin` 已在 MaaCore
  的 RUNPATH 中，通常无需额外设置。
- **RTLD_GLOBAL**：宿主（maa-cli）以 `RTLD_LOCAL` dlopen MaaCore 时，插件 DSO 里那些
  头文件 inline 单例（`SingletonHolder<Config/Logger/...>`）会各自实例化。`MaaCtrlLoader`
  在加载插件前用 `dladdr` + `dlopen(RTLD_NOLOAD | RTLD_GLOBAL)` 把 MaaCore 提升到全局
  作用域，保证插件读到的是 MaaCore 已加载的实例。
- 调试时可用 `MAA_CTRL_DLL=/abs/path/libmaa-ctrl-xxx.so` 直接指定插件，绕开 `touch_mode`。

## 构建与运行

**flake（推荐，可复现）**：

```bash
nix build .#        # → result/lib：libMaaCore.so + 全部 maa-ctrl-* 插件 + libc++/libunwind/MaaDeps 运行库
nix develop         # clang 21 + libc++ + MaaDeps（$MAA_DEPS、$LD_LIBRARY_PATH 已配好）
```

- 输入：nixpkgs（lock 固定）、`MaaXYZ/MaaUtils` 固定 commit、MaaDeps 2.12.6（devel + runtime，按 sha256 固定）。
- 产物用 patchelf 写入 `$ORIGIN` rpath 并把 libc++/libunwind/opencv/onnxruntime/fastdeploy 一并装进
  `result/lib`，`ldd` 无缺失。
- 给 maa-cli 用时把 `MAA_DATA_DIR` 指到一个含 `lib/` 与 `resource/` 的目录（`lib/` 里放 `result/lib/*.so` 的软链）。

**手动脚本**：`tools/build_plugins.sh`（同样用 clang + libc++ + MaaDeps），脚本会打印运行所需的
`LD_LIBRARY_PATH`。

## 范围说明

- 除 Windows 的 `attach_window`（把游戏窗口嵌进 WPF GUI）外，所有 controller 都由插件提供。
  该路径需要把 HWND 传给 `Win32Controller`，与触控后端无关，暂保留为内建实现（仅 `_WIN32`）。
- `ControllerRegistry` 用于宿主枚举可用插件（按 `maa-ctrl-*.so` 是否存在过滤），当前未接 C API；
  MaaCore 侧按 `touch_mode` 直接映射到插件名。
- 平台条件插件：`maa-ctrl-maafw-android`（`ANDROID`）、`maa-ctrl-mumu`（`WITH_EMULATOR_EXTRAS`）、
  `maa-ctrl-playtools` 的 MacSCK（`APPLE && WITH_MAC_SCK`）。
