# Download Knowledge

## 模块范围

- 下载 UI。
- `sVanilla/src/Download`。
- Aria2 交互。

## 已知规则

- 下载任务状态变更需要考虑 UI、队列、持久化和实际下载器状态的一致性。
- 不要改变下载唯一 ID，除非有迁移方案。
- `DownloadStatusThread` 应以 `shared_ptr` 快照保证锁外任务生命周期，不得持有任务集合锁调用真实下载器。
- 锁外处理结束后，只能删除 key 和下载器实例均与快照匹配的任务，避免误删同 key 的后续实例。
- 下载状态线程创建、开始运行和退出都应打 info 日志，便于定位后台下载循环是否活着。
- 每个下载器调用都有独立异常边界。异常任务设置为 Error 并移出状态线程容器，其他任务继续执行。
- Error 与 Stopped、Finished 一样是调度终态；UI 可以继续持有任务用于展示或重试，但状态线程不得继续轮询。

## 证据

- 需要结合实际任务读取 `sVanilla/src/Download` 和相关 UI。
