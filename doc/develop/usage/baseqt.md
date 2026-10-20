# BaseQt 使用

跨线程投递、树结构、UI 工具函数。设计原理见 [`../architecture/baseqt.md`](../architecture/baseqt.md)。

## 一、投递任务到外部对象的线程

```cpp
ActivateQbject receiver;                     // 通常住在主线程
receiver.moveToThread(qApp->thread());

SenderForQt sender(&receiver);
sender.sendTask([] { /* 在 receiver 所在线程执行 */ });
```

## 二、投递任务到自带线程

```cpp
TransceiverForQt transceiver;                // 内部有一个事件循环线程
transceiver.sendTask([this] { doHeavyWork(); });
```

`Aria2Net` 就是用 `TransceiverForQt` 把 aria2 进程的启动 / 关闭投递到独立线程（见 [aria2net.md](aria2net.md)）。

## 三、树节点

```cpp
auto* root = new TreeItem;
root->addChild(new TreeItem(root));          // 或用 addChildren
for (const TreeItem* child : root->children()) { /* ... */ }
root->clearChild(true);                      // 递归删除整棵子树
```

## 四、遍历 `QTreeView`

```cpp
TreeViewIterator it(treeView);
while (it.hasNext())
{
    MyNode* node = *it;                      // 从 internalPointer() 转出
    // ...
    ++it;
}
```

## 五、工具函数

```cpp
util::showInFileExplorer(filePath);
util::animate(widget, {{"0.0", "1.0"}}, "opacity", {}, 300);
QPixmap icon = util::binToImage(svgBytes, QSize(24, 24));
```

## 六、调用约定与陷阱

| 标注 | 内容 |
| --- | --- |
| `[约束]` | **`SenderForQt` 的接收者不能传 `nullptr`。** `sendTask()` 在未就绪（`enable()` 为 false）时**静默返回**，任务不会被执行也不报错——传空接收者会得到"调用成功但什么都没发生"。 |
| `[约束]` | **不要移动 / 拷贝 `TreeItem`。** 它声明了默认移动语义但成员是裸指针，移动后两个对象共享子节点，析构会重复 `delete`。 |
| `[陷阱]` | **`util::readFileToVector` 打开失败会抛 `std::runtime_error("Cannot open file")`**，不是返回空 vector。 |
| `[陷阱]` | **`Utility.h` 缺少必要包含。** 头文件只 include 了 `<QString>` 与 `<QMenu>`，却使用 `QVariant`、`QPixmap`、`QSize`、`std::vector`、`std::function`、`std::uint8_t`，靠传递包含"碰巧能编过"。新增使用方若包含顺序不同可能编译失败——必要时自己补上对应头。 |
| `[陷阱]` | **`TreeViewIterator::getPreviousIndex` 查的是顶层行**（`index()` 调用漏传 parent），依赖它"深入子树取最后一项"的调用方会得到错误结果。 |
