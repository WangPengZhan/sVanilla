# SUI 使用

用现成控件、搭内嵌浏览器并与 JS 互通。设计原理见 [`../architecture/sui.md`](../architecture/sui.md)。

## 一、使用普通控件

都是标准 Qt 控件的子类，可直接构造、连信号：

```cpp
auto* edit = new SearchLineEdit(parent);
connect(edit, &QLineEdit::textChanged, this, [](const QString& text) { /* ... */ });
```

## 二、搭建带桥的内嵌浏览器

```cpp
WebFrame* frame = new WebFrame(parent);
frame->load(QUrl("https://example.com"));

// 暴露一个既有的 QObject 给 JS（用它的 objectName 作为注册名）
myBridgeObject->setObjectName("myBridge");
frame->setWebChannelBridge(myBridgeObject);
```

## 三、初始化 WebEngine

进程内尽早调用一次：

```cpp
initializeWebEngine(cachePath, storagePath);
```

| 分组 | 可用类型 |
| --- | --- |
| 输入控件 | `AddLinkLineEdit`、`SearchLineEdit`、`FileSelector`、`NameRuleEditWidget` |
| 提示与弹窗 | `TipDialog`、`Toast`、`ToastTip` |
| 表格 | `FreezeTreeView`（注意：定义在 `FreezeTableView.h`） |
| 容器与布局 | `CollapsePanel`、`EditableLabel` |
| 展示控件 | `DownloadCount`、`RoundImageWidget`、`DividingLine`、`SlidingWaitBar` |
| 命名规则 | `NameRuleWidget`、`NameRuleSelectWidget` |
| WebEngine | `WebBridge`、`WebPage`、`WebView`、`WebFrame`、`WebSetting` |

## 四、调用约定与陷阱

| 标注 | 内容 |
| --- | --- |
| `[约束]` | **`Toast::showTip()` 必须先 `Toast::create()`。** 未创建时调用会**静默返回**（`ToastTip::showTip()` 同样判空），不崩溃也不提示——漏掉 `create()` 的症状是"提示不出现"。 |
| `[约束]` | **`WebFrame::setWebChannelBridge` 用 `bridge->objectName()` 作为注册名**。`WebBridge` 构造时默认名为 `"WebBridge"`；显式改名时 JS 侧必须同步，否则拿不到对象。 |
| `[陷阱]` | **`WebBridge::invokeJSCallback` 不转义参数**——用单引号拼 JS 字符串，回调参数含单引号或换行就会破坏脚本，调用方需自行转义。 |
| `[陷阱]` | **`WebBridge` 的方法名与参数没有编译期校验**（反射分发），写错只在运行时暴露；参数只能走字符串，需自行序列化。 |
| `[陷阱]` | **Debug 构建固定开启远程调试**：`WebSetting.cpp` 有 `#ifndef NDEBUG qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9876")`，即所有 Debug 版本都在 9876 端口暴露 WebEngine 调试接口。 |
| `[陷阱]` | **`FileSelector.cpp` 在 .cpp 内定义 `Q_OBJECT` 类并手动 `#include "FileSelector.moc"`**，依赖 CMake 的 `AUTOMOC`；把该文件加入不开启 AUTOMOC 的目标会链接失败。 |
| `[陷阱]` | **`EditableLabel` 经 `(const QString&, bool, QWidget*)` 构造时 `getEditedStatus()` 返回不确定值**（`isEdited` 未初始化）；且每次编辑会丢掉 prefix。 |
| `[陷阱]` | **`CollapsePanel` 在构造末尾就触发一次 `clicked(true)`**——挂在它上面的 lambda 会在构造期被执行，注意捕获对象的初始化顺序。 |
| `[陷阱]` | **`RoundImageWidget::resizePixmap()` 在 `height() == 0` 时除零**。 |
| `[事实]` | **文件名与类名不一致**：`FreezeTableView.h/.cpp` 里定义的类名是 `FreezeTreeView`，按文件名搜类型会找不到。 |
| `[事实]` | `initializeWebEngine(cache_path, storage_path)` 的**两个参数实际未被使用**；`NameRuleWidget.h` 使用 `std::unordered_map` / `vector` 等却未包含对应头文件，依赖传递包含。本模块没有 `.qrc`，`.ui` 中引用的 `:/icon/...` 由使用方（主程序）的资源提供。 |
