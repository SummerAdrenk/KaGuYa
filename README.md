# KaGuYa

Atelier Kaguya **params 系引擎的 Windows x86 洗屁屁版原生复刻**。
由于本人是BB社批，不保证南瓜社的兼容性！

所涉文件： `Start.exe`、`Graphics.dll`、`RenderDX.dll` 与 `Sound.dll`，可读取相应游戏的原始文件数据。

## 版本范围

- 目前已经兼容至最新版本的游戏，即第18号样本。  
- 游戏编号与作品名称见 [游戏清单](Start/games/catalog.json)，构建时选择对应编号即可。  
- 已知差异见 [兼容性说明](docs/COMPATIBILITY.md)。

## 构建

环境： Windows、支持 C++ latest 的 LLVM `clang-cl` / `lld-link`、CMake 3.30+、Ninja，
以及 Visual Studio 2022 的 MSVC v143（14.44）、x86 C++/ATL 与 Windows SDK 组件。  
➤ Build：

```text
cmake --preset clang-cl-release
cmake --build .BuildCache/clang-cl-release --target Start18
cmake --build .BuildCache/clang-cl-release --target Graphics RenderDX Sound
```

把 `Start18` 换成 `Start01` … `Start17` 即可构建对应游戏。

➤批量构建全部或只选择其中几个：

```text
cmake --build .BuildCache/clang-cl-release --target StartAll
cmake --build .BuildCache/clang-cl-release --target Start01 Start18
```

➤DLL 可以单独或组合构建：

```text
cmake --build .BuildCache/clang-cl-release --target Graphics RenderDX
cmake --build .BuildCache/clang-cl-release --target Graphics
```

➤默认 Release 使用小体积优化并关闭文件日志。诊断时配置一次，再统一构建：

```text
cmake --preset clang-cl-release -DKAGUYA_ENABLE_LOGGING=ON
cmake --build --preset clang-cl-release
```

将 `ON` 改回 `OFF` 可关闭日志。
Debug 使用独立的 `clang-cl-debug` 预设。  
其他见 [构建说明](docs/BUILDING.md)。

>目前不建议将原版 EXE 混配新 DLL 亦或者反之配套使用。  
部分日文数据路径仍使用 CP932 系统转换，需要对应的日文区域环境或用户自己的 Locale Emulator 配置。  
运行依赖相应的 x86 Visual C++ Runtime。

## 目录

| 路径                           | 内容                                              |
|--------------------------------|---------------------------------------------------|
| `Start/`                       | 入口、平台层、VM、资源、场景及持久化              |
| `Start/games/<game-slug>/`     | 按作品简称组织的配置和描述符                      |
| `Start/resources/<game-slug>/` | 同名作品的 RC、资源头、manifest、位图、图标和光标 |
| `Graphics/`                    | Surface、图片装载、像素效果及文字                 |
| `RenderDX/`                    | Direct3D 9 后端                                   |
| `Sound/`                       | 声音对象、解码及 DirectSound 后端                 |
| `Common/`                      | 日志和编译器适配                                  |
| `3rdParty/`                    | 随仓库提供的第三方源码及许可                      |
| `cmake/`                       | 构建导入、游戏目标及资源元数据脚本                |

## 免责声明与许可

项目自有源码采用 **GNU Affero General Public License v3.0 only（AGPL-3.0-only）**，见 [LICENSE](LICENSE)。  
第三方组件保持各自许可证，见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。  
