# 详细设计文档

各模块的**实现级**说明：文件与符号清单、数据结构、SQL / 算法细节、调用时序、边界与错误处理。

> 本文档只回答"**具体是怎么实现的**"。要回答"**为什么这么设计**"（定位、依赖、分层、关键抽象、取舍）请看 [`../architecture/`](../architecture/README.md)；要回答"**怎么用**"（前置条件、示例、调用约定）请看 [`../usage/`](../usage/README.md)。

## 一、与架构文档的分工

两个目录**同模块一一对应**，同名文件（如 `sqlite.md`）是同一个模块的两级视角，读的时候应该成对看：

| | 架构设计 `../architecture/` | 详细设计（本文档目录） |
| --- | --- | --- |
| 回答的问题 | 是什么架构、为什么这么选 | 具体怎么实现、边界在哪里 |
| 视角 | 跨模块：定位、依赖方向、分层、关键抽象、设计取舍、架构级风险 | 模块内：文件职责、类型与接口清单、数据结构、算法规则、时序、错误矩阵 |
| 尺度 | 一张图 + 一张表能说清 | 需要逐类型 / 逐入口 / 逐分支说明 |
| 变化频率 | 低（架构稳定后基本不动） | 高（改一次实现就应同步一次） |
| 读者 | 新人建立心智模型、评审设计 | 改代码的人、排查问题的人、写测试的人 |

判断内容该放哪一级的经验法则：**"换一种实现方式，这句话还成立吗？"** 成立 → 架构文档；不成立（依赖具体函数、具体 SQL、具体常量）→ 详细设计。

## 二、模块索引

| 模块 | 架构设计 | 详细设计 | 状态 |
| --- | --- | --- | --- |
| Base | [base.md](../architecture/base.md) | [base.md](base.md) | ✅ 已完成 |
| Logger | [logger.md](../architecture/logger.md) | [logger.md](logger.md) | ✅ 已完成 |
| ThreadPool | [threadpool.md](../architecture/threadpool.md) | [threadpool.md](threadpool.md) | ✅ 已完成 |
| Zipper | [zipper.md](../architecture/zipper.md) | [zipper.md](zipper.md) | ✅ 已完成 |
| BaseQt | [baseqt.md](../architecture/baseqt.md) | [baseqt.md](baseqt.md) | ✅ 已完成 |
| SUI | [sui.md](../architecture/sui.md) | [sui.md](sui.md) | ✅ 已完成 |
| Sqlite | [sqlite.md](../architecture/sqlite.md) | [sqlite.md](sqlite.md) | ✅ 已完成（样板） |
| Aria2Net | [aria2net.md](../architecture/aria2net.md) | [aria2net.md](aria2net.md) | ✅ 已完成 |
| Download | [download.md](../architecture/download.md) | [download.md](download.md) | ✅ 已完成 |
| Plugin | [plugin.md](../architecture/plugin.md) | [plugin.md](plugin.md) | ✅ 已完成 |
| ClientUi | [clientui.md](../architecture/clientui.md) | [clientui.md](clientui.md) | ✅ 已完成（按分区概览） |

`sqlite.md` 是本目录的**样板**：各篇按它的章节顺序与粒度铺开，不再另行设计模板。

本目录说明当前实现。

两处口径差异：

- **ClientUi** 不逐控件展开——UI 层无法在单元测试里稳定断言，只写装配与生命周期、配置落盘、可测的纯逻辑，以及各 UI 分区的职责与入口。
- **Base / Logger / ThreadPool / BaseQt / SUI** 的架构篇此前是"架构与详细设计"混写，已按同一分工收敛：实现级内容（文件清单、类型清单、宏展开）迁到本目录，架构篇只留定位、分层、取舍与架构级风险。

## 三、写作模板（以 sqlite.md 为样板）

按下面固定的章节顺序写，缺的内容写"无"而不是省略，便于跨模块对比：

| 节 | 内容要求 | 反例（不要这样写） |
| --- | --- | --- |
| 一、文件清单 | 只写目录名 |
| 二～N、分层逐层说明 | 每层按"类型 → 成员/方法 → 行为规则 → 数据结构"展开；给出表格化的取值/映射/状态转换规则 | 把架构文档的段落抄一遍 |
| SQL / 算法细节 | 逐入口列出生成的 SQL 模板、绑定顺序、事务边界、失败返回值 | 只写"调用 SqliteUtil 完成插入" |
| 时序 | 关键路径用 mermaid `sequenceDiagram`，标出异常抛出点与加锁位置 | 泛泛的调用链 |
| 边界与错误处理矩阵 | "场景 → 当前行为 → 依据（函数 / 用例）"三列 | 只写"注意边界情况" |
| 测试映射 | 覆盖点 → 代表用例名，便于"改这里要跑哪些用例" | 只写"有单元测试" |
| 扩展步骤 + 手工验证命令 | 可直接照抄的命令与检查清单 | 省略 |

硬性要求：

- **每条非显然结论都要有出处**：文件 + 符号名，或测试用例名；不要写"看起来"、"可能"。
