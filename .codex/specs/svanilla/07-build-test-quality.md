# Build, Test And Quality Specification

## 构建系统

根入口：

- `CMakeLists.txt`

关键设置：

- `CMAKE_CXX_STANDARD 20`
- vcpkg toolchain：`ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake`
- 默认构建类型：`Release`
- 编译定义：`UNICODE`

## CMake 选项

| 选项 | 默认 | 用途 |
| --- | --- | --- |
| `ENABLE_TEST` | `OFF` | 启用 `test` |
| `ENABLE_CLANG_TIDY` | `OFF` | 启用 clang-tidy |
| `ENABLE_BUILD_PLUGINS` | `OFF` | 构建插件 |
| `ENABLE_BUILD_EXAMPLE` | `OFF` | 构建示例 |
| `ENABLE_DEPLOY` | `ON` | ClientUi 部署逻辑 |

## 主要 target

| Target | 类型 | 位置 |
| --- | --- | --- |
| `sVanilla` | executable | `sVanilla/src/ClientUi` |
| `Aria2Net` | static library | `sVanilla/src/Aria2Net` |
| `BaseQt` | static library | `sVanilla/src/BaseQt` |
| `Download` | static library | `sVanilla/src/Download` |
| `Logger` | static library | `sVanilla/src/Logger` |
| `Plugin` | static library | `sVanilla/src/Plugin` |
| `Sqlite` | static library | `sVanilla/src/Sqlite` |
| `SUI` | static library | `sVanilla/src/SUI` |
| `ThreadPool` | static library | `sVanilla/src/ThreadPool` |
| `Zipper` | static library | `sVanilla/src/Zipper` |

## 推荐验证命令

配置和测试：

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DENABLE_TEST=ON
cmake --build build --config Debug --parallel --target all
ctest --test-dir build --build-config Debug --output-on-failure
```

格式化：

```bash
python3 scripts/clang_format_all.py
```

clang-tidy：

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DENABLE_CLANG_TIDY=ON
cmake --build build --config Release --parallel --target all
```

## 测试覆盖

| 测试目录 | 覆盖模块 |
| --- | --- |
| `test/SQLite` | SQLite 封装和 SQL/storage 行为 |
| `test/ThreadPool` | 线程池 |
| `test/Zipper` | 压缩和解压 |

## CI

| Workflow | 目的 |
| --- | --- |
| `ci-linux.yml` | Linux 构建和安装 |
| `ci-windows.yml` | Windows 构建和安装 |
| `ci-macos.yml` | macOS 构建和安装 |
| `ci-clang-format-check.yml` | clang-format 检查 |
| `ci-clang-tidy.yml` | clang-tidy 构建检查 |
| `ci-release.yml` | tag release 打包 |

## 质量约束

- 新增 C++ 源文件应符合 `.clang-format`。
- 影响 `Sqlite`、`ThreadPool`、`Zipper` 的变更优先补充对应测试。
- 影响 UI 的变更至少说明手动验收路径。
- 影响插件接口的变更必须同步更新插件规格和兼容性说明。
- 影响 CMake、vcpkg、CI 的变更必须说明平台影响。
