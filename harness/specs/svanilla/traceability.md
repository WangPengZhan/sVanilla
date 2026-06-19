# Traceability Matrix

## 功能到代码

| 功能 | 规格 | 代码位置 | 测试 |
| --- | --- | --- | --- |
| 启动和运行模式 | FR-001 | `ClientUi/main.cpp`、`ClientUi/CommandLine` | Open |
| 单实例控制 | FR-002 | `ClientUi/MainWindow/SingleAppHelper.*` | Open |
| URL 输入和历史 | FR-003 | `ClientUi/Home`、`ClientUi/VideoList`、`ClientUi/Storage/SearchHistoryStorage.*` | Open |
| 插件管理 | FR-004 | `sVanilla/src/Plugin`、`ThirdParty/sVanillaPluginCommon` | Open |
| 媒体列表展示 | FR-005 | `ClientUi/VideoList` | Open |
| 下载任务 | FR-006 | `ClientUi/Download`、`Download`、`ThirdParty/sVanillaPluginCommon/Download` | Open |
| 下载记录 | FR-007 | `ClientUi/Storage/DownloadingItemStorage.*`、`DownloadedItemStorage.*` | Open |
| 登录和 Cookie | FR-008 | `ClientUi/Login`、`ClientUi/Storage/CookiesInfoStorage.*` | Open |
| 应用设置 | FR-009 | `ClientUi/Config/SingleConfig.*`、`ClientUi/Utils/Setting.*`、`ClientUi/Setting` | Open |
| 平台初始化 | FR-010 | `ClientUi/main.cpp`、`ClientUi/PlatformInitializer`、`Aria2Net` | Open |
| 压缩和解压 | FR-011 | `Zipper` | `test/Zipper` |
| 线程池 | FR-012 | `ThreadPool` | `test/ThreadPool` |

## 模块到规格

| 模块 | 相关规格 |
| --- | --- |
| `ClientUi` | `02-functional-requirements.md`、`03-architecture-design.md`、`06-runtime-flows.md` |
| `Plugin` | `05-interfaces-contracts.md`、`06-runtime-flows.md` |
| `ThirdParty/sVanillaPluginCommon` | `05-interfaces-contracts.md` |
| `Sqlite` | `04-data-storage.md`、`05-interfaces-contracts.md` |
| `ThreadPool` | `05-interfaces-contracts.md`、`07-build-test-quality.md` |
| `Zipper` | `05-interfaces-contracts.md`、`07-build-test-quality.md` |
| `Aria2Net` | `05-interfaces-contracts.md`、`06-runtime-flows.md` |
| Build/CI | `07-build-test-quality.md` |

## 规格缺口

| 缺口 | 影响 | 建议 |
| --- | --- | --- |
| 命令行模式细节未完全展开 | CLI 行为验收不完整 | 后续读取 `CommandLine.cpp` 后补充 |
| 插件配置文件格式未完全展开 | 插件配置迁移规则不完整 | 后续读取 `PluginManager.cpp` 后补充 |
| aria2 下载参数未完全展开 | 高级下载配置验收不完整 | 后续读取 Aria2 client 和设置页面实现 |
| UI 手动验收路径未逐页列出 | UI 改动验证依赖人工经验 | 后续按页面补充验收脚本或清单 |
