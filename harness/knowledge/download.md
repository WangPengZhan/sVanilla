# Download Knowledge

## 模块范围

- 下载 UI。
- `sVanilla/src/Download`。
- Aria2 交互。

## 已知规则

- 下载任务状态变更需要考虑 UI、队列、持久化和实际下载器状态的一致性。
- 不要改变下载唯一 ID，除非有迁移方案。

## 证据

- 需要结合实际任务读取 `sVanilla/src/Download` 和相关 UI。
