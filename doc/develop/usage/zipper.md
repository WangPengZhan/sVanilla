# Zipper 使用

打包目录、解压外部归档。安全校验的设计见 [`../architecture/zipper.md`](../architecture/zipper.md)。

## 一、打包一个目录

```cpp
Zipper zipper(sourceDir, outputZipPath);   // 目录模式
if (!zipper.zip())
{
    // 失败
}
```

`zip()` 会把 `sourceDir` 递归打包，条目名是相对 `sourceDir` 的路径。

## 二、解压

```cpp
Unzipper unzipper(zipPath, outputDir);
if (!unzipper.unzip())
{
    // 失败（可能是路径非法、压缩比异常、或写盘失败）
}
```

## 三、自行写入多条目归档

```cpp
zipFile z = zipOpen64(path.c_str(), APPEND_STATUS_CREATE);
ResourceHelper guard([z] { zipClose(z, nullptr); });

Zipper::addFileToZip(z, "dir/file.txt", "/abs/path/file.txt");   // 普通文件
Zipper::addFileToZip(z, "emptydir", "");                          // 空目录（自动补 "/"）
Zipper::collectFileInDirToZip(z, "/abs/some/dir", "");            // 递归整个目录
```

## 四、调用约定与陷阱

| 标注 | 内容 |
| --- | --- |
| `[事实]` | `zip()` 对输出文件名为空、`zipOpen64` 失败和源目录不存在返回 `false`，文件列表或目录条目的写入失败也会累计并返回 `false`。目录遍历只在迭代器构造时传入 `error_code`；后续 `operator++`、`absolute()`、`relative()` 仍可能抛 `std::filesystem::filesystem_error`，调用方需要捕获。 |
| `[事实]` | `zip()` 在 `m_vectZipFiles` 非空时逐个打包指定文件，归档内名称取文件名，并累计每次 `addFileToZip()` 的返回值；列表为空时按 `m_strZipPath` 递归打包目录。 |
| `[陷阱]` | **`setZipFiles()` 会清空另外两个成员**（`m_strZipPath.clear(); m_strRelativePath.clear();`）。目录模式与文件列表模式共用同一个 `Zipper` 对象时会互相清状态。 |
| `[约束]` | `Unzipper` 的输出路径必须是**受信任的**本地目录：解压会拒绝任何越出该目录的条目，条目名含 `..`、绝对路径、盘符前缀都会让**整个解压失败**（不是跳过单条）。 |
| `[约束]` | 自行调用静态 `addFileToZip()` 写多个条目时，用 `ResourceHelper` 保证 `zipClose` 被调用，并逐项检查返回值；`zip()` 的文件列表模式会累计返回值。 |
| `[事实]` | 当前主仓库内 `Zipper` / `Unzipper` 没有任何生产调用方，唯一调用者是单元测试。 |
