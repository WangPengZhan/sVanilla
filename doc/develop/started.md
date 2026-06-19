# 开发快速开始

本文档面向希望从源码构建、调试或参与开发 sVanilla 的开发者。

## 环境要求

### 通用工具

- Git
- CMake 3.15 或更高版本
- Python 3
- 支持 C++20 的编译器
- Qt 5 或 Qt 6，至少需要 Core、Widgets、Network、WebEngine、LinguistTools 等项目源码中使用到的模块

### 推荐环境

- Windows: Visual Studio 2022 + Desktop development with C++
- Linux: GCC/Clang、Ninja 或 Make、Qt 开发包
- macOS: Xcode Command Line Tools、Qt、Ninja 或 Make

项目默认使用仓库内的 `ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake` 作为 CMake toolchain。首次配置时，vcpkg 会按 `vcpkg.json` 安装依赖，耗时较长是正常现象。

## 获取源码

```sh
git clone https://github.com/WangPengZhan/sVanilla.git
cd sVanilla
git submodule update --init --recursive
```

子模块包含：

- `ThirdParty/vcpkg`: C/C++ 依赖包管理器
- `ThirdParty/spdlog`: 日志库
- `ThirdParty/sVanillaPluginCommon`: 插件公共组件
- `ThirdParty/sVanillaPlugins`: 可选插件源码，启用插件构建时使用

## 配置工程

### 基础配置

```sh
cmake -S . -B build
```

如需指定构建类型：

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

Windows + Visual Studio 示例：

```sh
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
```

Windows + Ninja 示例：

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
```

如果 Qt 不在默认搜索路径中，可以通过 CMake 变量指定 Qt 安装位置：

```sh
cmake -S . -B build -DQt6_DIR=/path/to/Qt/lib/cmake/Qt6
```

Qt 5 环境可改用 `Qt5_DIR`。

## 构建

```sh
cmake --build build --config Release
```

使用单配置生成器（例如 Ninja、Make）时，`--config Release` 可省略：

```sh
cmake --build build
```

主程序目标名为 `sVanilla`。构建产物通常位于：

- Visual Studio: `build/Release/` 或 `build/Debug/`
- Ninja/Make: `build/`

实际位置取决于 CMake 生成器和平台配置。

## 常用 CMake 选项

| 选项 | 默认值 | 说明 |
| --- | --- | --- |
| `ENABLE_TEST` | `OFF` | 启用单元测试 |
| `ENABLE_CLANG_TIDY` | `OFF` | 启用 clang-tidy 检查 |
| `ENABLE_BUILD_PLUGINS` | `OFF` | 构建插件源码 |
| `ENABLE_BUILD_EXAMPLE` | `OFF` | 构建示例工程 |

示例：

```sh
cmake -S . -B build -DENABLE_TEST=ON -DENABLE_BUILD_EXAMPLE=ON
cmake --build build
```

启用插件构建：

```sh
cmake -S . -B build -DENABLE_BUILD_PLUGINS=ON
cmake --build build
```

## 运行测试

先启用测试并构建：

```sh
cmake -S . -B build -DENABLE_TEST=ON
cmake --build build
```

然后运行：

```sh
ctest --test-dir build --output-on-failure
```

测试目录当前包含：

- `test/SQLite`
- `test/ThreadPool`
- `test/Zipper`

## Python 依赖

CMake 配置阶段会执行：

```sh
python -m pip install -r scripts/requirements.txt
```

当前 Python 依赖包括：

- `requests`
- `gitpython`
- `lxml`

如果配置阶段因 Python 或 pip 失败，可以先手动安装：

```sh
python -m pip install -r scripts/requirements.txt
```

## 项目结构

```text
sVanilla
|-- CMakeLists.txt                  # 顶层 CMake 工程
|-- README.md                       # 项目简介
|-- AGENTS.md                       # AI 智能体协作总规则
|-- vcpkg.json                      # vcpkg 依赖清单
|-- vcpkg-configuration.json        # vcpkg 配置
|-- .codex/                         # Codex 专用命令与适配说明
|-- cmake/                          # CMake 辅助脚本与模板
|-- doc/                            # 文档
|   |-- develop/                    # 开发文档
|-- example/                        # 示例工程
|-- harness/                        # 共享规范、SDD、需求与实现知识
|-- overlay/                        # vcpkg overlay ports/triplets/toolchains
|-- scripts/                        # 构建、打包、资源生成脚本
|-- sVanilla/
|   |-- resource/                   # 图标、样式、配置等资源
|   |-- translate/                  # Qt 翻译文件
|   |-- src/
|       |-- Aria2Net/               # aria2 通信封装
|       |-- BaseQt/                 # Qt 基础工具
|       |-- ClientUi/               # 主程序 UI 与业务入口
|       |-- Download/               # 下载相关逻辑
|       |-- Logger/                 # 日志模块
|       |-- Plugin/                 # 插件加载与管理
|       |-- Sqlite/                 # SQLite 封装与存储
|       |-- SUI/                    # 自定义 UI 组件
|       |-- ThreadPool/             # 线程池
|       |-- Zipper/                 # 压缩/解压模块
|-- test/                           # 单元测试
|-- ThirdParty/                     # Git 子模块与第三方源码
|-- tools/                          # 内置工具，例如 aria2、ffmpeg
```

## 常见问题

### 找不到 Qt

确认 Qt 已安装对应编译器版本的套件，并通过 `Qt6_DIR` 或 `Qt5_DIR` 指向 Qt 的 CMake 配置目录。例如：

```sh
cmake -S . -B build -DQt6_DIR=C:/Qt/6.8.0/msvc2022_64/lib/cmake/Qt6
```

### vcpkg 下载或构建依赖失败

确认网络可访问依赖源，并确保子模块已经初始化：

```sh
git submodule update --init --recursive
```

必要时删除 `build/` 后重新配置。

### Python 依赖安装失败

确认 `python --version` 和 `python -m pip --version` 可正常执行。也可以先手动安装：

```sh
python -m pip install --upgrade pip
python -m pip install -r scripts/requirements.txt
```

### Visual Studio 构建较慢

顶层 CMake 已为 MSVC 启用 `/MP` 并行编译选项。首次构建慢通常来自 vcpkg 编译第三方依赖，后续增量构建会明显更快。
