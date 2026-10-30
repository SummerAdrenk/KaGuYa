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
EXE 按清单中的 `slug` 分别发布，不会互相覆盖。批量构建全部 18 个 EXE：

```text
cmake --build .BuildCache/clang-cl-release --target StartAll
```

`StartAll` 不包含 DLL；也可以一次选择几个编号，例如：

```text
cmake --build .BuildCache/clang-cl-release --target Start01 Start18
```

`Graphics`、`RenderDX`、`Sound` 可任意单独或组合构建。底层 `Start` 目标保留，但选择游戏时使用编号目标。
`KAGUYA_GAME` 仅决定默认整批构建发布哪款作品，默认 `mamatomo`；显式编号不要求重新配置。

日志开关：

```text
cmake --preset clang-cl-release -DKAGUYA_ENABLE_LOGGING=ON
cmake --build --preset clang-cl-release
```

Release 默认 OFF，Debug 默认 ON；Debug 使用独立的 `clang-cl-debug` 预设。
切换后统一构建 EXE 和三个 DLL，避免混用不同配置的旧产物。

## 输出

- `Release/<game-slug>/Start.exe`：所选游戏的 EXE，目录名与游戏清单中的 `slug` 一致。
- 例如 `Start01` 输出到 `Release/baku-ane/Start.exe`，`Start18` 输出到 `Release/mamatomo/Start.exe`。
- `Release/Lib/`：三个配套 DLL。
- `Release/licenses/`：第三方许可证；发布时也应携带根目录 `LICENSE`。
- `Release/Debug/<game-slug>/Start.exe`、`Release/Debug/Lib/`：Debug EXE 和 DLL。

## 游戏目录简称

同一作品在 `Start/games/`、`Start/resources/`、构建缓存及 `Release/` 下使用一致的简短 `slug`。
作品全名和编号仍保留在游戏清单中；目录改名不改变资源内容、存档格式或编译目标。

| 目标 | 目录名 | 目标 | 目录名 |
|---|---|---|---|
| `Start01` | `baku-ane` | `Start10` | `mamagoto` |
| `Start02` | `namaiki` | `Start11` | `mama-x-holic` |
| `Start03` | `chu-x-pet` | `Start12` | `nee-nee-nee` |
| `Start04` | `harami-tama` | `Start13` | `anetomo` |
| `Start05` | `ero-zemi` | `Start14` | `bunnys-mama` |
| `Start06` | `baku-ane-2` | `Start15` | `pure-x-holic` |
| `Start07` | `shigokare` | `Start16` | `oppai-dekai-nurse` |
| `Start08` | `love-x-holic` | `Start17` | `hakoniwa` |
| `Start09` | `nee-chan-no-susume` | `Start18` | `mamatomo` |
