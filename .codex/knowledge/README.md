# Implementation Knowledge

本目录按分类记录实现知识。Codex 执行任务时应先读大框架，再按模块读取细节。

## 文件

- `client-ui.md`: 主界面、设置页、插件管理页等 UI 规律。
- `plugin.md`: 插件加载、启用状态、插件契约。
- `download.md`: 下载流程和状态。
- `storage-sqlite.md`: 配置、SQLite 和持久化。
- `build-test.md`: CMake、vcpkg、测试和 CI。

## 写入规则

- 只记录可复用知识，不记录一次性流水账。
- 每条知识应包含证据位置、适用范围和注意事项。
- 如果是推断，标记为 Assumption；如果由代码验证，标记为 Confirmed。
