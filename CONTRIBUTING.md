# Contributing to LogSentinel Pro

## Team Members

| Name | Role | Files Owned |
|------|------|-------------|
| Ishan | Core Lead | `src/cpp/*`, `src/python/core/analyzer.py` |
| Anurag | Detection Engineer | `src/python/core/detector.py` |
| Abhi | AI/ML Engineer | `src/python/core/ai_explainer.py`, `src/python/ai_runner.py` |
| Laxit | Blockchain & Security | `src/python/core/blockchain.py`, `src/python/core/network_monitor.py` |
| Shubhanshu | Dashboard & DevOps | `src/ui/*`, `src/python/core/report_generator.py`, `build_and_run.sh`, `.github/*` |

## Workflow

1. **Fork** the repository to your account
2. **Clone** your fork and add upstream remote
3. **Create a branch** from `main` for each feature
4. **Code** your changes (only in your owned files)
5. **Commit** with conventional messages: `feat(scope): description`
6. **Push** to your fork
7. **Create a PR** to upstream `main`, link the related Issue
8. **Request review** from your assigned reviewer
9. **Address feedback** and get approval
10. **Merge** after approval (squash merge preferred)

## Branch Naming

```
feat/<name>-<description>
fix/<name>-<description>
docs/<name>-<description>
```

## Commit Messages

```
feat(ui): add glassmorphic threat cards
fix(parser): handle edge case in syslog timestamp
docs(readme): update architecture diagram
test(detector): add brute force unit tests
chore(build): update qmake6 config
```

## Code Review

- Minimum 1 approval required
- No merge conflicts
- All checks passing
- Linked to an Issue

## File Ownership

Only edit files you own. If you need changes in another member's files, create an Issue and assign it to them, or coordinate directly.
