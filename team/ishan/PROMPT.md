# Context

I'm working on **LogSentinel Pro v3.0**, an enterprise-grade SIEM platform built by our team (Dead Coder Society). My role is the **Core Infrastructure Lead** — I own the universal log ingestion layer, smart log routing, and the C++ high-speed regex log parser.

We're on a 20-hour sprint starting at **12:30 PM IST (April 6)** and ending at **8:30 AM IST (April 7)**. My schedule:

| Time (IST) | Task | Branch | PR → Issue |
|------------|------|--------|------------|
| 12:30 PM – 1:00 PM | Fork, clone, create 6 Issues | — | — |
| 1:00 PM – 4:30 PM | Ingestion layer + ECS normalizer | `feat/ishan-ingestion-layer` | PR #1 → closes Issue "Universal ingestion layer" |
| 4:30 PM – 7:00 PM | Smart log router (6-type classifier) | `feat/ishan-smart-router` | PR #2 → closes Issue "Smart log router" |
| 7:00 PM – 10:00 PM | C++ parser enhancement (15+ regex) | `feat/ishan-cpp-parser` | PR #3 → closes Issue "C++ log parser" |
| 10:00 PM – 4:30 AM | Integration with Anurag's pipelines | `feat/ishan-integration` | PR #4 → closes Issue "Kafka integration" |
| 4:30 AM – 8:30 AM | Bug fixes, testing, final merge | — | — |

## Git Workflow

Every feature gets its own branch and PR. Before creating a new branch:
```bash
git fetch upstream
git checkout main
git merge upstream/main
git checkout -b feat/ishan-<feature-name>
```

After coding, push and create a PR:
```bash
git add src/cpp/ src/python/core/analyzer.py
git commit -m "feat(ingestion): universal log ingestion with format auto-detection and ECS normalization"
git push origin feat/ishan-ingestion-layer
```
Then on GitHub: "Compare & Pull Request" → base: `Sharingan001/DeadCoderSociety main` → head: your fork's branch. Tag reviewers Anurag and Shubhanshu. Link the Issue with `Closes #<number>`.

## Issues I Create

1. `[FEAT] Universal ingestion layer with format auto-detection` — Labels: `enhancement`, `core`, `ishan`
2. `[FEAT] Normalize all log formats to ECS` — Labels: `enhancement`, `core`, `ishan`
3. `[FEAT] Smart log router — classify and route by type` — Labels: `enhancement`, `core`, `ishan`
4. `[FEAT] Enhance C++ regex log parser with 15+ rules` — Labels: `enhancement`, `backend`, `ishan`
5. `[FEAT] Rate limiting + deduplication` — Labels: `enhancement`, `core`, `ishan`
6. `[FEAT] Kafka integration for log streaming` — Labels: `enhancement`, `core`, `ishan`

## Project Architecture

Layered pipeline:
1. **My Layer (Ingestion + Router)** — receives raw logs from 7 source types (Network, Auth, System, Application, Cloud, Container, DB), auto-detects the format, normalizes to ECS, and routes each event to the correct detection pipeline.
2. Detection Pipelines (Anurag) — analyzes events for threats.
3. ML Ensemble (Abhi) — scores threats using 4 ML models.
4. Blockchain + SOAR (Laxit) — immutable audit trail and automated response.
5. Dashboard + Reports (Shubhanshu) — unified UI and report generation.

## Tech Stack
- **C++17** with `<regex>` for the log parser (extern "C" FFI for Python ctypes)
- **Python 3.10+** for ingestion and routing logic
- **FastAPI** for ingestion endpoints, **Kafka** for streaming

## My Files (only touch these)

- `src/cpp/include/log_parser.h` — C FFI header
- `src/cpp/src/log_parser.cpp` — Regex log parser (currently ~10 rules, needs 15+)
- `src/python/core/analyzer.py` — Python bridge via ctypes (needs format detection, ECS normalizer, router)

## What I Need Built Right Now

### 1. Format Auto-Detection
Inspect the first few lines of incoming data to determine format: JSON, CEF, LEEF, syslog, W3C, CLF, or plain text. Use regex pattern matching and magic bytes.

### 2. ECS Normalizer
Convert any detected format to Elastic Common Schema — standard fields: `@timestamp`, `source.ip`, `destination.ip`, `event.category`, `event.type`, `log.level`, `log.original`.

### 3. Smart Router
Classify each normalized event into one of 6 types (network, auth, system, application, cloud, container_db) based on field patterns, and pass it to the corresponding detection pipeline function.

### 4. Rate Limiter + Dedup
Rate limit at 10,000 events/sec per source. SHA256 hash-based deduplication within a 5-second sliding window.

### 5. C++ Parser Enhancement
Add regex patterns for: AWS CloudTrail JSON, Docker container logs, K8s pod events, Nginx access/error logs, Apache access logs. Keep the extern "C" FFI interface.

Keep code production-quality — proper error handling, docstrings, type hints. Output events should be JSON-serializable dicts.
