# sVanilla Codex Instructions

## 语言与协作

- 默认使用中文回复，除非用户明确要求其他语言。
- 面向工程结果工作：理解需求、读取规范和代码、设计、实现、验证、总结。
- 当用户提出明确需求时，优先完成可落地的修改，而不是只给建议。
- 如果需求不清，先基于仓库事实做合理假设；只有存在高风险歧义时才向用户提问。

## 工作模式：Harness Engineering + SDD + Knowledge Loop

本仓库采用三层工作法：

- Harness Engineering：把需求拆成可执行、可验证、可复用的工程闭环。
- SDD：同时表示 Software Design Description 和 Spec-Driven Development。
- Knowledge Loop：每次 AI 运行后沉淀经验、坑点、决策和可复用实现知识。

默认执行顺序：

1. 读取总规则：`AGENTS.md`。
2. 读取团队规范：`.codex/standards/`。
3. 读取 SDD：`.codex/specs/svanilla/00-sdd-index.md`、相关规格、`.codex/specs/svanilla/08-codex-working-rules.md`。
4. 读取需求档案：`.codex/requirements/` 中与任务相关的版本需求或落地需求。
5. 读取实现知识：`.codex/knowledge/` 中与模块相关的分类知识。
6. 读取实际代码，验证文档是否仍然成立。
7. 输出压缩版需求分析、规格设计和评审结论。
8. 实现、验证、总结。
9. 如有新经验、坑点、约定或实现细节，更新 `.codex/ai-experience/` 或 `.codex/knowledge/`。

小任务可以压缩文档输出，但不能跳过关键判断、验证和经验沉淀。

## 必读路由

每次任务至少读取：

- `.codex/standards/engineering.md`
- `.codex/standards/git.md`
- `.codex/standards/logging.md`
- `.codex/standards/self-test.md`
- `.codex/specs/svanilla/00-sdd-index.md`
- `.codex/specs/svanilla/08-codex-working-rules.md`

按任务类型追加读取：

- 新功能或 UI：`02-functional-requirements.md`、`03-architecture-design.md`、`06-runtime-flows.md`、`.codex/knowledge/client-ui.md`
- 数据持久化：`04-data-storage.md`、`05-interfaces-contracts.md`、`.codex/knowledge/storage-sqlite.md`
- 插件或下载：`05-interfaces-contracts.md`、`06-runtime-flows.md`、`.codex/knowledge/plugin.md`、`.codex/knowledge/download.md`
- 构建、测试、CI：`07-build-test-quality.md`、`.codex/knowledge/build-test.md`
- 历史需求或版本范围：`.codex/requirements/README.md` 和相关需求文件
- 经验复盘：`.codex/ai-experience/README.md`

如果文档与代码冲突：

- 以当前代码、构建文件和可复现实验作为事实来源。
- 本次改动改变行为、接口、数据、构建、测试或团队规范时，同步更新相关文档。
- 只发现文档过期时，先更新文档并说明证据。

## sVanilla 项目事实

- 技术栈：C++20、CMake、vcpkg、Qt Widgets/Qt WebEngine。
- 主程序：`sVanilla/src/ClientUi`。
- 静态库：`sVanilla/src/*`。
- 资源与样式：`sVanilla/resource`。
- 翻译：`sVanilla/translate`。
- 插件相关：`sVanilla/src/Plugin`、`ThirdParty/sVanillaPluginCommon`、`ThirdParty/sVanillaPlugins`。
- 测试：`test/SQLite`、`test/ThreadPool`、`test/Zipper`。
- 默认不要修改 `ThirdParty`、`build`、`out`、平台工具二进制或生成产物，除非需求明确要求。

## 变更原则

- 优先沿用现有架构、命名、CMake target、Qt 模式和测试组织。
- 保持改动范围最小；不要把顺手重构混入需求实现。
- 不要回滚用户已有修改。
- 行为变更必须有验收标准；高风险逻辑应补测试或说明无法自动化验证的原因。
- 插件 API、持久化 key、数据库结构、下载唯一 ID、Cookie 唯一维度等变更必须显式说明兼容性和迁移策略。

## 推荐验证

按风险选择最小相关验证：

```sh
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DENABLE_TEST=ON
cmake --build build --config Debug --parallel --target all
ctest --test-dir build --build-config Debug --output-on-failure
python scripts/clang_format_all.py
```

如果命令因本地环境、Qt、vcpkg、网络或权限失败，必须在最终回复中说明。

## 输出要求

最终回复应包含：

- 做了什么。
- 涉及的规范、需求、知识和 SDD 文件。
- 涉及的代码文件。
- 执行过的验证命令与结果。
- 未验证内容和残余风险。
- 是否沉淀了 AI 经验或实现知识。
- 是否需要用户做人工确认。
