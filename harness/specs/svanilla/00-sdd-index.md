# SDD Index

## 项目快照

- 项目：sVanilla
- 类型：跨平台桌面下载器
- 语言：C++20
- UI：Qt Widgets、Qt WebEngine
- 构建：CMake、vcpkg
- 测试：GoogleTest、CTest
- 主要平台：Windows、Linux、macOS

## 事实来源

本规格基于当前代码和工程文件反向生成，主要证据包括：

- `README.md`
- `CMakeLists.txt`
- `vcpkg.json`
- `.github/workflows`
- `sVanilla/src`
- `ThirdParty/sVanillaPluginCommon`
- `ThirdParty/sVanillaPlugins`
- `test`

## 规格状态

| 状态 | 含义 |
| --- | --- |
| Confirmed | 由代码、构建文件或现有文档直接支持 |
| Assumption | 根据结构和命名推断，后续需要代码或人工确认 |
| Open | 当前规格无法确认，需要后续需求或代码调查 |

## 阅读路由

| 任务类型 | 优先阅读 |
| --- | --- |
| 新功能需求 | `02-functional-requirements.md`、`03-architecture-design.md`、`08-agent-working-rules.md` |
| UI 修改 | `02-functional-requirements.md`、`03-architecture-design.md`、`06-runtime-flows.md` |
| 下载逻辑 | `05-interfaces-contracts.md`、`06-runtime-flows.md` |
| 插件逻辑 | `05-interfaces-contracts.md`、`06-runtime-flows.md` |
| 数据持久化 | `04-data-storage.md`、`05-interfaces-contracts.md` |
| 构建或 CI | `07-build-test-quality.md` |
| 调试问题 | `06-runtime-flows.md`、`traceability.md` |

## 维护要求

每次修改代码时，AI 智能体 应检查：

- 是否改变用户可见行为。
- 是否改变模块接口或数据结构。
- 是否改变构建、打包、部署或测试命令。
- 是否需要同步更新本目录规格。
