<div align="center">

# 🔵 ISHAN — Core Lead
### Universal Ingestion Layer + Smart Log Router + C++ Log Parser
**You are Layer 1 — Everything starts with you.**

</div>

---

## ⏱️ YOUR EXACT SCHEDULE (IST)

| Time | What To Do |
|------|------------|
| **12:30 PM – 1:00 PM** | Fork repo, clone, set up environment, create 6 GitHub Issues |
| **1:00 PM – 4:30 PM** | Build ingestion layer + format auto-detection + ECS normalizer |
| **4:30 PM – 7:00 PM** | Build Smart Log Router (classify events → 6 pipeline types) |
| **7:00 PM – 10:00 PM** | Enhance C++ log parser (`log_parser.cpp`) with 10+ regex rules |
| **10:00 PM – 4:30 AM** | Integration with Anurag's detection pipelines |
| **4:30 AM – 8:30 AM** | Testing, bug fixes, help final merge |

---

## 🍴 STEP 1: FORK & CLONE

```bash
# 1. Go to https://github.com/Sharingan001/DeadCoderSociety
# 2. Click "Fork" button (top right)
# 3. Clone YOUR fork:
git clone https://github.com/<YOUR-USERNAME>/DeadCoderSociety.git
cd DeadCoderSociety

# 4. Add upstream:
git remote add upstream https://github.com/Sharingan001/DeadCoderSociety.git
```

---

## 📂 YOUR FILES (ONLY TOUCH THESE)

```
✅ You PUSH these files:
LogSentinel-Pro/
├── src/cpp/include/log_parser.h        ← C FFI header (enhance)
├── src/cpp/src/log_parser.cpp          ← Regex log parser (enhance)
├── src/python/core/analyzer.py         ← Log analysis bridge (enhance)

❌ DO NOT touch:
├── src/ui/*                            ← Shubhanshu's
├── src/python/core/detector.py         ← Anurag's
├── src/python/core/ai_explainer.py     ← Abhi's
├── src/python/core/blockchain.py       ← Laxit's
├── src/python/core/report_generator.py ← Shubhanshu's
```

---

## 🕐 12:30 PM – 1:00 PM | Setup & Issues

```bash
# Create your branch:
git checkout -b feat/ishan-ingestion-layer
```

**Create these 6 GitHub Issues** (go to repo → Issues → New Issue):

1. **Title:** `[FEAT] Universal ingestion layer with format auto-detection`  
   **Labels:** `enhancement`, `core`, `ishan`  
   **Assignee:** You

2. **Title:** `[FEAT] Normalize all log formats to ECS`  
   **Labels:** `enhancement`, `core`, `ishan`

3. **Title:** `[FEAT] Smart log router — classify by type`  
   **Labels:** `enhancement`, `core`, `ishan`

4. **Title:** `[FEAT] Enhance C++ regex log parser with 10+ rules`  
   **Labels:** `enhancement`, `backend`, `ishan`

5. **Title:** `[FEAT] Rate limiting + deduplication`  
   **Labels:** `enhancement`, `core`, `ishan`

6. **Title:** `[FEAT] Kafka integration for log streaming`  
   **Labels:** `enhancement`, `core`, `ishan`

---

## 🕐 1:00 PM – 4:30 PM | Ingestion Layer

**Branch:** `feat/ishan-ingestion-layer`

**What to code:**
- Auto-detect incoming log format (JSON, CEF, LEEF, syslog, W3C, CLF, plain text)
- Normalize everything to ECS (Elastic Common Schema)
- Rate limit (10k events/sec) + dedup (hash-based, 5-sec window)

**How to push:**
```bash
git add src/python/core/analyzer.py
git add src/cpp/
git commit -m "feat(ingestion): universal log ingestion with format auto-detection and ECS normalization"
git push origin feat/ishan-ingestion-layer
```

**Then create PR on GitHub:**
- Base: `Sharingan001/DeadCoderSociety` → `main`
- Head: `<your-fork>` → `feat/ishan-ingestion-layer`
- Description: Link to Issue #1
- Reviewers: **Anurag**, **Shubhanshu**

---

## 🕐 4:30 PM – 7:00 PM | Smart Log Router

```bash
git checkout main && git pull upstream main
git checkout -b feat/ishan-smart-router
```

**Build the router** — classify every event and send to correct detection pipeline:

| Event Type | Routed To (Anurag's pipeline) |
|-----------|------|
| Network | Firewall, NetFlow, PCAP, DNS |
| Auth | SSH, LDAP, AD, OAuth, Kerberos |
| System | Syslog, Kernel, Cron, Systemd |
| Application | Nginx, Apache, App errors, APIs |
| Cloud | AWS CloudTrail, GCP, Azure AD |
| Container/DB | Docker, K8s, SQL, Pod events |

```bash
git add src/python/core/analyzer.py
git commit -m "feat(router): smart log router with 6-type classification"
git push origin feat/ishan-smart-router
# → Create PR, link Issue #3, reviewers: Anurag
```

---

## 🕐 7:00 PM – 10:00 PM | C++ Log Parser Enhancement

```bash
git checkout main && git pull upstream main
git checkout -b feat/ishan-cpp-parser
```

**Enhance `src/cpp/src/log_parser.cpp`** — the file is already in your folder (`files/`) as reference.

Add more regex patterns:
- Cloud log patterns (CloudTrail JSON)
- Container log patterns (Docker, K8s)
- Application log patterns (Nginx access/error)

```bash
git add src/cpp/
git commit -m "feat(parser): add 15+ regex patterns for cloud, container, app log parsing"
git push origin feat/ishan-cpp-parser
# → Create PR, link Issue #4, reviewers: Anurag, Shubhanshu
```

---

## 🕐 10:00 PM – 4:30 AM | Integration with Anurag

```bash
git checkout -b feat/ishan-integration
```

- Wire up: Ingestion → Router → Anurag's detection pipelines
- Test: raw log → normalize → classify → detect

```bash
git commit -m "feat(integration): connect ingestion to detection pipelines"
git push origin feat/ishan-integration
```

---

## 🕐 4:30 AM – 8:30 AM | Final Testing & Merge

- Fix bugs found during integration
- Help Shubhanshu with final merge to `main`
- Help tag release `v3.0`

---

## 🔗 Who You Work With

| Direction | Person | What |
|-----------|--------|------|
| You → Anurag | Send normalized events to his detection pipelines |
| You → Shubhanshu | Send ingestion metrics to dashboard |
| Anurag → You | Tells you what format he needs events in |

---

<div align="center">

*Every byte of data flows through you first. You are the gatekeeper.* 🔵

</div>
