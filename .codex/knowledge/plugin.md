# Plugin Knowledge

## 模块范围

- `sVanilla/src/Plugin`
- `ThirdParty/sVanillaPluginCommon`
- `ThirdParty/sVanillaPlugins`

## 已知规则

- 插件 ID、插件目录结构、插件接口属于兼容性敏感点。
- 修改插件启用状态时，需要区分 UI 状态、持久化状态、运行时可用状态。
- 禁用插件的默认安全语义应优先影响后续使用，不应默认中断已经运行的任务，除非需求明确要求。
- 如果启用或禁用失败，UI 应回滚到真实状态。

## 需要确认的问题

- 当前插件启用状态存放在配置、数据库还是内存结构中。
- 当前插件管理页是否已经有刷新信号或模型层。

## 证据

- 需要结合实际任务读取插件管理 UI 和 `sVanilla/src/Plugin`。
