# ThreadPool 使用

提交后台任务、把结果安全送回界面线程。设计原理见 [`../architecture/threadpool.md`](../architecture/threadpool.md)。

## 一、提交普通任务并取结果

```cpp
ThreadPool pool;                       // 默认 hardware_concurrency() 个线程，建议显式传参

auto future = pool.enqueue([](int a, int b) { return a + b; }, 1, 2);
int value = future.get();              // 3
```

## 二、提交 `Task` 对象（结果走 Qt 信号）

```cpp
auto task = std::make_shared<TemplateSignalReturnTask<std::function<int()>>>(
    [] { return heavyCompute(); }, /*parent*/ someQObject);

QObject::connect(task.get(), &SignalReturnTask::result, someQObject,
                 [](const std::any& ret) {
                     int v = std::any_cast<int>(ret);   // 已在 someQObject 线程执行
                 });

pool.enqueue(task);
```

## 三、与 `runTask` 配合（ClientUi 的推荐方式）

业务代码一般不直接碰 `ThreadPool`，而是用 `ClientUi/Utils/RunTask.h` 的 `runTask(taskFunc, callback, object)`，它把上面的模式固定下来：

```
封装为 TemplateSignalReturnTask → connect(result, object, ...) → sApp->threadPool().enqueue(task)
```

见 [clientui.md](clientui.md)。

## 四、生命周期

- 让 `ThreadPool` 作为长生命周期对象持有（`SApplication` 以值成员持有）。
- 需要"立即丢弃排队任务"时用 `drop()`；需要"停掉线程池"时用 `stop()`（析构会自动调用）。

## 五、调用约定与陷阱

| 标注 | 内容 |
| --- | --- |
| `[事实]` | 默认参数取 `hardware_concurrency()`；若平台返回 0，构造函数会回退为 1 个工作线程，任务仍可执行。需要固定并发度时显式传入线程数。 |
| `[陷阱]` | **`stop()` 会丢弃尚未执行的任务并使其 future 抛异常。** `stop()` 先清空队列再唤醒线程，被清掉的 `packaged_task` 使其 future 以 `std::future_error(broken_promise)` 结束。**析构函数就是 `stop()`**，所以析构时排队未执行的任务会全部丢失——需要"跑完再退出"的场景不能用它。 |
| `[陷阱]` | **`stop()` 之后再 `enqueue` 会抛 `std::runtime_error`**（`"enqueue on stopped ThreadPool"`），不是返回无效 future。 |
| `[约束]` | **单元测试必须先注册 `"ThreadPool"` logger**，否则构造线程池的瞬间段错误。见 [logger.md](logger.md)。 |
| `[陷阱]` | 日志宏名前缀是 `THEADPOOL_LOG_*`（少一个 `R`），不是 `THREADPOOL_LOG_*`。 |
| `[事实]` | `Task::Destroy()` 从未被调用，实际析构由 `shared_ptr` 完成；`drop()` 只清队列、不停止线程池，之后仍可正常 `enqueue`；`threadNumber(id)` 对非 worker 线程一律返回 `-1`。 |
