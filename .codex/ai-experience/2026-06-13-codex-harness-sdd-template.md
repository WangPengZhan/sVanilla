# 2026-06-13 Codex Harness SDD Template

## 背景

用户希望把 Harness Engineering、SDD、团队规范、需求归档、AI 经验复盘和实现知识库结合成一套 Codex 工作模板。

## 经验

- 只放 SDD 不够，Codex 还需要团队规范、需求文件夹、分类知识和经验沉淀入口。
- `AGENTS.md` 应作为总入口，`.codex/` 应承载可维护的规范资产。
- 需求文件应保留原始需求，并追加评审后需求和最终落地结果，避免覆盖历史。
- 模块知识应按任务路由读取，避免每次把所有细节塞进上下文。

## 后续使用

新需求默认按 `AGENTS.md` 的必读路由执行，并在任务结束时判断是否更新 `ai-experience/` 或 `knowledge/`。
