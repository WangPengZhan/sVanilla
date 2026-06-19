# Agent Working Rules For sVanilla

## 范围

这些规则适用于任何在本仓库工作的 AI 编码智能体。工具专用说明可以补充操作细节，但不应覆盖 `harness/` 中的项目事实、团队规范或 SDD 契约。

## 规格优先级

智能体执行需求时，应采用以下顺序：

1. 读取 `harness/specs/svanilla/` 中的相关规格。
2. 读取实际代码，验证规格是否仍然有效。
3. 对非平凡任务输出压缩版需求分析、设计摘要和评审结论。
4. 修改代码或文档。
5. 运行最小相关验证。
6. 如果行为、接口、数据、运行流程、构建、测试或团队规则发生变化，同步更新相关 `harness/` 文档。

## 修改路由

| 需求 | 优先位置 |
| --- | --- |
| 主窗口、导航、主题、托盘 | `sVanilla/src/ClientUi/MainWindow` |
| URL 输入和主页 | `sVanilla/src/ClientUi/Home` |
| 媒体展示、排序、搜索 | `sVanilla/src/ClientUi/VideoList` |
| 下载 UI 和下载状态 | `sVanilla/src/ClientUi/Download`、`sVanilla/src/Download` |
| 插件加载和配置 | `sVanilla/src/Plugin` |
| 插件接口 | `ThirdParty/sVanillaPluginCommon/PluginCommon` |
| 本地存储 | `sVanilla/src/ClientUi/Storage`、`sVanilla/src/Sqlite` |
| 账号和 Cookie | `sVanilla/src/ClientUi/Login`、`sVanilla/src/ClientUi/Setting` |
| 线程池 | `sVanilla/src/ThreadPool` |
| 压缩/解压 | `sVanilla/src/Zipper` |
| 构建和打包 | `CMakeLists.txt`、`cmake`、`scripts`、`.github/workflows` |

## 谨慎事项

- 不要随意修改 `ThirdParty`，除非需求明确涉及插件 API 或第三方集成。
- 不要修改 `build`、`out`、生成产物、平台二进制或依赖缓存。
- 不要把 UI 逻辑下沉到核心库。
- 不要让核心库反向依赖 `ClientUi`。
- 不要改变 `pluginId`、下载 `uniqueId`、Cookie 唯一维度等稳定持久化字段，除非有迁移方案。

## 验证策略

按风险选择最小验证：

- UI-only：说明手工路径；可构建时构建 `sVanilla`。
- SQLite：运行或补充 `test/SQLite`。
- ThreadPool：运行或补充 `test/ThreadPool`。
- Zipper：运行或补充 `test/Zipper`。
- CMake/依赖/CI：至少运行 CMake configure，并说明平台影响。
- 插件接口：检查主程序和插件公共库，同时说明 API/ABI 影响。

## 最终回复要求

最终回复应包含：

- 修改内容。
- 涉及或更新的 `harness/` 规范、需求、知识和 SDD 文件。
- 涉及的代码文件。
- 验证命令和结果。
- 未验证内容和原因。
- 是否沉淀了 AI 经验或实现知识。
- 是否需要后续人工确认。

