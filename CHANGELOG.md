## v2.2.2 (2026-06-28)

### 新增 | New

* 插件：新增抖音插件，支持解析抖音视频、图文、合集、系列、搜索/发现页和直播间等链接，并支持下载相关视频内容。
* 插件：新增小红书插件，支持解析小红书笔记、探索页和用户主页链接；带 `xsec_token` 的链接可解析笔记或用户作品列表。
* 插件：扩展第三方插件集合，新增抖音和小红书插件后，内置插件覆盖面进一步增加。
* 构建：新增 Harness/SDD 工作流、需求文档、规格文档和 AI 协作规范，便于后续更稳定地分析需求、生成规格、验收改动和整理发布说明。

### 改进 | Improved

* 界面：插件改为后台加载，应用启动时主界面响应更快；插件尚未加载完成时，URL 解析、网站图标识别和登录入口会提示稍后重试。
* 下载：增强下载任务异常隔离，单个下载器异常会被标记为错误并移除，降低下载线程卡死或影响其他任务的风险。
* 下载：本地 Aria2 RPC 默认仅监听本机并关闭跨源开放，减少运行时暴露面。
* 插件：提升插件异常隔离能力，单个插件接口异常不再容易影响主程序或其他插件。
* 插件：公共插件库新增加密、编码、二维码、JSON 转换和下载状态转换等通用能力，减少插件重复实现并提升插件维护性。
* 日志：补充 Qt、Qt WebEngine、下载线程、线程池和登录监控线程日志，问题定位信息更完整。

### 修复 | Fix

* 插件：修复插件管理页启用/禁用状态保存错误，避免勾选状态被反向写入或修改后没有持久化。
* 插件：修复插件配置加载失败时缺少日志的问题，现在会记录配置路径和失败原因。
* 插件：修复异步加载插件后账号页不会自动刷新已登录插件的问题。
* 账号/Cookie：修复登录监控在成功、失败或超时后未正确清理登录代理的问题。
* 命令行：修复 `--help`、`--version` 等只打印命令仍继续初始化完整应用的问题；命令行下载会等待插件加载完成后再解析 URL。
* 界面：修复导入插件和 Linux 开机自启动设置中的命令拼接风险，改为参数化调用。
* 下载：增强 ZIP 解压安全性，拒绝目录穿越、反斜杠穿越、Windows 绝对路径、符号链接和异常压缩比条目。
* 下载：修复 ZIP64 元数据文件解压/压缩支持，并补充输出目录自动创建和失败清理逻辑。

### 构建与测试 | Build & Test

* 构建：更新 `ThirdParty/sVanillaPluginCommon` 子模块，从 `0ff665e` 更新到 `3022fc7`，包含 `PluginCrypto`、JSON 转换、Aria2/Network 修复和 FFmpeg target 依赖更新。
* 构建：更新 `ThirdParty/sVanillaPlugins` 子模块，从 `0467ea7` 更新到 `1cfec67`，包含抖音/小红书插件、插件测试布局标准化、共享 common helper 复用和 `TemplatePlugin` 公共入口调整。
* 测试：新增 `Audit_test`，覆盖下载器异常隔离和插件代理异常隔离。
* 测试：重写 Zipper 测试，覆盖 ZIP64、输出目录创建、目录穿越、反斜杠穿越和 Windows 绝对路径拒绝。
* 测试：插件集合将多个插件测试拆分为 `test/plugin` 和 `test/unit`，并统一静态测试 target 链接方式。

### 文档 | Docs

* 文档：新增插件开发文档、插件 API 契约、构建测试指南和支持 URL 参考。
* 文档：新增用户手册、项目说明、SQLite 架构说明和开发入门文档。
* 文档：新增 Harness/SDD 文档，包括系统上下文、功能需求、架构、数据存储、接口契约、运行流程、构建测试质量和智能体工作规则。
* 文档：新增 changelog 技能和 Codex 工作流命令，用于后续发布说明、需求分析、规格设计和验收记录。

<details>
<summary><b>English</b></summary>

## v2.2.2 (2026-06-28)

### New

* Plugins: added the DouYin plugin, with URL parsing for videos, notes, collections, series, search/discovery pages, and live room links, plus video download support.
* Plugins: added the Xiaohongshu plugin, with URL parsing for notes, explore pages, and user profile links. Links with `xsec_token` can resolve notes or user post lists.
* Plugins: expanded the bundled third-party plugin set with DouYin and Xiaohongshu support.
* Build: added the Harness/SDD workflow, requirements, specs, and AI collaboration rules for more consistent requirement analysis, specification design, acceptance, and changelog generation.

### Improved

* UI: plugins now load in the background, improving startup responsiveness. URL parsing, website icon detection, and login actions now ask users to retry when plugin loading is still in progress.
* Download: improved isolation for download task failures. A failing downloader is marked as `Error` and removed, reducing the chance that one task blocks the download thread or affects other tasks.
* Download: local Aria2 RPC now listens only on the local machine by default and disables open cross-origin access.
* Plugins: improved plugin exception isolation so a single failing plugin call is less likely to affect the main application or other plugins.
* Plugins: added shared crypto, encoding, QR code, JSON conversion, and download status conversion helpers to the common plugin library.
* Logging: added Qt, Qt WebEngine, download thread, thread pool, and login monitor lifecycle logs for better diagnostics.

### Fix

* Plugins: fixed plugin manager enable/disable persistence so checkbox state is no longer saved inversely or left unsaved.
* Plugins: fixed missing logs when plugin configuration loading fails. The path and failure reason are now recorded.
* Plugins: fixed the account page not refreshing logged-in plugins after asynchronous plugin loading completes.
* Account/Cookie: fixed login monitor cleanup after success, failure, or timeout.
* CLI: fixed print-only commands such as `--help` and `--version` continuing into full application initialization. CLI downloads now wait for plugins to finish loading before parsing URLs.
* UI: replaced shell command string concatenation in plugin import and Linux auto-start setup with parameterized process calls.
* Download: hardened ZIP extraction against directory traversal, backslash traversal, Windows absolute paths, symlinks, and suspicious compression ratios.
* Download: fixed ZIP64 metadata archive support and added output directory creation plus failed extraction cleanup.

### Build & Test

* Build: updated `ThirdParty/sVanillaPluginCommon` from `0ff665e` to `3022fc7`, including `PluginCrypto`, JSON conversions, Aria2/Network fixes, and FFmpeg target dependency updates.
* Build: updated `ThirdParty/sVanillaPlugins` from `0467ea7` to `1cfec67`, including the DouYin/Xiaohongshu plugins, standardized plugin test layout, shared common helper usage, and the `TemplatePlugin` common entry point.
* Test: added `Audit_test` for downloader exception isolation and plugin proxy exception isolation.
* Test: rewrote Zipper tests to cover ZIP64, output directory creation, directory traversal, backslash traversal, and Windows absolute path rejection.
* Test: split plugin collection tests into `test/plugin` and `test/unit`, and standardized static test target linking.

### Docs

* Docs: added plugin development documentation, plugin API contracts, build/test guides, and supported URL references.
* Docs: added the user manual, project overview, SQLite architecture documentation, and developer getting-started guide.
* Docs: added Harness/SDD documents covering system context, functional requirements, architecture, data storage, interface contracts, runtime flows, build/test quality, and agent working rules.
* Docs: added the changelog skill and Codex workflow commands for future release notes, requirement analysis, specification design, and acceptance records.

</details>
