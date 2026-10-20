# Qt5/Qt6 重载信号 connect 歧义（macOS CI 编译失败）

## 现象

macOS runner（Qt 5.15.2）编译 `sVanilla/src/ClientUi/Setting/AdvanceSettings.cpp:96` 报：

```text
error: no matching member function for call to 'connect'
    connect(ui->spinBoxAria2Port, &QSpinBox::valueChanged, this, [this](int value) { ... });
note: candidate template ignored: couldn't infer template argument 'Func1'
```

Windows 本地（Qt 6.6.2）不报。

## 根因

- `CMakeLists.txt:71-72` 同时接受 Qt6/Qt5（`find_package(QT NAMES Qt6 Qt5 ...)`），但两个版本的信号重载集合不同。
- Qt5 的 `QSpinBox` 有 `valueChanged(int)` 与 `valueChanged(const QString&)` 两个重载；Qt6 删掉了 QString 版本。
- 新语法（PMF）connect 走模板推导，取重载函数地址无法唯一确定 `Func1`，于是回退到 `const char*` / `QMetaMethod` 候选并全部失败。
- 同源隐患类：`QComboBox::currentIndexChanged`、`QButtonGroup::buttonClicked`、`QProcess::finished`、`QDoubleSpinBox::valueChanged`。

## 处置

沿用仓库既有消歧写法（`static_cast`），与 `AriaServer.cpp:103` 的 `QProcess::finished`、`AdvanceSettings.cpp:66` / `SetCookieDialog.cpp:73` 的 `QComboBox::currentIndexChanged` 保持一致：

```cpp
connect(ui->spinBoxAria2Port, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this](int value) { ... });
```

`QOverload<int>::of(&QSpinBox::valueChanged)` 语义等价且是 Qt 官方文档推荐写法，但仓库目前统一用 `static_cast`，不要混用。

## 坑点与经验

1. **跨平台 CI 的 Qt 版本差异会掩盖问题**：改 UI 代码后，Windows 本地能过不代表 macOS（Qt5）能过。涉及信号 PMF connect 时先确认 Qt5 下该信号是否重载。
2. **本机无法验证 Qt5**：只装了 Qt 6.6.2 msvc2019_64，且缺 Windows 10 SDK（只有 8.1 + NETFXSDK），`cl.exe` 连 `stddef.h` 都找不到，单 TU 编译验证走不通。这类场景应如实说明"未实机验证"，改以官方文档/头文件事实作为证据。
3. **全仓库同类排查优先**：改之前先 grep `&Q*SpinBox::valueChanged`、`&QComboBox::currentIndexChanged` 等，确认是否只有一处，避免只修一点。本次全仓库仅此一处命中。
