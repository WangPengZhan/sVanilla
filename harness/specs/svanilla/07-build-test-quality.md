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
cmake -B out/harness -S . -DCMAKE_BUILD_TYPE=Debug -DENABLE_TEST=ON
cmake --build out/harness --config Debug --parallel
ctest --test-dir out/harness --build-config Debug --output-on-failure
```

Harness/AI 本地验证固定使用 `out/harness/` 构建树，不得复用或清理开发者维护的 `build/`。

格式化：

```bash
python3 scripts/clang_format_all.py
```

clang-tidy：

```bash
cmake -B out/harness/clang-tidy -S . -DCMAKE_BUILD_TYPE=Release -DENABLE_CLANG_TIDY=ON
cmake --build out/harness/clang-tidy --config Release --parallel
```

## 测试覆盖

| 测试目录 | 覆盖模块 |
| --- | --- |
| `test/SQLite` | SQLite 封装和 SQL/storage 行为 |
| `test/ThreadPool` | 线程池 |
| `test/Zipper` | 压缩和解压 |
| `test/Base`、`test/Logger`、`test/BaseQt` | 基础工具、日志、Qt 通用工具 |
| `test/Download`、`test/Plugin`、`test/Aria2Net` | 下载状态与异常回收、插件及代理异常隔离、aria2 进程管理 |
| `test/SUI`、`test/ClientUi` | UI 组件和 ClientUi 纯逻辑 |

## Third-Party License Disclosure

- `scripts/get_licenses.py` generates `licenses.json` beside the application executable; the About page reads this file to display dependency license text.
- Keep the generator's library list and `README.md` acknowledgements aligned with `vcpkg.json`, overlay port manifests, bundled tools, and plugin submodules.
- The generator reads package versions from `${VCPKG_INSTALLED_DIR}/vcpkg/status`; the About page overrides these with runtime/header version APIs when available.
- Existing `licenses.json` files are refreshed with current package versions without re-fetching license text; missing library entries trigger license generation.

## CI

| Workflow | 目的 |
| --- | --- |
| `ci-linux.yml` | Linux 构建和安装 |
| `ci-win.yml` | Windows 构建和安装 |
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
