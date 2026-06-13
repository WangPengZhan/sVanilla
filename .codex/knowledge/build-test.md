# Build And Test Knowledge

## 模块范围

- CMake。
- vcpkg。
- GoogleTest。
- CI。

## 已知规则

- 测试开关为 `ENABLE_TEST`。
- 现有测试目录包括 `test/SQLite`、`test/ThreadPool`、`test/Zipper`。
- UI 任务无法自动化时，应至少构建相关 target 并提供手工验收步骤。

## 常用命令

```sh
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DENABLE_TEST=ON
cmake --build build --config Debug --parallel --target all
ctest --test-dir build --build-config Debug --output-on-failure
```
