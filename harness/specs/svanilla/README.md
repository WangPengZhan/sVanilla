# sVanilla SDD 规格集

## 说明

本目录是面向 AI 智能体 使用的 SDD 规格基线。这里的 SDD 同时采用两层含义：

- Software Design Description：描述当前软件架构、模块、接口、数据和运行行为。
- Spec-Driven Development：后续需求、代码生成、评审和调试应以这些规格为基线。

## 使用规则

AI 智能体 在修改本仓库前应先读取：

1. `00-sdd-index.md`
2. 与任务相关的具体规格文件
3. `08-agent-working-rules.md`

如果代码与规格冲突：

- 以现有代码为事实来源。
- 在变更代码时同步更新规格。
- 如果只是发现规格过期，先更新规格并说明证据。

## 文件清单

| 文件 | 用途 |
| --- | --- |
| `00-sdd-index.md` | SDD 总览和阅读路由 |
| `01-system-context.md` | 系统边界、用户、外部依赖 |
| `02-functional-requirements.md` | 现有功能规格和验收标准 |
| `03-architecture-design.md` | 架构视图、模块职责、依赖关系 |
| `04-data-storage.md` | 配置、SQLite 表、持久化规则 |
| `05-interfaces-contracts.md` | 插件、下载器、配置、存储等接口契约 |
| `06-runtime-flows.md` | 启动、解析、下载、登录、插件、退出流程 |
| `07-build-test-quality.md` | 构建、测试、CI、质量约束 |
| `08-agent-working-rules.md` | AI 智能体 在本仓库工作的规范 |
| `traceability.md` | 功能到代码、测试、规格的追踪矩阵 |
