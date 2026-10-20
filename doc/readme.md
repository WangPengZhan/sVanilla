# sVanilla 文档中心

本文档是 `doc` 目录的导航入口，用于说明各文档的用途、适用读者和推荐阅读顺序。项目概览、功能介绍和截图展示请优先查看根目录的 [README](../README.md)。

## 文档索引

| 文档 | 适用读者 | 内容说明 |
| --- | --- | --- |
| [用户操作手册](manual.md) | 普通用户、测试人员、产品体验人员 | 介绍软件功能、主界面、链接解析、视频列表、下载管理、账号登录、插件管理、常用设置和常见问题。 |
| [开发快速开始](develop/started.md) | 首次参与开发的贡献者 | 介绍环境要求、源码获取、CMake 配置、构建、测试、Python 依赖和常见构建问题。 |
| [工程结构分析](develop/project.md) | 开发者、维护者、代码审查人员 | 说明项目定位、顶层目录、源码模块、资源与国际化、插件体系、构建测试入口和主要风险点。 |
| [架构文档](develop/architecture/README.md) | 参与具体模块开发的贡献者 | 各模块的定位、依赖、分层、关键抽象与设计取舍，以及架构级风险（`R1`…）。 |
| [详细设计文档](develop/detailed-design/README.md) | 参与具体模块开发的贡献者 | 各模块的实现级说明：类型清单、数据结构、算法与 SQL 细节、边界与错误处理。 |
| [使用文档](develop/usage/README.md) | 参与具体模块开发的贡献者 | 各模块的前置条件、示例代码与调用约定，标注 `[约束]` / `[陷阱]`，并列出必须先读的跨模块约定。 |

## 推荐阅读路径

### 使用 sVanilla

1. 先阅读根目录 [README](../README.md)，了解项目定位、主要功能、截图和免责声明。
2. 再阅读 [用户操作手册](manual.md)，按流程了解链接解析、下载管理、账号登录和插件管理。
3. 如果遇到问题，优先查看 [用户操作手册 - 常见问题](manual.md#11-常见问题)。

### 从源码构建

1. 阅读 [开发快速开始 - 环境要求](develop/started.md#环境要求)，确认 Git、CMake、Python、C++20 编译器和 Qt 环境。
2. 按 [开发快速开始 - 获取源码](develop/started.md#获取源码) 初始化仓库和子模块。
3. 按 [开发快速开始 - 配置工程](develop/started.md#配置工程)、[构建](develop/started.md#构建) 和 [运行测试](develop/started.md#运行测试) 完成最小验证。
4. 如果 CMake、Qt、vcpkg 或 Python 依赖失败，查看 [开发快速开始 - 常见问题](develop/started.md#常见问题)。

### 参与功能开发

1. 先阅读 [工程结构分析](develop/project.md)，确认模块边界、构建入口和测试入口。
2. UI、下载、插件、存储等改动可参考 [工程结构分析 - 源码模块](develop/project.md#六源码模块) 定位代码；进入具体模块前先读 [架构文档](develop/architecture/README.md) 了解设计方案，再读 [详细设计文档](develop/detailed-design/README.md) 对应篇看实现细节，然后读 [使用文档](develop/usage/README.md) 避开调用陷阱，并按 [Harness Engineering + SDD](../harness/README.md) 获取共享工程规范和模块知识。
3. 涉及本地数据、下载记录、历史记录、Cookie 或账号信息时，继续阅读 [SQLite 架构设计](develop/architecture/sqlite.md) 与 [SQLite 详细设计](develop/detailed-design/sqlite.md)。
4. 完成修改后，按 [开发快速开始 - 运行测试](develop/started.md#运行测试) 选择最小相关验证。

## 文档维护约定

- 用户可见行为发生变化时，同步更新 [用户操作手册](manual.md)。
- 构建、依赖、测试命令或环境要求变化时，同步更新 [开发快速开始](develop/started.md)。
- 模块职责、目录结构、插件体系或构建入口变化时，同步更新 [工程结构分析](develop/project.md)。
- 单个模块的设计、接口、依赖变化时，同步更新 [架构文档](develop/architecture/README.md) 与 [使用文档](develop/usage/README.md) 下对应模块的两篇文档（架构篇讲"怎么设计的"，使用篇讲"怎么用"）；涉及具体实现、SQL、算法时，同时更新 [详细设计文档](develop/detailed-design/README.md) 对应篇。
- 数据表、持久化字段、Storage 接口、查询方式或迁移策略变化时，同步更新 [SQLite 架构设计](develop/architecture/sqlite.md) 与 [SQLite 详细设计](develop/detailed-design/sqlite.md)。
- 新增文档后，应在本文档的“文档索引”和相关“推荐阅读路径”中补充跳转。

## 相关入口

- 项目主页：[README](../README.md)
- 用户文档：[doc/manual.md](manual.md)
- 开发文档目录：[doc/develop](develop/)
- 共享工程规范与 SDD：[harness/README.md](../harness/README.md)
- 插件仓库文档：[ThirdParty/sVanillaPlugins/README.md](../ThirdParty/sVanillaPlugins/README.md)
- 插件公共库文档：[ThirdParty/sVanillaPluginCommon/README.md](../ThirdParty/sVanillaPluginCommon/README.md)
