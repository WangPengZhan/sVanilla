# Codex Harness + SDD

本目录用于让 Codex 在 sVanilla 项目中按 Harness Engineering、SDD 和经验沉淀闭环工作。

## 目录结构

- `standards/`: 团队工程规范，例如日志、Git、自测、代码变更规则。
- `specs/svanilla/`: 项目的 SDD 规格基线。
- `requirements/`: 版本化需求、原始需求、评审后需求和最终落地需求。
- `knowledge/`: 按模块分类的实现知识，让 Codex 同时掌握大框架和小细节。
- `ai-experience/`: 每次 AI 运行后的经验、踩坑、决策和可复用提示。
- `workflows/`: Harness + SDD 工作流。
- `commands/`: 可复用的 Codex 任务提示。
- `templates/`: 需求、设计、评审、验收、复盘等输出模板。

## 推荐使用方式

完整实现：

```text
按 Harness + SDD 流程实现：<你的需求>
```

只做前置分析：

```text
先只做需求分析、规格设计和评审，不改代码：<你的需求>
```

要求沉淀经验：

```text
完成后把本次 AI 经验和实现知识沉淀到 .codex。
```

## 文档更新规则

- 需求变化：更新 `requirements/`。
- 团队约定变化：更新 `standards/`。
- 模块实现规律变化：更新 `knowledge/`。
- AI 协作经验变化：更新 `ai-experience/`。
- 行为、接口、数据、运行流程或构建方式变化：更新 `specs/svanilla/`。
