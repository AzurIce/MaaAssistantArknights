{
  description = "MaaAssistantArknights dev-v2 + controller 插件（maa-ctrl-*）";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    # 对应仓库里 src/MaaUtils 子模块的 commit；flake 源码不含子模块内容，构建时替换。
    maa-utils = {
      url = "github:MaaXYZ/MaaUtils/6e9ba33f6ad835418097d9324c01c44a82825a2b";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, flake-utils, maa-utils }:
    flake-utils.lib.eachSystem [ "x86_64-linux" ] (system:
      let
        pkgs = import nixpkgs { inherit system; };
        inherit (pkgs) lib;

        libcxx = pkgs.libcxx;
        libcxxDev = pkgs.libcxx.dev;
        libunwind = pkgs.llvmPackages.libunwind;

        # 上游 dev-v2 配套的预编译依赖（OpenCV / onnxruntime / fastdeploy / Boost / ...）
        maadepsVersion = "2.12.6";
        maadepsDevel = pkgs.fetchurl {
          url = "https://github.com/MaaXYZ/MaaDeps/releases/download/v${maadepsVersion}/MaaDeps-x64-linux-devel.tar.xz";
          sha256 = "5fa43dec17ab4cf972d673fc6185a256b70ca2f2260a2c79c1c970d3307bbdbb";
        };
        maadepsRuntime = pkgs.fetchurl {
          url = "https://github.com/MaaXYZ/MaaDeps/releases/download/v${maadepsVersion}/MaaDeps-x64-linux-runtime.tar.xz";
          sha256 = "080236a8d19f5b86c95eca2a8ac95ae4c1fbf1fc50f2f5e0b16d0dde3315c722";
        };

        maadeps = pkgs.stdenvNoCC.mkDerivation {
          pname = "maa-deps";
          version = maadepsVersion;
          dontUnpack = true;
          nativeBuildInputs = [ pkgs.xz ];
          installPhase = ''
            runHook preInstall
            mkdir -p $out
            tar -xJf ${maadepsDevel} -C $out
            tar -xJf ${maadepsRuntime} -C $out
            runHook postInstall
          '';
        };

        # 编译用 wrapper：clang 默认走 libstdc++，这里强制 libc++ 头文件（与 MaaDeps 的 ABI 一致）
        clangxx = pkgs.writeShellScriptBin "maa-clangxx" ''
          exec ${pkgs.clang}/bin/clang++ -nostdinc++ -isystem ${libcxxDev}/include/c++/v1 "$@"
        '';

        maadepsLib = "${maadeps}/vcpkg/installed/maa-x64-linux/lib";
        maadepsSysrootLib = "${maadeps}/x-tools/x86_64-linux-gnu/x86_64-linux-gnu/sysroot/usr/lib";

        # 运行时需要的搜索路径（libc++/libunwind/MaaDeps/系统 C++ 运行库）
        runtimeLibPath = lib.makeLibraryPath [ libcxx libunwind pkgs.gcc.cc.lib ];
        packageRpath = lib.concatStringsSep ":" [
          "$ORIGIN"
          "$ORIGIN/maa-deps"
          runtimeLibPath
          maadepsLib
          maadepsSysrootLib
        ];

        maa = pkgs.stdenv.mkDerivation {
          pname = "maa-core-plugins";
          version = "dev-v2";
          src = self;

          nativeBuildInputs = [
            pkgs.cmake
            pkgs.ninja
            pkgs.pkg-config
            pkgs.clang
            pkgs.lld
            pkgs.patchelf
          ];
          buildInputs = [ libcxx libcxxDev libunwind ];

          # Nix 的 stdenv 默认 -D_FORTIFY_SOURCE=2，会让 glibc 的 chdir 等带
          # warn_unused_result，与上游 -Werror 冲突；这里关掉。
          hardeningDisable = [ "fortify" ];

          postUnpack = ''
            # flake 源码不含子模块内容：用固定 commit 的 MaaUtils 输入替换
            rm -rf "$sourceRoot/src/MaaUtils"
            cp -r --no-preserve=mode ${maa-utils} "$sourceRoot/src/MaaUtils"
            # MaaDeps 必须位于 src/MaaUtils/MaaDeps（MaaUtils.cmake 里是硬编码路径）
            ln -s ${maadeps} "$sourceRoot/src/MaaUtils/MaaDeps"
          '';

          cmakeFlags = [
            "-DCMAKE_BUILD_TYPE=Release"
            "-DCMAKE_C_COMPILER=${pkgs.clang}/bin/clang"
            "-DCMAKE_CXX_COMPILER=${clangxx}/bin/maa-clangxx"
            # 自己用 patchelf 设 rpath；上游的 copy_and_add_rpath_library 依赖
            # `clang -print-file-name`，在 Nix 的 clang wrapper 下不可靠。
            "-DWITH_RPATH_LIBRARY=OFF"
            "-DCMAKE_SKIP_RPATH=ON"
          ];

          # 链接期参数：-stdlib=libc++ 让 clang 驱动链接 libc++/libc++abi。
          # 放在 LDFLAGS 里避免 cmakeFlags 以空格分割。
          env = {
            LDFLAGS = "-fuse-ld=lld -stdlib=libc++ -L${libcxx}/lib";
          };

          installPhase = ''
            runHook preInstall
            mkdir -p $out/lib
            # cmake hook 可能是 in-source 构建（bin/）或 build/bin/，两种都覆盖
            find . -maxdepth 4 -type f -name '*.so' -path '*/bin/*' -exec cp -a {} $out/lib/ \;
            # 运行库一并放进 $out/lib：所有 .so 的 rpath 只需 $ORIGIN
            cp -aL ${libcxx}/lib/libc++.so.1 ${libcxx}/lib/libc++abi.so.1 $out/lib/
            cp -aL ${libunwind}/lib/libunwind.so.1 $out/lib/
            cp -aL ${maadepsLib}/libopencv_world4.so.412 \
                   ${maadepsLib}/libonnxruntime.so.1 \
                   ${maadepsLib}/libfastdeploy_ppocr.so \
                   ${maadepsLib}/libonnxruntime_providers_shared.so $out/lib/
            runHook postInstall
          '';

          postFixup = ''
            for so in $out/lib/*.so*; do
              # 单引号：$ORIGIN 必须原样传给 patchelf，不能被 shell 展开
              patchelf --force-rpath --set-rpath '$ORIGIN:${pkgs.gcc.cc.lib}/lib' "$so"
            done
          '';

          meta = with lib; {
            description = "MaaCore + maa-ctrl-* controller plugins (dev-v2)";
            platforms = [ "x86_64-linux" ];
          };
        };
      in {
        devShells.default = pkgs.mkShell {
          packages = [
            pkgs.cmake
            pkgs.ninja
            pkgs.pkg-config
            pkgs.clang
            pkgs.lld
            libcxx
            libcxxDev
            libunwind
            pkgs.patchelf
            pkgs.git
          ];

          shellHook = ''
            export MAA_DEPS=${maadeps}
            export MAA_DEPS_LIB=${maadepsLib}
            export LD_LIBRARY_PATH=${lib.makeLibraryPath [ libcxx libunwind pkgs.gcc.cc.lib ]}:${maadepsLib}:${maadepsSysrootLib}''${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
            echo "MaaCtrl devShell: clang $(clang --version | head -1 | awk '{print $3}'), MaaDeps ${maadepsVersion}"
            echo "  本地构建：把 \$MAA_DEPS 解到 src/MaaUtils/MaaDeps，或直接用 nix build .#"
          '';
        };

        packages = {
          inherit maadeps;
          default = maa;
        };
      });
}
