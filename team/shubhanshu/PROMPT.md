# Context

I'm working on **LogSentinel Pro v3.0**, an enterprise SIEM platform built by our team (Dead Coder Society). I'm the **Dashboard & DevOps Lead** — I own the unified Qt6 desktop UI, deep report generator, and the build/CI system.

We're on a 20-hour sprint starting **12:30 PM IST (April 6)** → ending **8:30 AM IST (April 7)**.

| Time (IST) | Task | Branch | PR → Issue |
|------------|------|--------|------------|
| 12:30 PM – 1:00 PM | Fork, clone, set up branch protection + labels + milestone, create 8 Issues | — | — |
| 1:00 PM – 4:00 PM | Dashboard foundation (6 panels, CyberX theme) | `feat/shubhanshu-dashboard-ui` | PR #1 → closes Issues #1-6 |
| 4:00 PM – 7:00 PM | Dashboard visualizations (gauge, cards, severity bar) | `feat/shubhanshu-dashboard-viz` | PR #2 |
| 7:00 PM – 10:00 PM | Deep report generator (6 report types) | `feat/shubhanshu-reports` | PR #3 → closes "Report generator" |
| 10:00 PM – 12:00 AM | CI/CD pipeline (GitHub Actions + PR template) | `feat/shubhanshu-ci-cd` | PR #4 → closes "CI/CD pipeline" |
| 12:00 AM – 4:30 AM | Integration (connect dashboard to all 4 layers) | `feat/shubhanshu-integration` | PR #5 |
| 4:30 AM – 7:00 AM | Polish (animations, hover effects, screenshots) | `feat/shubhanshu-polish` | PR #6 |
| 7:00 AM – 8:30 AM | Final docs, README update, merge all PRs, tag v3.0 | — | — |

## Git Workflow

Each feature gets its own branch → PR → linked Issue.
```bash
git fetch upstream && git checkout main && git merge upstream/main
git checkout -b feat/shubhanshu-dashboard-ui
# ... code ...
git add src/ui/ src/python/core/report_generator.py build_and_run.sh .github/
git commit -m "feat(ui): unified dashboard with 6 panels"
git push origin feat/shubhanshu-dashboard-ui
```
Create PR on GitHub → base: main, reviewers: Ishan. Write `Closes #<issue-number>` in PR body.

## Repo Admin (First thing I do)

1. **Branch Protection** on `main`: require PR reviews (min 1), restrict direct pushes
2. **Create Labels**: `enhancement`, `bug`, `documentation`, `ishan`, `anurag`, `abhi`, `laxit`, `shubhanshu`, `ui`, `backend`, `ai-ml`, `blockchain`, `devops`, `detection`, `security`, `core`, `priority-high`, `priority-medium`, `priority-low`
3. **Create Milestone**: `v3.0 — LogSentinel Pro`

## Issues I Create

1. `[FEAT] Dashboard — live log stream panel` → Labels: `enhancement`, `ui`, `shubhanshu`
2. `[FEAT] Dashboard — threat heatmap` → Labels: `enhancement`, `ui`, `shubhanshu`
3. `[FEAT] Dashboard — ATT&CK matrix overlay` → Labels: `enhancement`, `ui`, `shubhanshu`
4. `[FEAT] Dashboard — kill-chain timeline` → Labels: `enhancement`, `ui`, `shubhanshu`
5. `[FEAT] Dashboard — zero-trust score` → Labels: `enhancement`, `ui`, `shubhanshu`
6. `[FEAT] Dashboard — hash chain audit viewer` → Labels: `enhancement`, `ui`, `shubhanshu`
7. `[FEAT] Deep report generator — STIX + PDF + scorecard` → Labels: `enhancement`, `reports`, `shubhanshu`
8. `[FEAT] CI/CD pipeline with GitHub Actions` → Labels: `devops`, `shubhanshu`

## Current Codebase

### `src/ui/mainwindow.h` (212 lines)
Custom widgets: `RiskGauge`, `SeverityBar`, `StatCard`, `NavButton`. Theme namespace with CyberX colors. 5-page `MainWindow` with signals/slots.

### `src/ui/mainwindow.cpp` (700+ lines)
Full Qt6 implementation — sidebar nav, stacked widget pages, log parsing, detection, system metrics, blockchain view, AI process.

### `src/python/core/report_generator.py` (110 lines)
Text-based report with Executive Summary, IOCs, MITRE mapping, Event Log, Blockchain trail, Recommendations.

### `build_and_run.sh` (46 lines)
Python venv setup + qmake6/make build + launch.

## What I Need Built

### 1. Dashboard (6 panels)
- **Live Log Stream** — scrolling real-time viewer, color-coded by severity, filters
- **Threat Heatmap** — QPainter grid, time buckets × categories, green→red density
- **ATT&CK Matrix** — MITRE grid, detected techniques highlighted, clickable
- **Zero-Trust Score** — RiskGauge widget with neon glow
- **Kill-chain Timeline** — horizontal stages connected by arrows
- **Hash Chain Viewer** — block cards with index, hash, timestamp, integrity indicator

### Design System
BG: `#0A0B1E`/`#060714`/`#111228`. Borders: `#1e2044`. Accents: blue `#3b82f6`, cyan `#06b6d4`, green `#10b981`, red `#ef4444`, amber `#f59e0b`, purple `#a855f7`. Glassmorphic cards with subtle borders.

### 2. Report Generator (6 types)
STIX 2.1 JSON, PDF incident report, attack timeline PDF, executive summary, zero-trust scorecard, blockchain proof certificate.

### 3. CI/CD
GitHub Actions `build.yml`: Qt6 C++ build + Python pytest. PR template with Description, Related Issue, Files Changed, Testing checklist, Screenshots, Reviewers.

C++ code: proper Qt6 patterns, signals/slots, RAII, QPainter with antialiasing, QPropertyAnimation for micro-animations.
