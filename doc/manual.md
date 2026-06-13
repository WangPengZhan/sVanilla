# sVanilla 用户操作手册

## 1. 软件简介

sVanilla 是一款跨平台桌面下载工具，用于下载视频、音频、音乐等流媒体内容。软件通过插件识别不同网站链接，通过内置或远程 aria2 执行下载，并提供登录、下载队列、历史记录、主题、插件管理等功能。

适用平台：

- Windows
- Linux
- macOS

主要功能：

- 输入一个链接解析单个视频、播放列表或频道内容。
- 以网格或列表方式查看解析结果。
- 支持单个下载、选中下载、全部下载。
- 管理下载中任务和已完成任务。
- 支持站点账号登录或 Cookie 登录。
- 支持插件导入、启用和停用。
- 支持浅色、深色、跟随系统主题。
- 支持系统托盘、开机启动、窗口状态保存、未完成任务自动恢复。

## 2. 界面截图导览

以下截图来自项目 README 中提供的软件界面示例。由于不同平台、主题、插件和版本可能略有差异，实际界面以当前安装版本为准。

![sVanilla 软件主界面示例](https://github.com/user-attachments/assets/7ed8aba0-0457-4ad4-9460-72d2508156f4)

图 1：软件主界面示例。顶部导航用于在 Home、Video、Download、Settings 等页面之间切换，主区域根据当前页面显示链接输入、解析结果、下载任务或设置项。

![sVanilla 功能页面示例](https://github.com/user-attachments/assets/ed61b979-0f11-4a4b-b360-e0f17ce7a3d7)

图 2：功能页面示例。软件会根据当前操作展示不同内容，例如解析结果、任务列表、详情面板或配置区域。

![sVanilla 列表与详情区域示例](https://github.com/user-attachments/assets/36b0727e-6da4-4468-9a9d-0e0784a34c39)

图 3：列表与详情区域示例。解析结果和下载任务都采用列表或卡片形式展示，右侧详情面板可用于查看标题、发布者、文件路径、下载状态等信息。

![sVanilla 下载与设置页面示例](https://github.com/user-attachments/assets/4cb98dad-08c6-490c-b7ce-9cd59e39d4e2)

图 4：下载与设置页面示例。Download 页面用于控制下载中和已完成任务，Settings 页面用于修改下载目录、主题、语言、插件和 aria2 相关配置。

![sVanilla 其他界面示例](https://github.com/user-attachments/assets/a06b401e-ba35-46e5-9be3-62e02616a773)

图 5：其他界面示例。不同插件、登录状态和主题会影响界面中展示的网站图标、账号信息、列表内容和状态提示。

## 3. 启动与主界面

启动 sVanilla 后，窗口顶部是主导航栏，包含四个主要页面：

| 页面 | 用途 |
| --- | --- |
| Home | 输入链接、从剪贴板解析、查看搜索历史、导入插件、登录下载站点 |
| Video | 查看链接解析结果，搜索、排序、切换视图并创建下载任务 |
| Download | 管理下载中任务和已完成任务 |
| Settings | 配置下载、外观、系统托盘、账号、aria2、插件和软件信息 |

快捷键：

| 快捷键 | 功能 |
| --- | --- |
| Ctrl + 1 | 切换到 Home 页面 |
| Ctrl + 2 | 切换到 Video 页面 |
| Ctrl + 3 | 切换到 Download 页面 |
| Ctrl + 4 | 切换到 Settings 页面 |

如果启用了系统托盘，点击托盘图标可以在显示和隐藏主窗口之间切换。托盘右键菜单提供 Restore、Minimize、Quit 等操作。

参考图 1 可以快速了解主窗口结构：顶部是页面导航，中央区域是当前页面的主要操作区。

## 4. 解析链接

### 4.1 从首页解析

图片示例：

<img src="https://github.com/user-attachments/assets/7ed8aba0-0457-4ad4-9460-72d2508156f4" alt="Home 页面链接输入示例" width="720">

1. 打开 Home 页面。
2. 在中间的链接输入框中粘贴或输入视频、音频、播放列表、频道等链接。
3. 输入框左侧会根据链接自动识别可用插件，并显示对应站点图标；未识别时显示默认图标。
4. 点击输入框右侧的进入按钮，或在输入框内按回车，开始解析。
5. 软件会自动切换到 Video 页面并显示解析结果。

参考图 1 中的主操作区域，Home 页面主要围绕链接输入框、剪贴板按钮、历史按钮、插件导入和站点登录入口展开。

### 4.2 从剪贴板解析

图片示例：

<img src="https://github.com/user-attachments/assets/7ed8aba0-0457-4ad4-9460-72d2508156f4" alt="剪贴板解析入口示例" width="720">

1. 先在浏览器或其他程序中复制目标链接。
2. 在 Home 页面或 Video 页面点击剪贴板按钮。
3. 软件会读取系统剪贴板内容并尝试解析。

### 4.3 使用历史记录

图片示例：

<img src="https://github.com/user-attachments/assets/ed61b979-0f11-4a4b-b360-e0f17ce7a3d7" alt="历史记录入口示例" width="720">

1. 点击 Home 或 Video 页面中的历史按钮。
2. 在弹出的历史菜单中选择之前解析过的链接。
3. 选中的链接会填入输入框，可再次解析。

说明：

- 软件只有在插件支持该网站时才能解析链接。
- 播放列表是否被完整解析，取决于插件能力和高级设置中的 NoParsePlayList 配置。
- 如果链接为空，软件会提示 url is empty。

## 5. 查看视频列表

链接解析成功后，Video 页面会显示结果列表。页面上方仍可继续输入新链接，下方为解析结果区域。

参考图 2 和图 3，Video 页面通常由上方工具区、结果列表区和详情面板组成。

### 5.1 切换网格和列表视图

图片示例：

<img src="https://github.com/user-attachments/assets/ed61b979-0f11-4a4b-b360-e0f17ce7a3d7" alt="Video 页面网格和列表切换示例" width="720">

Video 页面提供 Grid 和 List 两种视图：

- Grid：以封面卡片形式展示，适合浏览多个视频。
- List：以列表形式展示，适合查看标题、作者、时长等信息。

点击 Grid/List 切换按钮即可切换显示方式。

### 5.2 查看详情

图片示例：

<img src="https://github.com/user-attachments/assets/36b0727e-6da4-4468-9a9d-0e0784a34c39" alt="视频详情面板示例" width="720">

在视频条目上点击详情按钮，或通过右键菜单选择 Show Infomation，可以打开右侧详情面板。详情面板通常包含：

- 标题
- 发布者
- 时长
- 发布时间
- 描述
- 下载保存目录

参考图 3，详情面板适合在不离开列表的情况下核对视频信息或下载文件信息。

### 5.3 搜索结果

图片示例：

<img src="https://github.com/user-attachments/assets/ed61b979-0f11-4a4b-b360-e0f17ce7a3d7" alt="Video 页面搜索入口示例" width="720">

1. 点击搜索按钮。
2. 输入标题关键词。
3. 列表会只显示标题匹配的条目。
4. 点击重置按钮可恢复显示全部条目。

### 5.4 排序结果

图片示例：

<img src="https://github.com/user-attachments/assets/ed61b979-0f11-4a4b-b360-e0f17ce7a3d7" alt="Video 页面排序入口示例" width="720">

点击排序按钮后，可以选择排序字段和排序方向。

排序字段：

- Original Order：原始顺序
- Title：标题
- Publish Date：发布时间
- Duration：时长

排序方向：

- Ascending order：升序
- Descending order：降序

### 5.5 创建下载任务

图片示例：

<img src="https://github.com/user-attachments/assets/36b0727e-6da4-4468-9a9d-0e0784a34c39" alt="视频条目下载操作示例" width="720">

可以通过以下方式创建下载任务：

- 单个下载：点击某个视频条目的下载按钮，或右键选择 Download。
- 下载选中项：先选择多个条目，再点击 Download Selected。
- 下载全部：点击 Download All。

创建成功后，软件会提示 video add to download，并将任务加入 Download 页面的下载中列表。

## 6. 下载管理

Download 页面分为两个标签：

- Downloading：下载中任务。
- Completed：已完成任务。

参考图 4，下载相关页面通常包含任务列表、批量操作按钮和详情面板。

### 6.1 下载中任务

图片示例：

<img src="https://github.com/user-attachments/assets/4cb98dad-08c6-490c-b7ce-9cd59e39d4e2" alt="下载中任务管理示例" width="720">

Downloading 页面显示当前未完成任务。每个任务会展示文件名、进度、下载速度、总大小、阶段状态和剩余时间。

可用操作：

| 操作 | 说明 |
| --- | --- |
| Start All | 开始或继续全部任务 |
| Stop All | 暂停全部任务 |
| Delete All | 删除全部下载中任务 |
| Selected Action | 对选中的任务批量执行 Start Selected、Pause Selected、Delete Selected |
| 暂停按钮 | 暂停或继续单个任务 |
| 文件夹按钮 | 打开文件所在目录；当任务错误时，该按钮会变为重新开始 |
| 删除按钮 | 删除单个下载中任务 |
| 详情按钮 | 打开下载详情面板 |
| 右键菜单 | Start Download、Pause Download、Delete、Open Folder、Show Infomation |

当任务下载完成后，会自动从 Downloading 移动到 Completed。

### 6.2 已完成任务

图片示例：

<img src="https://github.com/user-attachments/assets/4cb98dad-08c6-490c-b7ce-9cd59e39d4e2" alt="已完成任务管理示例" width="720">

Completed 页面显示历史完成记录。

可用操作：

| 操作 | 说明 |
| --- | --- |
| Clear All | 清空所有已完成记录 |
| Reloaded All | 将全部已完成项重新加入下载 |
| Scan | 扫描文件是否仍存在 |
| 删除按钮 | 删除单个完成记录 |
| 重新下载按钮 | 将单个完成项重新加入下载 |
| 文件夹按钮 | 在文件管理器中定位文件 |
| 详情按钮 | 查看已完成文件信息 |

如果文件已被移动或删除，Scan 后列表会标记该条目，并显示 File does not exist。

## 7. 登录与账号

部分网站需要登录后才能解析或下载会员、收藏、历史等内容。sVanilla 支持插件提供的登录方式。

参考图 5，登录状态和插件能力会影响界面中展示的账号信息、网站图标和可用菜单。

### 7.1 登录下载站点

图片示例：

<img src="https://github.com/user-attachments/assets/a06b401e-ba35-46e5-9be3-62e02616a773" alt="站点登录入口示例" width="720">

1. 打开 Home 页面。
2. 点击 Login Download Website。
3. 在弹出的菜单中选择要登录的网站插件。
4. 根据插件支持的方式完成登录：
   - Web 登录：在内置网页窗口中登录。
   - 二维码登录：扫码后在对应 App 中确认。
   - Cookie 登录：选择 Set Cookie，并粘贴对应网站 Cookie。
5. 登录成功后，软件会切换到 Settings 的 Account 页面。

### 7.2 管理账号

图片示例：

<img src="https://github.com/user-attachments/assets/a06b401e-ba35-46e5-9be3-62e02616a773" alt="账号管理页面示例" width="720">

Settings > Account 页面会显示已登录账号。

账号项通常包含：

- 头像
- 用户名
- 会员类型
- 个人主页链接

可用操作：

- History：读取该账号在对应站点的历史记录，并切换到 Video 页面展示。
- Logout：退出该站点账号。

如果 Home 页面提示 No login plugin available，表示当前没有插件提供登录能力。

## 8. 插件管理

sVanilla 依赖插件支持不同网站。插件决定软件可以识别哪些链接、如何登录、如何获取下载地址。

### 8.1 导入插件

图片示例：

<img src="https://github.com/user-attachments/assets/7ed8aba0-0457-4ad4-9460-72d2508156f4" alt="导入插件入口示例" width="720">

1. 打开 Home 页面。
2. 点击 Import Plugin。
3. 选择插件动态库文件。
4. 软件会校验插件并复制到插件目录。
5. 如果软件安装在受保护目录，系统可能要求管理员权限。
6. 重启软件后，新插件生效。

### 8.2 启用或停用插件

图片示例：

<img src="https://github.com/user-attachments/assets/4cb98dad-08c6-490c-b7ce-9cd59e39d4e2" alt="插件列表配置示例" width="720">

1. 打开 Settings > Plugins。
2. 在插件表格中查看插件信息：
   - Name
   - Load
   - Version
   - ID
   - FileName
   - Description
3. 修改 Load 勾选状态。
4. 重启软件后配置生效。

注意：无效插件会在列表中置灰，通常不能启用。

## 9. 常用设置

打开 Settings 页面后，可以在顶部切换 Default、Account、Advanced、Plugins、About。

参考图 4，Settings 页面按功能分组展示配置项，常用配置集中在 Default 和 Advanced 页面。

### 9.1 Default 设置

图片示例：

<img src="https://github.com/user-attachments/assets/4cb98dad-08c6-490c-b7ce-9cd59e39d4e2" alt="Default 设置页面示例" width="720">

Default 页面包含下载、外观、系统托盘和启动行为。

下载设置：

| 配置 | 说明 |
| --- | --- |
| Path | 默认下载保存目录 |
| NameRule | 文件命名规则 |
| Resolution | 默认清晰度选项，例如 1080P、720P |
| Thread | 下载线程数；当前界面中该项可能隐藏 |

外观设置：

| 配置 | 说明 |
| --- | --- |
| Light | 浅色主题 |
| Dark | 深色主题 |
| Auto | 跟随系统主题 |
| Language | English 或中文；修改后通常需要重启生效 |

系统托盘和启动：

| 配置 | 说明 |
| --- | --- |
| Enable SystemTray | 启用系统托盘图标 |
| Minimize to SystemTray | 关闭窗口时最小化到托盘 |
| Open at startup | 开机启动 |
| Keep size and position of the window when exit | 退出时保存窗口大小和位置 |
| Automatically resume all unfinished tasks | 启动后自动恢复未完成任务 |

Default 页面底部会显示 Aria2 Status，包括：

- Status
- Version
- Features

如果状态为 Connected，表示 aria2 通信正常；如果显示 Disconnected 或错误信息，需要检查 aria2 配置。

### 9.2 Advanced 设置

图片示例：

<img src="https://github.com/user-attachments/assets/4cb98dad-08c6-490c-b7ce-9cd59e39d4e2" alt="Advanced 设置页面示例" width="720">

Advanced 页面包含 Video Page 和 Aria2 Settings。

Video Page：

| 配置 | 说明 |
| --- | --- |
| Default View | 默认使用 Grid 或 List |
| Sort by | 默认排序字段，可选 default、title、time、duration |
| Sort Order | 默认升序或降序 |
| NoParsePlayList | 启用后，解析播放列表链接时只解析链接对应的单个视频，而不是完整列表 |

Aria2 Settings：

| 配置 | 说明 |
| --- | --- |
| Remote Aria2 | 使用远程 aria2 下载 |
| Url | 远程 aria2 地址 |
| Port | 远程 aria2 端口 |
| Token | 远程 aria2 RPC token |
| Aria2 Advanced Settings | 启用 aria2 高级参数配置 |

修改远程 aria2 配置后，建议返回 Default 页面查看 Aria2 Status 是否连接成功。

### 9.3 About 页面

图片示例：

<img src="https://github.com/user-attachments/assets/a06b401e-ba35-46e5-9be3-62e02616a773" alt="About 页面示例" width="720">

About 页面包含：

- 软件版本
- Git hash
- 分支
- 构建时间
- 作者信息
- 开源许可证
- GitHub、官网、Telegram、QQ 等链接

常用按钮：

| 按钮 | 说明 |
| --- | --- |
| Copy Info | 复制软件版本和构建信息 |
| Open Log | 打开日志目录或日志文件 |
| Check Update | 检查更新 |
| Update Log | 查看更新日志 |

## 10. 文件与数据

sVanilla 会在本地保存配置、搜索历史、下载中记录、已完成记录和登录 Cookie 等信息。根据源码结构，主要数据类型包括：

- 搜索历史
- 下载中任务
- 已完成任务
- Cookie 信息
- 账号头像缓存
- 封面缓存
- 窗口位置和大小

下载文件保存位置由 Settings > Default > Path 控制。文件名由 NameRule 控制。

## 11. 常见问题

### 11.1 链接无法解析

可能原因：

- 没有安装或启用支持该网站的插件。
- 链接格式不被插件支持。
- 网站需要登录后才能访问内容。
- 网络不可用或网站访问失败。

处理方法：

1. 确认 Settings > Plugins 中相关插件已启用。
2. 尝试在 Home 页面登录对应站点。
3. 检查链接是否能在浏览器中正常打开。
4. 重启软件后再试。

### 11.2 下载任务添加失败

可能原因：

- 插件未能获取可下载地址。
- 文件已经存在于已完成记录中。
- 下载目录不可写。
- aria2 未连接或配置错误。

处理方法：

1. 检查 Settings > Default > Path 是否可写。
2. 检查 Settings > Default 底部 Aria2 Status。
3. 如果使用远程 aria2，检查 Advanced 中的 Url、Port、Token。
4. 在 Completed 页面确认是否已有相同任务。

### 11.3 已完成列表提示文件不存在

Completed 页面中的 Scan 会检查本地文件是否仍存在。如果文件被移动、重命名或删除，列表会提示 File does not exist。

处理方法：

- 将文件移回原下载目录。
- 删除该完成记录。
- 点击重新下载按钮重新下载。

### 11.4 修改语言、插件后没有立即变化

语言和插件配置通常需要重启后生效。修改后请退出并重新打开 sVanilla。

### 11.5 关闭窗口后软件仍在运行

如果启用了 Minimize to SystemTray，关闭窗口会隐藏到系统托盘。需要完全退出时，请右键托盘图标并选择 Quit，或关闭 Minimize to SystemTray。

## 12. 使用建议

- 首次使用时，先在 Settings > Plugins 确认需要的网站插件已加载。
- 下载前先在 Settings > Default 设置好下载目录和命名规则。
- 需要下载登录后内容时，先在 Home 页面完成站点登录。
- 批量下载播放列表前，确认 Advanced > NoParsePlayList 没有被错误启用。
- 使用远程 aria2 时，先确认 Settings > Default 的 Aria2 Status 为 Connected。

## 13. 合规说明

sVanilla 仅提供网络资源下载能力，不提供资源上传、存储到服务器或内容分发功能。用户下载的内容版权归原作者或权利方所有。请在遵守当地法律法规、网站服务条款和版权要求的前提下使用本软件。
