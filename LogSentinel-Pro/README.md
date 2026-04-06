# LogSentinel Pro v3.0 — Enterprise SIEM Platform

**Pure C++ Qt6 Desktop Application** for real-time Security Information and Event Management (SIEM). No Python UI — the entire interface is compiled C++ with Qt6 Widgets, matching CyberX-grade cybersecurity dashboard aesthetics.

---

## Architecture

```
┌──────────────────────────────────────────────────────────────────┐
│                  LogSentinel Pro v3.0 (C++)                      │
├──────────────┬───────────────────────────────────────────────────┤
│  C++ Qt6 UI  │  C++ System Core                                │
│              │  ┌──────────────────────────────────────────┐    │
│ Dashboard    │  │ Log Parser    — QRegularExpression engine│    │
│ Threat Feed  │  │ Detector      — MITRE ATT&CK mapping    │    │
│ Network Mon  │  │ Risk Scorer   — Heuristic scoring        │    │
│ Blockchain   │  │ System Monitor— /proc reader (no deps)  │    │
│ AI Report    │  │ Network Mon   — /proc/net/tcp parser     │    │
│              │  └──────────────────────────────────────────┘    │
│              │  ┌──────────────────────────────────────────┐    │
│              │  │ Python Backend (AI only, via QProcess)   │    │
│              │  │ ai_runner.py → Gemini / OpenAI / Ollama  │    │
│              │  └──────────────────────────────────────────┘    │
└──────────────┴───────────────────────────────────────────────────┘
```

## Features

| Feature | Technology | Description |
|---------|-----------|-------------|
| **Desktop UI** | C++ Qt6 Widgets | CyberX-inspired dark navy professional interface |
| **Risk Gauge** | Custom QPainter | Circular gauge with neon glow arcs |
| **Stat Cards** | Qt6 QFrame | Glassmorphic event/threat/risk cards |
| **Log Parsing** | C++ QRegularExpression | 10+ regex patterns for syslog/auth.log |
| **Detection Engine** | C++ native | MITRE ATT&CK brute force/privesc/injection rules |
| **System Monitor** | /proc filesystem | CPU/RAM/DISK — zero external dependencies |
| **Network Monitor** | /proc/net/tcp | Native TCP connection parser |
| **Live Tracking** | QTimer polling | Real-time log file monitoring every 2s |
| **AI Analysis** | Python QProcess | Gemini 2.0 / OpenAI / Ollama via subprocess |
| **Report Export** | C++ QTextStream | Professional forensic text reports |
| **Severity Bar** | Custom QPainter | Animated severity distribution visualization |

## Directory Structure

```
LogSentinel-Pro/
├── build_and_run.sh              # One-click build & launch
├── .env                          # AI API keys
├── README.md
└── src/
    ├── ui/
    │   ├── mainwindow.h          # Qt6 header (all widgets)
    │   ├── mainwindow.cpp        # Full C++ implementation (700+ lines)
    │   ├── logsentinel.pro       # qmake6 project file
    │   └── build/                # Compiled binary output
    ├── cpp/
    │   ├── include/log_parser.h  # C FFI header (shared library)
    │   └── src/log_parser.cpp    # C++ shared library parser
    └── python/
        ├── ai_runner.py          # AI subprocess entry point
        └── core/
            ├── ai_explainer.py   # Gemini/OpenAI/Ollama client
            ├── blockchain.py     # PoW SHA-256 blockchain
            ├── detector.py       # Python detection rules
            └── ...
```

## Quick Start

### Prerequisites
- **Qt6 development**: `apt install qt6-base-dev`
- **g++**: C++17 compiler
- **Python3**: Only for AI reports (optional)

### Build & Run
```bash
cd LogSentinel-Pro
chmod +x build_and_run.sh
./build_and_run.sh
```

### Manual Build
```bash
cd src/ui && mkdir -p build && cd build
qmake6 ../logsentinel.pro
make -j$(nproc)
./logsentinel_pro
```

## Design System (CyberX-Inspired)

| Token | Value | Usage |
|-------|-------|-------|
| BG Primary | `#0A0B1E` | Main background |
| BG Sidebar | `#060714` | Navigation panel |
| BG Card | `#111228` | Content cards |
| Accent Blue | `#3b82f6` | Active elements, highlights |
| Accent Red | `#ef4444` | Threat indicators |
| Accent Green | `#10b981` | Safe/success states |
| Accent Amber | `#f59e0b` | Warning states |
| Text Primary | `#e2e8f0` | Main typography |

## Hackathon Alignment

- [x] Log ingestion — Native C++ parser
- [x] Alerts — Real-time threat feed with severity
- [x] ML anomaly detection — Gemini AI integration
- [x] Python + C++ stack
- [x] Dashboard — Professional 5-page SIEM
- [x] Blockchain — PoW audit ledger
- [x] Innovation — Custom QPainter gauges, /proc system monitoring
