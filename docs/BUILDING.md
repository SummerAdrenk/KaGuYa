# 构建与运行

## 前置条件

- LLVM 的 `clang-cl`、`lld-link`、`llvm-lib` 与 `clangd`，支持当前 C++ latest 模式。
- Visual Studio 2022 Build Tools 或 Visual Studio 2022：MSVC v143 / 14.44、x86 C++、ATL 和 Windows SDK。
- CMake 3.30+、Ninja，可从终端找到相应命令。

工具链通过 PATH 定位 LLVM，通过 vswhere 定位 VS 2022。构建依赖 MSVC 的 STL、CRT、ATL 和 SDK。  
必要时在首次配置中显式传入 `KAGUYA_LLVM_ROOT`、`KAGUYA_VS_ROOT`。

## 游戏目录简称

同一作品在 `Start/games/`、`Start/resources/`、构建缓存及 `Release/` 下使用一致的简短 `slug`。

| 目标      | 目录名               |
|-----------|----------------------|
| `Start01` | `baku-ane`           |
| `Start02` | `namaiki`            |
| `Start03` | `chu-x-pet`          |
| `Start04` | `harami-tama`        |
| `Start05` | `ero-zemi`           |
| `Start06` | `baku-ane-2`         |
| `Start07` | `shigokare`          |
| `Start08` | `love-x-holic`       |
| `Start09` | `nee-chan-no-susume` |
| `Start10` | `mamagoto`           |
| `Start11` | `mama-x-holic`       |
| `Start12` | `nee-nee-nee`        |
| `Start13` | `anetomo`            |
| `Start14` | `bunnys-mama`        |
| `Start15` | `pure-x-holic`       |
| `Start16` | `oppai-dekai-nurse`  |
| `Start17` | `hakoniwa`           |
| `Start18` | `mamatomo`           |
