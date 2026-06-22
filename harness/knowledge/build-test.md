# Build And Test Knowledge

## 模块范围

- CMake。
- vcpkg。
- GoogleTest。
- CI。

## 已知规则

- 测试开关为 `ENABLE_TEST`。
- 现有测试目录包括 `test/SQLite`、`test/ThreadPool`、`test/Zipper`、`test/Audit`。
- UI 任务无法自动化时，应至少构建相关 target 并提供手工验收步骤。

## 常用命令

```sh
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DENABLE_TEST=ON
cmake --build build --config Debug --parallel --target all
ctest --test-dir build --build-config Debug --output-on-failure
```

## 2026-06-20 Audit Tests

- `Audit_test` covers downloader exception containment and PluginProxy exception isolation.
- `Zipper_test` generates archives at runtime, covers ZIP64 metadata and unsafe path rejection, and must not use developer-specific absolute paths.
- On Windows, `Audit_test` copies the shared spdlog runtime beside the test executable in a post-build command.
