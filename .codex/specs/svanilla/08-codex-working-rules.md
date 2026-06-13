# Codex Working Rules For sVanilla

## 规格优先

Codex 在本仓库执行需求时，应采用以下顺序：

1. 阅读本 SDD 目录的相关规格。
2. 阅读实际代码验证规格是否仍然有效。
3. 输出需求分析和设计摘要。
4. 修改代码。
5. 运行最小相关验证。
6. 如行为、接口、数据或构建发生变化，同步更新 SDD。

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

## 禁止或谨慎事项

- 不要随意修改 `ThirdParty`，除非需求明确涉及插件接口或第三方集成。
- 不要修改 `build` 目录产物。
- 不要把 UI 逻辑下沉到基础库。
- 不要让核心库反向依赖 `ClientUi`。
- 不要改变 `pluginId`、下载 `uniqueId`、Cookie 唯一维度等持久化关键字段，除非有迁移方案。

## 验证策略

按风险选择最小验证：

- UI-only：说明手动路径，能构建时构建 `sVanilla`。
- SQLite：运行或补充 `test/SQLite`。
- ThreadPool：运行或补充 `test/ThreadPool`。
- Zipper：运行或补充 `test/Zipper`。
- CMake/依赖/CI：至少运行 CMake configure，说明平台影响。
- 插件接口：检查主程序和插件公共库，同时说明 ABI/API 影响。

## 输出要求

最终回复应包含：

- 修改内容。
- 涉及规格文件。
- 涉及代码文件。
- 验证命令和结果。
- 未验证内容和原因。
- 是否需要后续人工确认。
