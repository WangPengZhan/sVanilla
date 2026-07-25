---
name: prepare-commit
description: Prepare and create reviewed Git commits in sVanilla and its submodules by reading repository rules, preserving existing index state, formatting changed sources, minimizing commit count, staging explicit content, proposing repository-conformant messages, and requiring explicit approval before every commit. Use when asked to prepare, stage, review, split, or create commits or commit messages.
---

# Prepare Commit

Turn a worktree into the smallest reasonable set of focused, reviewable commits. Treat formatting and staging as preparation. Never create a commit until the user explicitly approves the current staged content and message.

## Capture repository state

1. Read every applicable `AGENTS.md`, starting at the repository root and continuing into the target submodule.
2. For the sVanilla root, read the mandatory Harness standards and SDD routes required by `AGENTS.md`.
3. Inspect branch or detached-HEAD state, upstream, remotes, recent subjects, submodule state, `git status --short`, staged diff, unstaged diff, and untracked files.
4. Record pre-existing staged paths. Preserve them unless the user explicitly approves regrouping.
5. Exclude generated output, ignored files, secrets, unrelated changes, and user-maintained build trees.

## Format before grouping

1. Discover the formatter declared by the current repository or submodule.
2. Run the narrowest supported formatter over changed source files before choosing commit boundaries.
3. Do not invent a formatter, install tools, or run a repository-wide rewrite without authorization.
4. If formatting is unavailable or fails, report the exact limitation and continue only when the diff remains reliably reviewable.
5. Reinspect status and diffs after formatting.

## Minimize and stage commits

1. Prefer one commit for one coherent purpose. Split only independently useful, independently revertible, or differently typed changes.
2. Keep implementation with directly related tests, specifications, and documentation.
3. Stage only explicit paths or safe hunks for the next commit. Do not use `git add .` or `git add -A` without proving every path belongs.
4. Verify `git diff --cached --check`, `git diff --cached --stat`, and the complete cached diff.
5. If safe non-interactive hunk staging is impossible, stop and identify the mixed file or hunk.

## Require review for every commit

Present the proposed commit count, grouping rationale, staged paths and statistics, formatter and verification results, exact proposed message, and remaining changes.

Ask the user to approve both staged content and message. Stop. Staging is not permission to commit. Responses such as “批准”, “确定”, “ok”, or “使用该信息提交” approve only the current proposal unless the user explicitly approves a larger named batch.

After approval:

1. Recheck `git diff --cached --check`, staged statistics, and the exact message.
2. Run `git commit` without bypassing hooks.
3. If hooks modify or reject content, inspect the new state and return to review rather than retrying silently.
4. Report the hash, subject, and remaining changes.
5. Return to staging and require separate approval for the next commit.

Do not amend, reset, clean, tag, or push unless the user separately requests it. When the task spans nested submodules, also use `$commit-submodule-stack`.
