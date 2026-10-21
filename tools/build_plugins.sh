#!/usr/bin/env bash
# 在 NixOS 上构建 MaaCore + controller 插件（clang + libc++ + MaaDeps）。
#
# 前置：
#   - 已初始化 MaaUtils 子模块：git submodule update --init src/MaaUtils
#   - 已准备 MaaDeps（x64-linux devel + runtime）到 src/MaaUtils/MaaDeps/
#     下载：https://github.com/MaaXYZ/MaaDeps/releases
#   - 可选：WITH_EMULATOR_EXTRAS=ON 时需 3rdparty/EmulatorExtras 子模块
#
# 用法：
#   tools/build_plugins.sh                 # Release 构建全部目标
#   JOBS=12 tools/build_plugins.sh         # 限制并行度
#   tools/build_plugins.sh -DWITH_EMULATOR_EXTRAS=ON   # 追加 cmake 参数
#
# 说明：
#   - 上游默认用 -flto=thin + libc++，NixOS 的 clang 需要显式给 libc++ 头文件与运行库；
#   - WITH_RPATH_LIBRARY=OFF 跳过把 libc++/libc++abi/libunwind 拷贝进安装目录
#     （运行时用下面打印的 LD_LIBRARY_PATH 提供）。
set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$ROOT"

if [ ! -d "$ROOT/src/MaaUtils/MaaDeps/vcpkg/installed/maa-x64-linux" ]; then
    echo "error: MaaDeps not found (src/MaaUtils/MaaDeps). See header of this script." >&2
    exit 1
fi

LIBCXX=$(nix eval --raw nixpkgs#libcxx.outPath)
LIBCXX_DEV=$(nix eval --raw nixpkgs#libcxx.dev.outPath)
UNWIND=$(nix eval --raw nixpkgs#llvmPackages.libunwind.outPath)
MAA_DEPS="$ROOT/src/MaaUtils/MaaDeps"
SYSROOT_LIB="$MAA_DEPS/x-tools/x86_64-linux-gnu/x86_64-linux-gnu/sysroot/usr/lib"

# clang 默认走 libstdc++ 头文件，这里用 -nostdinc++ 强制 libc++（与 MaaDeps 的 ABI 一致）。
TOOLCHAIN="$ROOT/build/toolchain"
mkdir -p "$TOOLCHAIN"
cat > "$TOOLCHAIN/clangxx" <<EOF
#!/bin/sh
exec clang++ -nostdinc++ -isystem $LIBCXX_DEV/include/c++/v1 "\$@"
EOF
chmod +x "$TOOLCHAIN/clangxx"
cat > "$TOOLCHAIN/clang" <<'EOF'
#!/bin/sh
exec clang "$@"
EOF
chmod +x "$TOOLCHAIN/clang"

LINK_FLAGS="-fuse-ld=lld -L$LIBCXX/lib -lc++ -lc++abi"
JOBS=${JOBS:-8}

nix shell nixpkgs#cmake nixpkgs#ninja nixpkgs#clang nixpkgs#libcxx nixpkgs#libcxx.dev nixpkgs#lld -c bash -c "
set -euo pipefail
# 编译器路径变化时 CMake 会丢弃缓存并自动重跑，首次 -D 参数可能不生效，
# 因此跑两遍（第二遍是幂等的快速配置）。
for _ in 1 2; do
    cmake -B build -GNinja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER=$TOOLCHAIN/clang \
        -DCMAKE_CXX_COMPILER=$TOOLCHAIN/clangxx \
        -DWITH_RPATH_LIBRARY=OFF \
        -DCMAKE_EXE_LINKER_FLAGS='$LINK_FLAGS' \
        -DCMAKE_SHARED_LINKER_FLAGS='$LINK_FLAGS' \
        -DCMAKE_MODULE_LINKER_FLAGS='$LINK_FLAGS' \
        $*
done
cmake --build build -j$JOBS
"

cat <<EOF

build done: $ROOT/build/bin

运行本地构建的 MaaCore + 插件时：
  export LD_LIBRARY_PATH=$LIBCXX/lib:$UNWIND/lib:$ROOT/build/bin:$MAA_DEPS/vcpkg/installed/maa-x64-linux/lib:$SYSROOT_LIB:\$LD_LIBRARY_PATH

可选：
  - 调试时用 MAA_CTRL_DLL=/abs/path/libmaa-ctrl-xxx.so 直接指定插件；
  - 插件默认按名字 dlopen（MaaCore 的 RUNPATH 已包含 build/bin）。
EOF
