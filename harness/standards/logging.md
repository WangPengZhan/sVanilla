# Logging Standard

## 目标

日志用于定位用户环境中的真实问题，不用于替代错误处理。

## 级别

- Debug：开发期细节，例如分支选择、临时状态、非关键计数。
- Info：关键业务流程完成，例如插件加载完成、配置保存成功。
- Warn：可恢复异常，例如单个插件加载失败但应用可继续运行。
- Error：用户操作失败、数据写入失败、关键依赖不可用。

## 内容

日志应包含：

- 模块名或上下文。
- 关键 ID，例如 `pluginId`、`downloadId`、`config key`。
- 操作结果。
- 失败原因或错误码。

日志避免：

- Cookie、token、密码、完整个人路径等敏感信息。
- 高频循环内无节制输出。
- 只写 `failed` 而没有上下文。

## 线程日志

后台线程的创建和退出必须打 `info` 日志，所有后台线程都必须覆盖。

## 容量与 Qt 日志

- 每个 logger 使用 50 MiB 单文件上限并保留当前文件与 10 个轮转文件，总计最多 11 个文件。
- Release 构建必须将 Qt/WebEngine 的 Debug、Info、Warning、Critical 和 Fatal 消息转发到 Client logger，不得使用空 handler 全部吞掉。
- Qt 日志应保留 category、源文件、行号和函数上下文；Fatal 日志必须立即 flush。

## 插件管理示例

启用或禁用插件时，至少记录 `pluginId`、目标状态、持久化结果和失败原因。
