---
name: commit-submodule-stack
description: Safely prepare, commit, synchronize, and push changes across recursive Git submodules in sVanilla according to dependency order. Use when multiple nested submodules are dirty, the same remote submodule appears at several paths, detached HEADs must be published to dependency branches, child commits must propagate into parent gitlinks, or the root pointer update should be handled separately.
---

# Commit Submodule Stack

Publish a recursive submodule change set from leaves to root while preserving unrelated work. Use `$prepare-commit` for every content or gitlink commit and retain its per-commit approval gate.

## Build the dependency graph

1. Read root and nested `AGENTS.md` files plus each repository's Git, formatting, and test rules.
2. Inspect every `.gitmodules` directly. Do not assume `git submodule foreach --recursive` works in the current shell.
3. Record each path, canonical remote URL, HEAD, branch or detached state, upstream, remote branches, worktree state, parent, and gitlink path.
4. Group instances by canonical remote URL. Treat repeated paths to the same remote as synchronized copies, not separate source changes.
5. Produce a leaf-to-root order. A child content commit must exist remotely before a parent gitlink commit is published.

## Prepare leaf content commits

1. Start with the deepest dirty repository containing real file changes rather than only gitlink movement.
2. Apply `$prepare-commit`: format, group, explicitly stage, show the proposal, and wait for approval.
3. A detached HEAD is not itself an error. Confirm the intended dependency branch from repository facts or explicit user direction.
4. After approval, commit and push the exact commit. When remaining detached is safest, use an explicit refspec such as `git push origin HEAD:master`; never guess a branch.
5. Verify the remote branch resolves to the new commit.

## Synchronize repeated copies

After publishing a leaf commit, proactively update every initialized copy sharing its remote URL.

1. Fetch the target branch in each copy because nested copies may use independent object stores.
2. Before moving HEAD, compare all local modified paths with the fetched target tree. Stop if any content differs.
3. Move the copy to the exact fetched commit using a non-destructive checkout or switch that preserves verified content.
4. Confirm the copy's HEAD equals the published commit and its file-level worktree is clean.
5. Never use `reset --hard`, clean, stash, or overwrite unverified changes merely to make a pointer update succeed.

If Git reports that a branch switch would overwrite local changes even though another copy already committed them, keep the source commit detached, push it by explicit refspec, then fetch and synchronize the other copies individually.

## Commit parent gitlinks

1. Once child copies are synchronized, inspect the immediate parent and separate real parent changes by coherent purpose.
2. Put a dependency gitlink in the commit that needs that dependency. Use a standalone `chore(deps)` commit only when independently meaningful.
3. Use `$prepare-commit` for each parent content or gitlink commit and wait for separate approval.
4. Push the completed parent chain, verify the remote tip, then continue to its parent.
5. Repeat until reaching the root repository.

The root gitlink update is optional. If the user defers it, unstage the pointers to prevent accidental inclusion, preserve them as worktree changes, and report the exact unpublished commits they reference.

## Validate and report

Run the narrowest checks required by each repository. Record formatter absence, interrupted validation, network-dependent tests, and detached-HEAD publishing explicitly; never imply they passed.

Report the dependency order; repository, target branch, hash, subject, and push result; synchronized duplicate paths; root pointer disposition; verification results; remaining changes; and residual risks.
