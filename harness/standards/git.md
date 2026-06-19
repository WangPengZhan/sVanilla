# Git Standard

## 分支和提交

- 一个提交对应一个清晰目的。
- 不把格式化、重构和功能修改混在同一提交，除非改动很小且强相关。
- 不提交 `build/`、`out/`、临时文件、平台生成文件。
- 不回滚用户已有改动，除非用户明确要求。

## 提交信息

建议格式：

```text
type(scope): summary
```

常用 type：

- feat：新功能。
- fix：修复缺陷。
- refactor：不改变行为的重构。
- test：测试。
- docs：文档。
- build：构建或依赖。
- chore：杂项维护。

示例：

```text
fix(plugin): persist enabled state from manager UI
docs(agent): add harness SDD workflow standards
```

## 提交前检查

- `git status --short`
- 检查 diff 是否只包含本次需求相关内容。
- 运行最小相关验证，并在提交说明或 PR 中记录。
