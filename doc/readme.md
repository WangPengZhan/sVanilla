# sVanilla 文档中心

本文档是 `doc` 目录的导航入口，用于说明各文档的用途、适用读者和推荐阅读顺序。项目概览、功能介绍和截图展示请优先查看根目录的 [README](../README.md)。

## 文档索引

| 文档 | 适用读者 | 内容说明 |
| --- | --- | --- |
| [用户操作手册](manual.md) | 普通用户、测试人员、产品体验人员 | 介绍软件功能、主界面、链接解析、视频列表、下载管理、账号登录、插件管理、常用设置和常见问题。 |
| [开发快速开始](develop/started.md) | 首次参与开发的贡献者 | 介绍环境要求、源码获取、CMake 配置、构建、测试、Python 依赖和常见构建问题。 |
| [工程结构分析](develop/project.md) | 开发者、维护者、代码审查人员 | 说明项目定位、顶层目录、源码模块、资源与国际化、插件体系、构建测试入口和主要风险点。 |
| [SQLite 存储设计](develop/sqlite_architecture.md) | 负责持久化、下载记录、历史记录、Cookie 或账号数据的开发者 | 说明 SQLite 封装架构、数据流、条件组装、业务表、Storage 使用方式和扩展新表建议。 |

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
2. UI、下载、插件、存储等改动可参考 [工程结构分析 - 源码模块](develop/project.md#六源码模块) 定位代码，并按 [Harness Engineering + SDD](../harness/README.md) 获取共享工程规范和模块知识。
3. 涉及本地数据、下载记录、历史记录、Cookie 或账号信息时，继续阅读 [SQLite 存储设计](develop/sqlite_architecture.md)。
4. 完成修改后，按 [开发快速开始 - 运行测试](develop/started.md#运行测试) 选择最小相关验证。

## 文档维护约定

- 用户可见行为发生变化时，同步更新 [用户操作手册](manual.md)。
- 构建、依赖、测试命令或环境要求变化时，同步更新 [开发快速开始](develop/started.md)。
- 模块职责、目录结构、插件体系或构建入口变化时，同步更新 [工程结构分析](develop/project.md)。
- 数据表、持久化字段、Storage 接口、查询方式或迁移策略变化时，同步更新 [SQLite 存储设计](develop/sqlite_architecture.md)。
- 新增文档后，应在本文档的“文档索引”和相关“推荐阅读路径”中补充跳转。

## 相关入口

- 项目主页：[README](../README.md)
- 用户文档：[doc/manual.md](manual.md)
- 开发文档目录：[doc/develop](develop/)
- 共享工程规范与 SDD：[harness/README.md](../harness/README.md)
- 插件仓库文档：[ThirdParty/sVanillaPlugins/README.md](../ThirdParty/sVanillaPlugins/README.md)
- 插件公共库文档：[ThirdParty/sVanillaPluginCommon/README.md](../ThirdParty/sVanillaPluginCommon/README.md)
