# System Context

## 系统目标

sVanilla 是一个桌面端流媒体/文件下载工具，目标是通过插件体系解析不同网站链接，展示可下载内容，并通过下载器完成任务管理、状态追踪和本地持久化。

## 用户角色

| 角色 | 目标 |
| --- | --- |
| 普通用户 | 输入链接、查看解析结果、选择内容并下载 |
| 登录用户 | 保存站点账号/Cookie，下载需要登录权限的内容 |
| 插件开发者 | 基于插件接口扩展可解析网站 |
| 维护者 | 维护主程序、构建、插件集成和跨平台发布 |

## 系统边界

### 系统内

- 桌面 UI、主窗口、设置页、下载页和视频列表页。
- 插件发现、加载、配置和启用状态管理。
- URL 解析、列表展示、排序、搜索、历史记录。
- 下载任务创建、启动、暂停、恢复、停止、完成状态追踪。
- 本地配置和 SQLite 数据持久化。
- 本地 aria2 服务管理。
- 崩溃收集、日志、平台初始化、单实例控制。

### 系统外

- 远端网站和媒体资源。
- 插件实现的站点解析逻辑。
- aria2、ffmpeg、crashpad 等外部工具。
- Qt、vcpkg、SQLite、curl、OpenSSL、minizip 等第三方依赖。

## 外部依赖

| 依赖 | 用途 | 位置 |
| --- | --- | --- |
| Qt | UI、WebEngine、信号槽、翻译、资源 | `vcpkg.json`、CMake |
| aria2 | 下载执行和状态查询 | `tools/*/aria`、`Aria2Net` |
| ffmpeg | 媒体处理工具，构建后复制 | `tools/*/ffmpeg` |
| SQLite | 本地数据存储 | `Sqlite`、`ClientUi/Storage` |
| minizip | 压缩和解压 | `Zipper` |
| spdlog | 日志 | `Logger` |
| crashpad | 崩溃收集 | `ClientUi`、CMake |
| sVanillaPluginCommon | 插件接口和下载公共库 | `ThirdParty/sVanillaPluginCommon` |

## 约束

- Confirmed：项目要求 C++20。
- Confirmed：构建依赖 vcpkg toolchain。
- Confirmed：主程序支持 Windows、Linux、macOS。
- Confirmed：Qt5 和 Qt6 都有兼容分支，但 CI 主要覆盖 Qt 5.15.2 和 Qt 6.x。
- Assumption：插件 ABI/接口兼容性对发布版本很重要，修改插件接口前需要评审。
