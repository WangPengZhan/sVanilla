# Harness Engineering + SDD

本目录保存 sVanilla 的工具无关工程上下文，供 AI 智能体、Claude、Cursor 以及其他编码智能体共同使用。

## 目录结构

- `standards/`：团队工程规范，包括 Git、日志、自测和变更规则。
- `specs/svanilla/`：sVanilla 的 SDD 规格基线。
- `requirements/`：版本化需求、评审后需求和落地记录。
- `knowledge/`：按模块分类的可复用实现知识。
- `ai-experience/`：工具无关的 AI 协作经验、坑点和偏好。
- `workflows/`：Harness + SDD 工作流。
- `templates/`：需求分析、规格设计、评审、验收和复盘模板。

## 推荐入口

执行需求前，智能体应优先读取：

1. `harness/standards/`
2. `harness/specs/svanilla/00-sdd-index.md`
3. `harness/specs/svanilla/08-agent-working-rules.md`
4. `harness/requirements/` 中与任务相关的文件
5. `harness/knowledge/` 中与模块相关的文件

## 工具适配层

`.codex/`、Claude 或 Cursor 的配置目录只应保存工具专用命令、提示词和集成说明。共享的项目事实、规范、需求、SDD 和可复用知识应先写入 `harness/`。

