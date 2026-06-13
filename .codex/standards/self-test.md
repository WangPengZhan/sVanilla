# Self-Test Standard

## 原则

- 自测要覆盖本次需求的主要风险，而不是只证明能编译。
- 优先运行最小相关验证，再根据风险扩大范围。
- 无法自动化验证时，必须给出手工验收路径。

## 常用命令

```sh
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DENABLE_TEST=ON
cmake --build build --config Debug --parallel --target all
ctest --test-dir build --build-config Debug --output-on-failure
python scripts/clang_format_all.py
```

## 结果记录

最终回复和验收报告必须记录：

- 命令。
- 结果。
- 失败原因。
- 未验证内容。
- 残余风险。

## 手工验收

UI、插件、下载流程如果难以自动化，必须写出可重复步骤、预期结果和观察点。
