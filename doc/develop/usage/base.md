# Base 使用

用到「同一语义、不同参数个数」的宏分发时才需要它。设计原理见 [`../architecture/base.md`](../architecture/base.md)。

## 一、引用方式

```cpp
#include "Base/MacroTool.h"   // 使用方需把 sVanilla/src 加入 include 路径
```

## 二、按参数个数分发到不同实现

```c
#define LogLimitTime(...) EXPAND(CAT(LogLimitTime_, MACRO_COUNT(__VA_ARGS__))(__VA_ARGS__))
// LogLimitTime(LogI)          -> LogLimitTime_1(LogI)
// LogLimitTime(LogI, 5000000) -> LogLimitTime_2(LogI, 5000000)
```

新增一个分发宏：① 写好各参数个数的实现宏，命名为 `<前缀>_1`、`<前缀>_2` …；② 用 `CAT(前缀, MACRO_COUNT(__VA_ARGS__))` 拼出目标宏名；③ 外层套 `EXPAND(...)` 确保 `__VA_ARGS__` 已展开。

## 三、调用约定与陷阱

| 标注 | 内容 |
| --- | --- |
| `[约束]` | 本目录**没有 `CMakeLists.txt`**，不产生构建目标。引用方须自行确保 `sVanilla/src` 在 include 路径上（直接用 `Logger` 等目标编译会找不到该头）。 |
| `[约束]` | `MACRO_COUNT` 最多支持 9 个参数。超出不会显式报错，而是产生难以理解的宏展开错误；需要更多分支时须先扩展 `HELPER_OF_MACRO_COUNT` 的占位数量。 |
| `[陷阱]` | 不要「简化」掉 `STR` / `CAT` 的两级展开。单层实现在参数本身是宏时会得到宏名而非展开值。 |
