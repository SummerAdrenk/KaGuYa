# KaGuYa

Atelier Kaguya **params 系引擎的 Windows x86 洗屁屁版原生复刻**。
由于本人是BB社批，不保证南瓜社的兼容性！

所涉文件： `Start.exe`、`Graphics.dll`、`RenderDX.dll` 与 `Sound.dll`，可读取相应游戏的原始文件数据。

## 版本范围

- 目前已经兼容至最新版本的游戏，即第18号样本。  
- 游戏编号与作品名称见 [游戏清单](Start/games/catalog.json)，构建时选择对应编号即可。  
- 已知差异见 [兼容性说明](docs/COMPATIBILITY.md)。

## 构建

需要 Windows、支持 C++ latest 的 LLVM `clang-cl` / `lld-link`、CMake 3.30+、Ninja，
以及 Visual Studio 2022 的 MSVC v143（14.44）、x86 C++/ATL 与 Windows SDK 组件。  
可以在 CLion 中打开本目录，也可以在工程根目录运行：

```text
cmake --preset clang-cl-release
cmake --build .BuildCache/clang-cl-release --target Start18
cmake --build .BuildCache/clang-cl-release --target Graphics RenderDX Sound
```

把 `Start18` 换成 `Start01` … `Start17` 即可构建对应游戏。每次只选择一个 Start 编号，
因为它们都会发布为 `Release/Start.exe`。  
DLL 可以单独或组合构建：

```text
cmake --build .BuildCache/clang-cl-release --target Graphics RenderDX
cmake --build .BuildCache/clang-cl-release --target Graphics
```

默认 Release 使用小体积优化并关闭文件日志。诊断时配置一次，再统一构建：

```text
cmake --preset clang-cl-release -DKAGUYA_ENABLE_LOGGING=ON
cmake --build --preset clang-cl-release
```

将 `ON` 改回 `OFF` 可关闭日志。
Debug 使用独立的 `clang-cl-debug` 预设。完整前置条件、工具定位和输出规则见 [构建说明](docs/BUILDING.md)。

## 运行

自行准备对应游戏数据，在**独立游戏副本**中放置：

```text
<game>/Start_RS.exe
<game>/Lib/Graphics.dll
<game>/Lib/RenderDX.dll
<game>/Lib/Sound.dll
```

`Start_RS.exe` 来自所选编号构建出的 `Release/Start.exe`，三个 DLL 使用本工程的配套产物。  
目前不建议将原版 EXE 混配新 DLL 亦或者反之配套使用。  
部分日文数据路径仍使用 CP932 系统转换，需要对应的日文区域环境或用户自己的 Locale Emulator 配置。  
运行依赖相应的 x86 Visual C++ Runtime。

## 目录

| 路径 | 内容 |
|---|---|
| `Start/` | 入口、平台层、VM、资源、场景及持久化 |
| `Start/games/` | 各作品的配置和描述符 |
| `Start/resources/` | RC、资源头、manifest、位图、图标和光标 |
| `Graphics/` | Surface、图片装载、像素效果及文字 |
| `RenderDX/` | Direct3D 9 后端 |
| `Sound/` | 声音对象、解码及 DirectSound 后端 |
| `Common/` | 日志和编译器适配 |
| `3rdParty/` | 随仓库提供的第三方源码及许可 |
| `cmake/` | 构建导入、游戏目标及资源元数据脚本 |

## 免责声明与许可

本仓库包含构建所需的 Win32 位图、图标、光标及资源声明，不包含完整游戏数据、商业游戏的原始 EXE/DLL。  
作品名称、原生界面素材等第三方内容的权利仍属于各自权利人；项目源码许可证不重新授予这些内容的权利。  
本项目不是原厂发行物，也不代表原厂授权。

项目自有源码采用 **GNU Affero General Public License v3.0 only（AGPL-3.0-only）**，见 [LICENSE](LICENSE)。  
第三方组件保持各自许可证，见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。  
分发编译产物时，应一并提供适用的许可证和对应源代码；具体义务以许可证全文为准。
