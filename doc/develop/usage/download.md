# Download 使用

把下载任务交给轮询线程、按契约实现下载器。设计原理与状态映射表见 [`../architecture/download.md`](../architecture/download.md)。

## 一、提交与停止

```cpp
download::DownloadStatusThread thread;                  // 构造即启动线程

thread.addTask(downloader);                             // 返回 false 表示入参为空或同 guid 已存在
// ...
thread.stop();                                          // 或由析构自动调用
```

## 二、下载器需要满足的契约

`AbstractDownloader`（定义在子模块 `sVanillaPluginCommon`）至少需要：

- `guid()` 返回稳定且唯一的任务标识；
- `status()` 返回当前状态，并在回调中**及时同步改变状态**（否则会被反复调用，见下）；
- 实现 6 个回调：`start` / `downloadStatus` / `pause` / `resume` / `stop` / `finish`。

## 三、在 ClientUi 中的位置

该线程由 `SApplication` 以值成员持有（`sApp->downloadThread()`），由 `DownloadManager::addItem()` 提交任务；真正把 UI 与下载器粘起来的是 `UiDownloader`。见 [clientui.md](clientui.md)，相关单元测试见 `test/Download`。

## 四、调用约定与陷阱

| 标注 | 内容 |
| --- | --- |
| `[约束]` | **回调里必须同步改变 `status()`。** 分派依据是**当前** `status()`；若某个下载器在 `start()` 后没有及时把状态切走，下次轮询（+600 ms）会再次调用 `start()`，造成重复启动。 |
| `[约束]` | **不要依赖"失败会收到 `error()` 回调"。** `Error` 状态只把任务移除，**不调用任何虚函数**——需要感知失败只能在下载器内部处理，或对比任务是否从表中消失。 |
| `[陷阱]` | **`stop()` 后线程可能仍睡满 600 ms。** `stop()` 只做 `m_running = false` + `notify_all()`；若线程正处于 `sleep_for` 中，必须等这次 sleep 结束才会检查标志位。因此**析构函数 `join()` 最长会阻塞约 600 ms**，在 UI 线程析构会表现为卡顿。 |
| `[陷阱]` | **600 ms 是"每轮末尾固定 sleep"，不是定时器。** 若一次遍历中所有任务的处理耗时很长，实际轮询间隔会明显大于 600 ms。 |
| `[约束]` | 回调在**锁外**执行，因此回调里可以再 `addTask` / 查询而不会死锁；但同一轮内新增的任务不会被本轮看到。 |
| `[事实]` | `addTask` 的唤醒条件是"任务表非空"而非 `insert` 是否成功，逻辑上无害但语义反直觉；真正重复时函数返回 `false`。 |
| `[事实]` | 状态枚举的完整取值以子模块 `sVanillaPluginCommon` 为准；主仓库可验证的是 `Ready` / `Downloading` / `Pause` / `Resumed` / `Stopped` / `Finished` / `Error` 七种及其映射。 |
