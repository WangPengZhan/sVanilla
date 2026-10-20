# 2026-10-15 — vcpkg 不可用时的单文件编译与迷你测试跑法

## 现象

`cmake --build out/harness` 触发 CMake 重新配置 → 重新执行 vcpkg manifest install →
`out/harness/vcpkg_installed/x64-windows` 的 `lib/` 与 `include/` 被清空；随后 zlib 在配置阶段
因系统策略禁止 `reg.exe` 失败，`MSBuild.exe` 也被 LOLBin 策略拦截。二进制缓存 `Restored 0 package(s)`，
重建 21 个依赖包不现实。

**结论：不要再通过 `cmake --build` 触发重新配置。** 仓库根目录的 `build/`（用户自己的 VS 构建目录）
不受影响，里面有完整的 `vcpkg_installed/x64-windows/{include,lib,bin}`。

## 可行替代：直接用 cl.exe 编译单个 TU

`cl.exe`、`uic.exe`、`moc.exe` 本身没有被拦截，可以直接调用：

```sh
export MSYS_NO_PATHCONV=1   # 关键：否则 /nologo 等参数会被 Git Bash 当成路径
cl.exe /nologo /utf-8 /std:c++20 /EHsc /MT /permissive- /Zc:__cplusplus \
  /DUNICODE /D_UNICODE /DWIN32 /D_WIN32 /DNOMINMAX \
  /I<MSVC>/include /I<build>/vcpkg_installed/x64-windows/include [/I.../minizip] \
  /I<SDK>/Include/10.0.26100.0/{ucrt,um,shared} \
  /I<Qt>/include [/I<Qt>/include/QtCore ...] \
  /I sVanilla/src [/I ThirdParty/spdlog/include] [/I ThirdParty/sVanillaPluginCommon{/PluginCommon}] \
  /Fe out/xxx.exe <sources> /link /LIBPATH:... gtest.lib minizip.lib zlib.lib Qt6Core.lib ...
```

坑位：

| 坑 | 现象 | 解法 |
| --- | --- | --- |
| Git Bash 路径转换 | `/nologo: No such file`、`unrecognized source file type 'Files/Microsoft'` | `export MSYS_NO_PATHCONV=1`，且不要把一个含空格的 flags 字符串当单个参数传（用 bash 数组） |
| 缺 `vcruntime.h` | `corecrt.h: cannot open include file` | 显式 `/I <MSVC>/include`（cl 不会自动带自己的 STL 头路径） |
| `LoadLibraryA cannot convert LPCWSTR` | 探测环境没开 UNICODE | 加 `/DUNICODE /D_UNICODE`（项目本身是 Unicode 构建） |
| `Q_OBJECT` 符号未定义 | LNK2019 `staticMetaObject` / `qt_metacast` | 先跑 `moc.exe -I sVanilla/src Xxx.h -o out/moc_Xxx.cpp` 再一起编译 |
| `ui_Xxx.h` 不存在 | C1083 | 先跑 `uic.exe Xxx.ui -o out/uic/ui_Xxx.h` |
| `main already defined` | LNK2005 | 部分测试目录自带 `main()`，此时不要再附加自建的 `gtest_probe_main.cpp` |
| **/MD 的 exe 跑不起来** | `error while loading shared libraries: api-ms-win-crt-math-l1-1-0.dll` | 本机 `System32` 下没有 `api-ms-win-crt-*.dll`；改用 **`/MT`** 静态链接 CRT |
| /MT 下仍缺 DLL | `minizip.dll not found` | 把 `<build>/vcpkg_installed/x64-windows/bin/*.dll` 复制到 exe 同目录，**并在 PowerShell 里运行**（Git Bash 的加载器找不到 exe 同目录的 DLL） |

## 已验证可跑通的迷你套件

| 套件 | 需要的 lib |
| --- | --- |
| `test/Zipper` | `gtest.lib minizip.lib zlib.lib`（无 Qt） |
| `test/SQLite` | `gtest.lib sqlite3.lib`（无 Qt；spdlog 走 ThirdParty 子模块头文件） |
| `test/ClientUi` | `gtest.lib Qt6Core.lib` |
| `test/BaseQt` | `gtest.lib Qt6Core/Gui/Widgets/Svg.lib` + `moc_SenderForQt.cpp`，环境变量 `QT_QPA_PLATFORM=offscreen` |
| `test/ThreadPool` | `gtest.lib Qt6Core/Gui/Widgets/Network.lib` + `moc_Task.cpp`（自带 `main()`） |

`test/Plugin` 依赖子模块构建产物 `PluginCommon` / `PluginDownload`，本机没有现成 lib，只能做编译校验。

## 附带经验：改测试前先看用例注释

`test/Zipper` 有注释写明"记录既有行为（最小化改动前后一致）"——这类用例是把**缺陷行为**锁住的回归网。
修掉缺陷时要么同步改用例、要么放弃修改，不能只改代码让用例变红。本轮修 D2（源目录缺失改为返回 false）
时同步把 `ThrowsForMissingSourceDirectory` 改名为 `ReturnsFalseForMissingSourceDirectory`，并在文档里标明这是契约变更。
