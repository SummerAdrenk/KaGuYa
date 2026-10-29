# 构建与运行

## 前置条件

- Windows，目标架构固定为 x86 / PE32；宿主开发机可以是 x64。
- LLVM 的 `clang-cl`、`lld-link`、`llvm-lib` 与 `clangd`，支持当前 C++ latest 模式。
- Visual Studio 2022 Build Tools 或 Visual Studio 2022：MSVC v143 / 14.44、x86 C++、ATL 和 Windows SDK。
- CMake 3.30+、Ninja，可从终端找到相应命令。

工具链通过 PATH 定位 LLVM，通过 vswhere 定位 VS 2022。构建依赖 MSVC 的 STL、CRT、ATL 和 SDK。

必要时在首次配置中显式传入 `KAGUYA_LLVM_ROOT`、`KAGUYA_VS_ROOT`；路径属于本机配置，
不要把个人配置写入并提交 `CMakePresets.json`。也可以使用不纳入 Git 的 `CMakeUserPresets.json`。

## 命令

在仓库根运行：

```text
cmake --preset clang-cl-release
cmake --build .BuildCache/clang-cl-release --target Start18
cmake --build .BuildCache/clang-cl-release --target Graphics RenderDX Sound
```

`Start01` … `Start18` 选择对应的游戏配置和资源。完整对应关系见
[`Start/games/catalog.json`](../Start/games/catalog.json)。所有编号共用运行时静态库；
一次只发布一个 Start 编号，避免多个目标竞争 `Release/Start.exe`。

`Graphics`、`RenderDX`、`Sound` 可任意单独或组合构建。底层 `Start` 目标保留，但选择游戏时使用编号目标。
`KAGUYA_GAME` 仅决定默认整批构建发布哪款作品，默认 `mamatomo`；显式编号不要求重新配置。

日志开关：

```text
cmake --preset clang-cl-release -DKAGUYA_ENABLE_LOGGING=ON
cmake --build --preset clang-cl-release
```

关回去把 `ON` 改成 `OFF`。Release 默认 OFF，Debug 默认 ON；Debug 使用独立的 `clang-cl-debug` 预设。
切换后统一构建 EXE 和三个 DLL，避免混用不同配置的旧产物。

## 输出

- `Release/Start.exe`：所选游戏的 EXE。
- `Release/Lib/`：三个配套 DLL。
- `Release/licenses/`：第三方许可证；发布时也应携带根目录 `LICENSE`。
- `Release/Debug/`：Debug 产物。
