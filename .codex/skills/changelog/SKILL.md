---
name: changelog
description: 根据 sVanilla 的 git 提交、diff、tag 与发布范围，整理面向用户且符合本仓库 Harness/SDD 约定的 changelog 或 release notes Markdown。用于用户要求生成、更新、审查或整理发布日志、CHANGELOG、版本说明时；过滤噪音、合并同类改动，输出可直接写入发布说明或 CHANGELOG 的最终版本。
---

# sVanilla Changelog Skill

## 目标

为 sVanilla 发布整理 changelog。读取指定发布范围内的 commit、diff、tag、PR 信息、现有发布说明或 CHANGELOG，并输出可直接使用的 Markdown。

- 默认只输出最终 Markdown，不输出分析过程、分类理由或代码围栏，除非用户明确要求说明。
- 第一目标：最终用户能看懂、同类改动不重复、历史版本不断裂、项目术语与模块边界准确。
- 当仓库没有 `CHANGELOG.md` 时，把用户提供的历史发布说明、GitHub Release 内容或指定基准 tag 作为事实来源。
- 未明确要求写文件时，只输出 Markdown，不主动修改 `CHANGELOG.md` 或发布说明文件。

## 必读上下文

在生成 changelog 前先读取这些项目规则：

- `AGENTS.md`
- `harness/standards/git.md`
- `harness/standards/engineering.md`
- `harness/specs/svanilla/00-sdd-index.md`
- `harness/specs/svanilla/08-agent-working-rules.md`
- 与发布范围相关的 `harness/requirements/`、`harness/knowledge/` 或 SDD 文件

若文档与代码或 diff 冲突，以当前代码、构建文件和可复现实验为准。

## 必要输入

- 目标版本号或发布标题，例如 `v0.4.0`、`v0.4.1-beta.1`。
- 待发布提交范围，例如 `v0.3.0..HEAD`、两个 tag、分支或用户指定 commit 范围。
- 当前发布说明或历史 changelog。若不存在，明确说明使用 git 范围从零整理。

若输入不足，先从最新 tag、当前分支、现有 release notes 和用户上下文推断；仍无法确定发布边界时，只询问缺失的版本号或提交范围。

## 核心规则

### 1. 净变更优先

- 同一功能、修复或文档工作的多条 commit 合并为单条，面向用户描述最终效果。
- commit 标题含糊、口语化或只写内部动作时，必须查看 diff 后改写为清晰描述。
- Revert 处理：原改动被完整撤销则删除该项；仍保留部分效果则合并为一条描述最终结果的条目。
- 格式化、注释、review 调整、纯重排、临时调试、生成产物、无行为变化的机械改动不单独写入。
- `chore`、`refactor`、`perf` 不默认过滤；只要带来用户可感知的稳定性、性能、兼容性或维护收益，就放入合适分类。

### 2. 分类与模块标注

一级标题按变更类型分类。只保留有内容的分类，空分类省略。

```markdown
### 新增 | New
### 改进 | Improved
### 修复 | Fix
### 构建与测试 | Build & Test
### 文档 | Docs
### 其他 | Other
```

分类原则：

- `新增 | New`：新功能、新入口、新平台支持、新兼容能力。
- `改进 | Improved`：通用体验、性能、稳定性、架构收益或跨模块优化。
- `修复 | Fix`：缺陷、崩溃、异常、回归、兼容性修复。
- `构建与测试 | Build & Test`：CMake、vcpkg、CI、脚本、测试 Harness、打包流程。
- `文档 | Docs`：README、Harness/SDD/需求/知识文档。
- `其他 | Other`：确实需要记录但不适合以上分类的内部维护。

用条目前缀标注 sVanilla 模块，避免把模块和变更类型混成同级标题：

- `插件：` 插件加载、插件管理、插件 API、插件通用库、第三方插件集成。
- `下载：` 下载任务、队列、状态、Aria2、FFmpeg、文件保存、压缩/解压。
- `界面：` Qt Widgets/WebEngine 页面、主题、托盘、交互、可见文本。
- `存储：` SQLite、配置、持久化 key、迁移。
- `账号/Cookie：` 登录、Cookie、账号配置和 WebEngine 会话。
- `构建：` CMake、vcpkg、CI、脚本、打包、测试 Harness。

同一条改动不要重复出现在多个分类。若改动跨模块，选用户最容易理解的主分类，并在条目中列出关键模块。

### 3. 文案与排序

