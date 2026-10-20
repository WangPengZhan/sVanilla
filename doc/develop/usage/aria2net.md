# Aria2Net 使用

启动 / 关闭随程序分发的 aria2 本地服务。设计原理与命令行参数见 [`../architecture/aria2net.md`](../architecture/aria2net.md)。

## 一、启动本地服务

```cpp
aria2net::AriaServer server;

server.setLogDir(logRoot);                     // 必须在 start 之前
server.setErrorFunc([this] {
    // 启动失败：提示用户或降级
});

server.startLocalServerAsync();                // 立即返回
```

## 二、关闭

```cpp
server.closeServer();                          // 与 forceCloseServer 等价
```

## 三、由 `SApplication` 托管

`AriaServer` 由 `SApplication` 以**值成员**持有，`SApplication::init()` 中在非远程模式下调用 `startLocalServerAsync()`，见 [clientui.md](clientui.md)。

## 四、调用约定与陷阱

| 标注 | 内容 |
| --- | --- |
| `[约束]` | **`setLogDir()` 应在启动前调用。** 它内部执行 `m_dir = dir + "/aria"` 并 `mkpath`；未调用时 `m_dir` 为空，会话与日志目录回退到 `applicationDirPath() + "/aria"`（该目录需存在，否则创建会话文件会失败）。 |
| `[约束]` | **启动失败只能通过 `setErrorFunc` 的回调感知**——没有成功回调，`startLocalServerAsync()` 的返回值被丢弃。需要"启动成功后再做事"的流程必须自行加轮询或超时。 |
| `[事实]` | 会话与日志落在 `<dir>/aria.session` 与 `<dir>/aria.log`；升级前遗留的 `aira.*` 文件不再被读取，可手工清理。 |
| `[陷阱]` | **RPC 地址与密钥是硬编码的**（端口 6800、`--rpc-secret=sVanilla`）。若本机已有其它 aria2 占用 6800，或用同一密钥连上别的实例，行为不可控。 |
| `[陷阱]` | **`closeServer()` 与 `forceCloseServer()` 实现完全相同**（都是 `kill()` + `waitForFinished()` + `reset()`），没有"优雅退出"与"强制退出"的区别。 |
| `[陷阱]` | **析构函数会阻塞。** 析构里用 `promise/future` 同步等待另一线程完成 `kill()` + `waitForFinished()`；若 aria2c 不响应 `kill`，析构会一直卡住。不要在主线程的高频路径上构造 / 析构它。 |
| `[陷阱]` | **`AriaServer.h` 只 include 了 `<future>` 与 `<QString>`**，却用到 `std::function`、`std::unique_ptr`、`std::vector` 等，依赖传递包含。 |
| `[事实]` | `testServer()` 是空实现（只有一句注释），没有健康检查 / 版本查询能力；`setCloseFunc()` 写入的回调已连接到 `QProcess::finished`，但当前主仓库没有调用 `setCloseFunc()`。 |
