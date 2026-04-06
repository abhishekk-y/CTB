<div align="center">

# 🟣 ANURAG — Detection Engineer
### All 6 Detection Pipelines (Network, Auth, System, App, Cloud, Container/DB)
**You are Layer 2 — You detect every threat.**

</div>

---

## ⏱️ YOUR EXACT SCHEDULE (IST)

| Time | What To Do |
|------|------------|
| **12:30 PM – 1:00 PM** | Fork repo, clone, set up environment, create 6 GitHub Issues |
| **1:00 PM – 3:00 PM** | Network detection pipeline (C2 beacon, port scan, DNS tunnel, exfil) |
| **3:00 PM – 5:00 PM** | Auth detection pipeline (brute force, impossible travel, priv escalation) |
| **5:00 PM – 6:30 PM** | System detection pipeline (cron tampering, root cmd, kernel panic) |
| **6:30 PM – 8:00 PM** | Application detection pipeline (SQLi/XSS/RCE, API abuse, path traversal) |
| **8:00 PM – 9:00 PM** | Cloud detection pipeline (IAM abuse, S3 export, role escalation) |
| **9:00 PM – 10:00 PM** | Container/DB detection pipeline (pod crash, DROP/TRUNCATE, schema change) |
| **10:00 PM – 4:30 AM** | Integration with Ishan (ingestion) and Abhi (ML ensemble) |
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
├── src/python/core/detector.py          ← Main detection engine (ENHANCE THIS)

❌ DO NOT touch:
├── src/ui/*                             ← Shubhanshu's
├── src/cpp/*                            ← Ishan's
├── src/python/core/ai_explainer.py      ← Abhi's
├── src/python/core/blockchain.py        ← Laxit's
├── src/python/core/analyzer.py          ← Ishan's
├── src/python/core/report_generator.py  ← Shubhanshu's
```

---

## 🕐 12:30 PM – 1:00 PM | Setup & Issues

```bash
git checkout -b feat/anurag-network-detection
```

**Create these 6 GitHub Issues:**

1. `[FEAT] Network detection — C2 beacon, port scan, DNS tunnel, exfil` → Labels: `enhancement`, `detection`, `anurag`
2. `[FEAT] Auth detection — brute force, impossible travel, priv escalation` → Labels: `enhancement`, `detection`, `anurag`
3. `[FEAT] System detection — cron tampering, root cmd, kernel panic` → Labels: `enhancement`, `detection`, `anurag`
4. `[FEAT] App detection — SQLi/XSS/RCE, API abuse, shell injection` → Labels: `enhancement`, `detection`, `anurag`
5. `[FEAT] Cloud detection — IAM abuse, S3 export, region anomaly` → Labels: `enhancement`, `detection`, `anurag`
6. `[FEAT] Container/DB detection — pod crash, privilege mount, schema change` → Labels: `enhancement`, `detection`, `anurag`

---

## 🕐 1:00 PM – 3:00 PM | Network Detection

**Branch:** `feat/anurag-network-detection`

**Enhance `detector.py`** — add these detection rules:

| Detection | Logic | MITRE ID |
|-----------|-------|----------|
| C2 Beacon | Periodic connection pattern | T1071 |
| Port Scan | Multiple ports from same IP | T1046 |
| DNS Tunnel | Long DNS queries, high frequency | T1071.004 |
| Exfil Detect | Large outbound to external IPs | T1041 |

```bash
git add src/python/core/detector.py
git commit -m "feat(detection): network pipeline — C2 beacon, port scan, DNS tunnel, exfil"
git push origin feat/anurag-network-detection
# → Create PR, link Issue #1, reviewers: Ishan
```

---

## 🕐 3:00 PM – 5:00 PM | Auth Detection

```bash
git checkout main && git pull upstream main
git checkout -b feat/anurag-auth-detection
```

| Detection | Logic | MITRE ID |
|-----------|-------|----------|
| Brute Force Rate | 5+ failures from same IP in 60s | T1110 |
| Impossible Travel | 2 geolocations too far apart | T1078 |
| New Device Login | First-time device/IP | T1078 |
| Off-Hours Access | Login outside business hours | T1078 |
| Priv Escalation | sudo/su, role change | T1548 |

```bash
git add src/python/core/detector.py
git commit -m "feat(detection): auth pipeline — brute force, impossible travel, priv escalation"
git push origin feat/anurag-auth-detection
# → Create PR, link Issue #2, reviewers: Ishan, Abhi
```

---

## 🕐 5:00 PM – 6:30 PM | System Detection

```bash
git checkout main && git pull upstream main
git checkout -b feat/anurag-system-detection
```

| Detection | MITRE ID |
|-----------|----------|
| Cron Tampering | T1053 |
| Root Cmd Abuse | T1059 |
| File Permission (chmod 777) | T1222 |
| Kernel Panic | — |
| Service Crash (repeated restart) | T1489 |

```bash
git add src/python/core/detector.py
git commit -m "feat(detection): system pipeline — cron tampering, root cmd, kernel panic"
git push origin feat/anurag-system-detection
```

---

## 🕐 6:30 PM – 8:00 PM | Application Detection

```bash
git checkout main && git pull upstream main
git checkout -b feat/anurag-app-detection
```

| Detection | MITRE ID |
|-----------|----------|
| SQLi / XSS / RCE | T1190 |
| Error Spike Burst | — |
| API Abuse Rate | T1190 |
| Path Traversal (`../`) | T1083 |
| Shell Injection (`;`, `\|`, backtick) | T1059 |

```bash
git commit -m "feat(detection): app pipeline — SQLi/XSS/RCE, API abuse, shell injection"
git push origin feat/anurag-app-detection
```

---

## 🕐 8:00 PM – 9:00 PM | Cloud Detection

```bash
git checkout -b feat/anurag-cloud-detection
```

| Detection | MITRE ID |
|-----------|----------|
| IAM Abuse | T1098 |
| S3 Mass Export | T1530 |
| Role Escalation | T1098 |
| Region Anomaly | T1535 |
| Secret Access | T1552 |

```bash
git commit -m "feat(detection): cloud pipeline — IAM abuse, S3 export, role escalation"
git push origin feat/anurag-cloud-detection
```

---

## 🕐 9:00 PM – 10:00 PM | Container/DB Detection

```bash
git checkout -b feat/anurag-container-detection
```

| Detection | MITRE ID |
|-----------|----------|
| Pod Crash Loop | T1610 |
| Privilege Mount | T1611 |
| Mass DB Read | T1005 |
| DROP/TRUNCATE | T1485 |
| Schema Change | T1565 |

```bash
git commit -m "feat(detection): container/DB pipeline — pod crash, privilege mount, schema change"
git push origin feat/anurag-container-detection
```

---

## 🕐 10:00 PM – 4:30 AM | Integration

```bash
git checkout -b feat/anurag-integration
```

- Wire up with Ishan: receive ECS-normalized events
- Wire up with Abhi: send alerts to ML ensemble
- Each detection outputs:
```python
{
    "pipeline": "network|auth|system|app|cloud|container_db",
    "detection": "brute_force",
    "mitre_id": "T1110",
    "severity": "critical|high|medium|low",
    "confidence": 0.95
}
```

---

## 🔗 Who You Work With

| Direction | Person | What |
|-----------|--------|------|
| Ishan → You | Sends you classified events from the router |
| You → Abhi | Send detection alerts to his ML ensemble |
| You → Shubhanshu | Alert feed shown on dashboard |

---

<div align="center">

*Nothing malicious gets past your pipelines. You are the eyes.* 🟣

</div>