- 列表项统一使用 `*`。
- 中文条目在前，纯英文条目排后。
- 中英文与数字间留空格，例如 `修复 3 个下载任务状态异常`。
- 保持术语一致：sVanilla、Qt、Qt WebEngine、CMake、vcpkg、SQLite、Aria2、FFmpeg、Cookie、pluginId、downloadId。
- 用户可见行为优先于内部实现细节；必要时用括号补充内部模块。
- 模块内按重要性排序：用户可见功能/修复 > 兼容性/稳定性 > 构建/文档/维护。
- 如有 PR 或作者信息，保留在条目末尾，例如 `([#123](...)) @author`；没有就不要编造。
- 涉及兼容性或迁移时，在条目里明说影响面，例如插件 API、数据库结构、持久化 key、downloadId、Cookie 维度。

### 4. 版本与历史连续性

- 不带 `-beta`、`-alpha`、`-rc` 后缀的版本视为正式版；带后缀的视为预发布版。
- patch 或预发布版只写相对直接前驱版本的增量，不复制已发布条目。
- 若用户要求合并历史区块，可使用 `<details open>` 展示当前版本，用 `<details>` 收起历史版本。
- 由预发布版晋升为正式版时，优先合并已有预发布 changelog 条目，再只分析最后一个预发布 tag 到 HEAD 的少量增量，避免重复全量重写。

### 5. Highlights

仅当用户要求 release notes 或现有格式包含 Highlights 时生成。

- Highlights 只总结最值得强调的用户价值，不机械搬运全部条目。
- 可以中英双语；若需要英文，放入 `<details><summary><b>English</b></summary>`。
- patch 或预发布版没有重大用户可感知变化时，复用前驱版本 Highlights，仅更新版本标题和日期。

### 6. 必须过滤的噪音

删除以下类型提交，除非 diff 证明有独立用户价值：

- `Release vX.Y.Z`、`Update CHANGELOG`、`Bump version`
- 自动生成、依赖锁文件的纯机械更新、格式化批处理
- 带 `[skip changelog]` 标记的提交
- 完整撤销且无最终效果的 revert
- 构建输出、`out/`、`build/`、临时文件、平台工具二进制变动

### 7. git 查询建议

Windows PowerShell 下优先使用 UTF-8，避免中文提交乱码：

```powershell
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
git -c core.quotepath=false -c i18n.logoutputencoding=utf-8 -c i18n.commitencoding=utf-8 log --encoding=utf-8 --format="%H %s" RANGE
git -c core.quotepath=false diff --stat RANGE
git -c core.quotepath=false diff RANGE -- path/to/module
```

如仍乱码，将输出写入 UTF-8 临时文件再读取。

## 工作流程

1. 确定发布边界：目标版本、前驱版本、提交范围、是否正式版或预发布版。
2. 读取现有发布说明或 CHANGELOG；没有则记录为从 git 范围直接整理。
3. 读取范围内 commit 列表和 diff，不只看 commit 标题。
4. 过滤噪音和被完整 revert 的改动。
5. 按净变更合并同类提交，并改写为面向用户的条目。
6. 按变更类型分类，在条目中标注 sVanilla 模块，并统一排序和术语。
7. 如需要，生成 Highlights 和历史 `<details>` 区块。
8. 输出最终 Markdown。

## 输出模板

无 Highlights 的简洁版本：

```markdown
## vX.Y.Z (YYYY-MM-DD)

### 新增 | New

* 插件：支持 ...

### 修复 | Fix

* 下载：修复 ...

### 构建与测试 | Build & Test

* 构建：调整 ...
```

带 Highlights 和历史区块的版本：

```markdown
## vX.Y.Z (YYYY-MM-DD)

### Highlights

#### 中文标题

中文正文。

<details>
<summary><b>English</b></summary>

#### English Title

English paragraph.

</details>

----

以下是详细内容：

<details open>
<summary><b>vX.Y.Z (YYYY-MM-DD)</b></summary>

### 改进 | Improved

* 改进 ...

### 修复 | Fix

* 修复 ...

</details>
```

## 最终检查

- [ ] 只保留最终有效净变更，而不是机械罗列 commit。
- [ ] 已查看必要 diff，含糊标题已改写。
- [ ] 已过滤 release、bump、纯生成、完整 revert、`[skip changelog]` 等噪音。
- [ ] 没有把旧版本已发布内容重复抄入当前版本。
- [ ] 一级分类是变更类型，模块信息写在条目前缀或正文中。
- [ ] 条目可被普通用户或维护者独立理解。
- [ ] 术语、大小写、空格和 Markdown 层级一致。
- [ ] 没有引用其他项目的专属规则，除非发布范围确实涉及这些内容。
