# Requirements Repository

本目录存放版本化需求、评审后需求和最终落地需求。

## 推荐结构

```text
requirements/
  README.md
  backlog/
  v2.3/
  accepted/
  delivered/
```

## 文件命名

```text
REQ-YYYYMMDD-short-name.md
```

示例：

```text
REQ-20260613-plugin-enabled-dynamic.md
```

## 状态

- Draft：原始需求或待澄清需求。
- Reviewed：完成需求分析和规格设计。
- Accepted：评审通过，允许实现。
- Delivered：已实现并验收。
- Superseded：被后续需求替代。

## 维护规则

- 原始需求不要覆盖，追加“澄清记录”。
- 最终落地行为必须写入同一个需求文件的“落地结果”。
- 如果需求改变 SDD 覆盖的行为，同步更新 `harness/specs/svanilla/`。
