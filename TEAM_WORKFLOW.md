<div align="center">

# 🏴‍☠️ Dead Coder Society — Team Collaboration Guide

### LogSentinel Pro v3.0 | 20-Hour Hackathon Sprint

### ⏱️ April 6, 12:30 PM IST → April 7, 8:30 AM IST

[![Team](https://img.shields.io/badge/Team-5%20Members-blueviolet)]()
[![Workflow](https://img.shields.io/badge/Git-Fork%20%26%20PR-orange?logo=git)]()
[![Branches](https://img.shields.io/badge/Branches-Protected-green?logo=github)]()

</div>

---

## 🏗️ System Architecture

```
╔══════════════════════════════════════════════════════════════════════════╗
║                        LOG SOURCES (7 Types)                            ║
║  ┌──────────┐ ┌──────┐ ┌────────┐ ┌─────────┐ ┌───────┐ ┌─────┐ ┌──┐ ║
║  │ Network  │ │ Auth │ │ System │ │   App   │ │ Cloud │ │Cont.│ │DB│ ║
║  │Firewall  │ │ SSH  │ │Syslog  │ │ Nginx   │ │  AWS  │ │Docker│ │SQL║
║  │NetFlow   │ │ LDAP │ │Kernel  │ │ Apache  │ │  GCP  │ │ K8s │ │   │ ║
║  │PCAP, DNS │ │OAuth │ │Cron    │ │ APIs    │ │Azure  │ │Pods │ │   │ ║
║  └────┬─────┘ └──┬───┘ └───┬────┘ └───┬─────┘ └──┬────┘ └──┬──┘ └┬─┘ ║
╚═══════╪══════════╪═════════╪═════════╪══════════╪═════════╪══════╪═══╝
        │          │         │         │          │         │      │
        └──────────┴─────────┴────┬────┴──────────┴─────────┴──────┘
                                  ▼
╔══════════════════════════════════════════════════════════════════════════╗
║  LAYER 1 — ISHAN                                                        ║
║  ┌─────────────────────────────────────────────────────────────────┐    ║
║  │     Universal Ingestion Layer (FastAPI + Kafka)                  │    ║
║  │  Format Auto-Detection: JSON·CEF·LEEF·syslog·W3C·CLF·plaintext │    ║
║  │  Normalization → ECS (Elastic Common Schema)                     │    ║
║  │  Dedup · Rate Limit · HWID Binding                               │    ║
║  └───────────────────────────┬─────────────────────────────────────┘    ║
║                              ▼                                          ║
║  ┌─────────────────────────────────────────────────────────────────┐    ║
║  │              Smart Log Router                                    │    ║
║  │     Classifies every event → routes to correct pipeline          │    ║
║  └──┬───────┬────────┬─────────┬─────────┬──────────┬─────────────┘    ║
╚═════╪═══════╪════════╪═════════╪═════════╪══════════╪══════════════════╝
      │       │        │         │         │          │
      ▼       ▼        ▼         ▼         ▼          ▼
╔══════════════════════════════════════════════════════════════════════════╗
║  LAYER 2 — ANURAG                                                       ║
║  ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐ ┌──────────┐  ║
║  │Network │ │  Auth  │ │System  │ │  App   │ │ Cloud  │ │Container/│  ║
║  │Pipeline│ │Pipeline│ │Pipeline│ │Pipeline│ │Pipeline│ │DB Pipeline│  ║
║  │        │ │        │ │        │ │        │ │        │ │          │  ║
║  │C2 bcon │ │Brute F.│ │Cron    │ │SQLi    │ │IAM     │ │Pod crash │  ║
║  │FFT     │ │Imp.Trv │ │Root cmd│ │XSS/RCE │ │S3 exp  │ │Priv mount│  ║
║  │Port scn│ │New dev │ │File prm│ │API rate│ │Role esc│ │Mass read │  ║
║  │DNS tun │ │Off hrs │ │Kern pnc│ │Path trv│ │Reg anom│ │DROP/TRUNC│  ║
║  │Exfil   │ │Priv esc│ │Svc crsh│ │Shell inj│ │Sec acc│ │Schema chg│  ║
║  └───┬────┘ └───┬────┘ └───┬────┘ └───┬────┘ └───┬────┘ └────┬─────┘  ║
╚══════╪══════════╪══════════╪══════════╪══════════╪═══════════╪═════════╝
       └──────────┴──────────┴────┬─────┴──────────┴───────────┘
                                  ▼
╔══════════════════════════════════════════════════════════════════════════╗
║  LAYER 3 — ABHI                                                         ║
║  ┌─────────────────────────────────────────────────────────────────┐    ║
║  │       Universal ML Ensemble (same 4 models, any log type)       │    ║
║  │                                                                  │    ║
║  │  Isolation Forest (outlier)          GNN (entity graph)          │    ║
║  │  LSTM (sequence memory)              BERT (semantic)             │    ║
║  │                                                                  │    ║
║  │  → weighted ensemble → threat score 0.0–1.0                     │    ║
║  └───────────────────────────┬─────────────────────────────────────┘    ║
║                              ▼                                          ║
║  ┌─────────────────────────────────────────────────────────────────┐    ║
║  │       MITRE ATT&CK Tagger + Kill-chain Builder                  │    ║
║  │  Every alert tagged with technique ID                            │    ║
║  │  Chained into attack narrative · Redis 1hr window                │    ║
║  └───────────────────────────┬─────────────────────────────────────┘    ║
╚══════════════════════════════╪══════════════════════════════════════════╝
                               ▼
╔══════════════════════════════════════════════════════════════════════════╗
║  LAYER 4 — LAXIT                                                        ║
║  ┌─────────────────────────────────────────────────────────────────┐    ║
║  │     CP-003 Hash Chain → Polygon Root Anchor                     │    ║
║  │  SHA256(prev + time + actor + action) on every event             │    ║
║  │  Merkle root → Polygon Mumbai testnet every N blocks             │    ║
║  └───────────────────────────┬─────────────────────────────────────┘    ║
║                              ▼                                          ║
║  ┌─────────────────────────────────────────────────────────────────┐    ║
║  │              SOAR Auto-Response Engine                           │    ║
║  │  Block IP / Revoke Token    │   Isolate Host / Kill Session     │    ║
║  │  Quarantine Container       │   All actions → blockchain audit  │    ║
║  └───────────────────────────┬─────────────────────────────────────┘    ║
╚══════════════════════════════╪══════════════════════════════════════════╝
                               ▼
╔══════════════════════════════════════════════════════════════════════════╗
║  LAYER 5 — SHUBHANSHU                                                   ║
║  ┌─────────────────────────────────────────────────────────────────┐    ║
║  │         Deep Report Generator                                    │    ║
║  │  STIX 2.1 · PDF Incident · Timeline · Executive Summary         │    ║
║  │  Zero-Trust Scorecard · Blockchain Proof Certificate             │    ║
║  └───────────────────────────┬─────────────────────────────────────┘    ║
║                              ▼                                          ║
║  ┌─────────────────────────────────────────────────────────────────┐    ║
║  │    Unified Dashboard (C++ Qt6 + QPainter + WebSocket)           │    ║
║  │                                                                  │    ║
║  │  Live Log Stream (all types)    Threat Heatmap                  │    ║
║  │  ATT&CK Matrix Overlay          Zero-Trust Score                │    ║
║  │  Kill-chain Timeline             Hash Chain Audit Viewer        │    ║
║  └─────────────────────────────────────────────────────────────────┘    ║
╚══════════════════════════════════════════════════════════════════════════╝
```

---

## 👥 Team Roster

| # | Name | Layer | Role | Files Owned |
|---|------|-------|------|-------------|
| 1 | **Ishan** | Layer 1 | Core Lead | `src/cpp/*`, `src/python/core/analyzer.py` |
| 2 | **Anurag** | Layer 2 | Detection Engineer | `src/python/core/detector.py` |
| 3 | **Abhi** | Layer 3 | AI/ML Engineer | `src/python/core/ai_explainer.py`, `src/python/ai_runner.py` |
| 4 | **Laxit** | Layer 4 | Blockchain & Security | `src/python/core/blockchain.py`, `src/python/core/network_monitor.py` |
| 5 | **Shubhanshu** | Layer 5 | Dashboard & DevOps | `src/ui/*`, `report_generator.py`, `build_and_run.sh`, `.github/*` |

---

## 📂 Team Folder Structure

Each member's folder contains:
- `README.md` — Exact IST schedule, git commands, file ownership, step-by-step workflow
- `PROMPT.md` — IDE/AI assistant prompt with full project context, timings, and PRs
- **Source files** — Copies of the actual code files they own (as reference)

```
team/
├── ishan/
│   ├── README.md            ← Schedule + workflow
│   ├── PROMPT.md            ← IDE prompt
│   ├── log_parser.h         ← C FFI header (reference)
│   ├── log_parser.cpp       ← C++ regex parser (reference)
│   └── analyzer.py          ← Python bridge (reference)
│
├── anurag/
│   ├── README.md
│   ├── PROMPT.md
│   └── detector.py          ← Detection engine (reference)
│
├── abhi/
│   ├── README.md
│   ├── PROMPT.md
│   ├── ai_explainer.py      ← AI engine (reference)
│   └── ai_runner.py         ← QProcess entry (reference)
│
├── laxit/
│   ├── README.md
│   ├── PROMPT.md
│   ├── blockchain.py        ← PoW blockchain (reference)
│   └── network_monitor.py   ← Network monitor (reference)
│
└── shubhanshu/
    ├── README.md
    ├── PROMPT.md
    ├── mainwindow.h          ← Qt6 header (reference)
    ├── mainwindow.cpp        ← Qt6 implementation (reference)
    ├── logsentinel.pro       ← qmake6 project (reference)
    ├── report_generator.py   ← Report gen (reference)
    └── build_and_run.sh      ← Build script (reference)
```

---

## 🔧 Setup for EVERY Member

```bash
# 1. Fork: https://github.com/abhishekk-y/CTB → Click "Fork"
# 2. Clone YOUR fork:
git clone https://github.com/<YOUR-USERNAME>/CTB.git
cd CTB

# 3. Add upstream:
git remote add upstream https://github.com/abhishekk-y/CTB.git

# 4. Sync before every new branch:
git fetch upstream && git checkout main && git merge upstream/main
```

---

## 🌿 Branch + Commit Convention

```
Branch:  feat/<name>-<description>    (e.g. feat/ishan-ingestion-layer)
         fix/<name>-<description>     (e.g. fix/anurag-auth-detection-bug)
         docs/<name>-<description>    (e.g. docs/shubhanshu-api-docs)

Commit:  feat(scope): description     (e.g. feat(detection): add brute force rule)
         fix(scope): description
         docs(scope): description
         chore(scope): description
```

---

## 🔁 Pull Request Rules

1. **Never push directly to `main`**
2. **Every PR links to a GitHub Issue** (`Closes #<number>`)
3. **Minimum 1 review** before merge
4. **Use the PR template** (auto-loaded from `.github/PULL_REQUEST_TEMPLATE.md`)

### Review Matrix

| Author | Reviewer 1 | Reviewer 2 |
|--------|-----------|-----------|
| Ishan | Anurag | Shubhanshu |
| Anurag | Ishan | Abhi |
| Abhi | Laxit | Anurag |
| Laxit | Abhi | Shubhanshu |
| Shubhanshu | Ishan | Laxit |

---

## 🎯 Complete Issue Tracker (33 Issues Total)

### Ishan's Issues (6)
| # | Title | Labels |
|---|-------|--------|
| I-1 | `[FEAT] Universal ingestion layer with format auto-detection` | `enhancement`, `core`, `ishan` |
| I-2 | `[FEAT] Normalize all log formats to ECS` | `enhancement`, `core`, `ishan` |
| I-3 | `[FEAT] Smart log router — classify and route by type` | `enhancement`, `core`, `ishan` |
| I-4 | `[FEAT] Enhance C++ regex log parser with 15+ rules` | `enhancement`, `backend`, `ishan` |
| I-5 | `[FEAT] Rate limiting + deduplication` | `enhancement`, `core`, `ishan` |
| I-6 | `[FEAT] Kafka integration for log streaming` | `enhancement`, `core`, `ishan` |

### Anurag's Issues (6)
| # | Title | Labels |
|---|-------|--------|
| A-1 | `[FEAT] Network detection — C2 beacon, port scan, DNS tunnel, exfil` | `enhancement`, `detection`, `anurag` |
| A-2 | `[FEAT] Auth detection — brute force, impossible travel, priv escalation` | `enhancement`, `detection`, `anurag` |
| A-3 | `[FEAT] System detection — cron tampering, root cmd, kernel panic` | `enhancement`, `detection`, `anurag` |
| A-4 | `[FEAT] App detection — SQLi/XSS/RCE, API abuse, shell injection` | `enhancement`, `detection`, `anurag` |
| A-5 | `[FEAT] Cloud detection — IAM abuse, S3 export, region anomaly` | `enhancement`, `detection`, `anurag` |
| A-6 | `[FEAT] Container/DB detection — pod crash, privilege mount, schema change` | `enhancement`, `detection`, `anurag` |

### Abhi's Issues (7)
| # | Title | Labels |
|---|-------|--------|
| B-1 | `[FEAT] Isolation Forest outlier detection model` | `enhancement`, `ai-ml`, `abhi` |
| B-2 | `[FEAT] LSTM sequence memory model` | `enhancement`, `ai-ml`, `abhi` |
| B-3 | `[FEAT] GNN entity graph model` | `enhancement`, `ai-ml`, `abhi` |
| B-4 | `[FEAT] BERT embeddings semantic model` | `enhancement`, `ai-ml`, `abhi` |
| B-5 | `[FEAT] Weighted ensemble → threat score 0.0–1.0` | `enhancement`, `ai-ml`, `abhi` |
| B-6 | `[FEAT] MITRE ATT&CK auto-tagger` | `enhancement`, `ai-ml`, `abhi` |
| B-7 | `[FEAT] Kill-chain builder — attack narrative` | `enhancement`, `ai-ml`, `abhi` |

### Laxit's Issues (6)
| # | Title | Labels |
|---|-------|--------|
| L-1 | `[FEAT] CP-003 hash chain — SHA256(prev+time+actor+action)` | `enhancement`, `blockchain`, `laxit` |
| L-2 | `[FEAT] Polygon root anchor for on-chain immutability` | `enhancement`, `blockchain`, `laxit` |
| L-3 | `[FEAT] Chain integrity verifier` | `enhancement`, `blockchain`, `laxit` |
| L-4 | `[FEAT] SOAR — Block IP / Revoke Token` | `enhancement`, `security`, `laxit` |
| L-5 | `[FEAT] SOAR — Isolate Host / Kill Session` | `enhancement`, `security`, `laxit` |
| L-6 | `[FEAT] SOAR — Quarantine Container` | `enhancement`, `security`, `laxit` |

### Shubhanshu's Issues (8)
| # | Title | Labels |
|---|-------|--------|
| S-1 | `[FEAT] Dashboard — live log stream panel` | `enhancement`, `ui`, `shubhanshu` |
| S-2 | `[FEAT] Dashboard — threat heatmap` | `enhancement`, `ui`, `shubhanshu` |
| S-3 | `[FEAT] Dashboard — ATT&CK matrix overlay` | `enhancement`, `ui`, `shubhanshu` |
| S-4 | `[FEAT] Dashboard — kill-chain timeline` | `enhancement`, `ui`, `shubhanshu` |
| S-5 | `[FEAT] Dashboard — zero-trust score` | `enhancement`, `ui`, `shubhanshu` |
| S-6 | `[FEAT] Dashboard — hash chain audit viewer` | `enhancement`, `ui`, `shubhanshu` |
| S-7 | `[FEAT] Deep report generator — STIX + PDF + scorecard` | `enhancement`, `reports`, `shubhanshu` |
| S-8 | `[FEAT] CI/CD pipeline with GitHub Actions` | `devops`, `shubhanshu` |

---

## ⏱️ Master Timeline (IST)

```
12:30 PM ──── PHASE 1: SETUP ─────────────────────────── 1:00 PM
               All: Fork, clone, create Issues
               Shubhanshu: Branch protection + labels

 1:00 PM ──── PHASE 2: CORE BUILD ────────────────────── 10:00 PM
               Ishan:     Ingestion → Router → C++ Parser
               Anurag:    Network → Auth → System → App → Cloud → Container
               Abhi:      IsoForest → LSTM → GNN → BERT → Ensemble
               Laxit:     Hash Chain → Polygon → Verifier → SOAR
               Shubhanshu: Dashboard → Visualizations → Reports

10:00 PM ──── PHASE 3: INTEGRATION ───────────────────── 4:30 AM
               Ishan ↔ Anurag:     Ingestion → Detection
               Anurag ↔ Abhi:      Detection → ML Scoring
               Abhi ↔ Laxit:       ML → Blockchain Audit + SOAR
               All → Shubhanshu:   Everything → Dashboard

 4:30 AM ──── PHASE 4: POLISH & SUBMIT ───────────────── 8:30 AM
               All:       Testing, bug fixes
               Shubhanshu: Final merge, README, tag v3.0
 8:30 AM ──── 🎉 DONE
```

---

## 📊 Data Flow

```
Raw Log → Ishan (normalize + route)
              ↓
         Anurag (detect threats, tag MITRE)
              ↓
         Abhi (ML score 0.0–1.0, build kill-chain)
              ↓
         Laxit (hash to blockchain, SOAR auto-respond)
              ↓
         Shubhanshu (display on dashboard, generate reports)
```

---

<div align="center">

### 🏴‍☠️ Dead Coder Society — *Code Like Your Life Depends On It* 🚀

</div>
