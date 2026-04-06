<div align="center">

# 🟤 ABHI — AI/ML Engineer
### Universal ML Ensemble + MITRE ATT&CK Tagger + Kill-chain Builder
**You are Layer 3 — You are the brain that scores threats.**

</div>

---

## ⏱️ YOUR EXACT SCHEDULE (IST)

| Time | What To Do |
|------|------------|
| **12:30 PM – 1:00 PM** | Fork repo, clone, set up environment, create 7 GitHub Issues |
| **1:00 PM – 3:00 PM** | Isolation Forest outlier detection model |
| **3:00 PM – 5:00 PM** | LSTM sequence memory model |
| **5:00 PM – 7:00 PM** | GNN entity graph model |
| **7:00 PM – 8:00 PM** | BERT semantic embeddings model |
| **8:00 PM – 9:00 PM** | Weighted ensemble combiner (threat score 0.0–1.0) |
| **9:00 PM – 12:00 AM** | MITRE ATT&CK auto-tagger |
| **12:00 AM – 3:00 AM** | Kill-chain builder (attack narrative) |
| **3:00 AM – 6:00 AM** | Integration with Anurag + Laxit + Shubhanshu |
| **6:00 AM – 8:30 AM** | Testing, bug fixes, help final merge |

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
├── src/python/core/ai_explainer.py      ← AI explanation engine (ENHANCE THIS)
├── src/python/ai_runner.py              ← QProcess entry point (ENHANCE THIS)

❌ DO NOT touch:
├── src/ui/*                             ← Shubhanshu's
├── src/cpp/*                            ← Ishan's
├── src/python/core/detector.py          ← Anurag's
├── src/python/core/blockchain.py        ← Laxit's
├── src/python/core/analyzer.py          ← Ishan's
├── src/python/core/report_generator.py  ← Shubhanshu's
```

---

## 🕐 12:30 PM – 1:00 PM | Setup & Issues

```bash
git checkout -b feat/abhi-isolation-forest
```

**Create these 7 GitHub Issues:**

1. `[FEAT] Isolation Forest outlier detection model` → Labels: `enhancement`, `ai-ml`, `abhi`
2. `[FEAT] LSTM sequence memory model` → Labels: `enhancement`, `ai-ml`, `abhi`
3. `[FEAT] GNN entity graph model` → Labels: `enhancement`, `ai-ml`, `abhi`
4. `[FEAT] BERT embeddings semantic model` → Labels: `enhancement`, `ai-ml`, `abhi`
5. `[FEAT] Weighted ensemble → threat score 0.0–1.0` → Labels: `enhancement`, `ai-ml`, `abhi`
6. `[FEAT] MITRE ATT&CK auto-tagger` → Labels: `enhancement`, `ai-ml`, `abhi`
7. `[FEAT] Kill-chain builder — attack narrative` → Labels: `enhancement`, `ai-ml`, `abhi`

---

## 🕐 1:00 PM – 3:00 PM | Isolation Forest

**Branch:** `feat/abhi-isolation-forest`

**Add to `ai_explainer.py`** — Outlier detection:
- Use `sklearn.ensemble.IsolationForest`
- Features: event frequency, byte count, time deltas, error rates
- Output: `anomaly_score` (0.0 = normal, 1.0 = extreme outlier)

```bash
git add src/python/core/ai_explainer.py
git commit -m "feat(ml): isolation forest outlier model with sklearn"
git push origin feat/abhi-isolation-forest
# → Create PR, link Issue #1, reviewers: Laxit
```

---

## 🕐 3:00 PM – 5:00 PM | LSTM Sequence Memory

```bash
git checkout main && git pull upstream main
git checkout -b feat/abhi-lstm-model
```

- Learns temporal patterns in log sequences
- Detects unusual event orderings
- Input: sliding window of last N events
- Output: `sequence_anomaly_score` (0.0–1.0)

```bash
git add src/python/core/ai_explainer.py
git commit -m "feat(ml): LSTM sequence memory model for temporal patterns"
git push origin feat/abhi-lstm-model
# → Create PR, link Issue #2, reviewers: Anurag
```

---

## 🕐 5:00 PM – 7:00 PM | GNN Entity Graph

```bash
git checkout -b feat/abhi-gnn-model
```

- Build entity relationship graph (users, IPs, services)
- Detect unusual relationship patterns
- Output: `graph_anomaly_score` (0.0–1.0)

```bash
git commit -m "feat(ml): GNN entity graph model for relationship analysis"
git push origin feat/abhi-gnn-model
```

---

## 🕐 7:00 PM – 8:00 PM | BERT Semantic Embeddings

```bash
git checkout -b feat/abhi-bert-embeddings
```

- Encode log messages into semantic vectors
- Compare against known-threat clusters
- Output: `semantic_threat_score` (0.0–1.0)

```bash
git commit -m "feat(ml): BERT embeddings for semantic log similarity"
git push origin feat/abhi-bert-embeddings
```

---

## 🕐 8:00 PM – 9:00 PM | Weighted Ensemble Combiner

```bash
git checkout -b feat/abhi-ensemble
```

```python
# Combine all 4 models:
threat_score = (0.30 * isolation_score +
                0.25 * lstm_score +
                0.25 * gnn_score +
                0.20 * bert_score)
# Output: unified threat_score 0.0–1.0
```

```bash
git commit -m "feat(ml): weighted ensemble combiner → threat score 0.0-1.0"
git push origin feat/abhi-ensemble
```

---

## 🕐 9:00 PM – 12:00 AM | MITRE ATT&CK Tagger

```bash
git checkout -b feat/abhi-mitre-tagger
```

- Auto-tag every alert with MITRE technique IDs
- Map to tactic (Initial Access, Execution, Persistence, etc.)
- Redis 1-hour window for correlation

```bash
git commit -m "feat(ml): MITRE ATT&CK auto-tagger with technique ID mapping"
git push origin feat/abhi-mitre-tagger
```

---

## 🕐 12:00 AM – 3:00 AM | Kill-chain Builder

```bash
git checkout -b feat/abhi-killchain
```

- Chain related alerts into attack narrative
- Lockheed Martin Kill Chain stages
- Group by source IP + time window

```bash
git commit -m "feat(ml): kill-chain builder — chains alerts into attack narrative"
git push origin feat/abhi-killchain
```

---

## 🕐 3:00 AM – 8:30 AM | Integration & Testing

```bash
git checkout -b feat/abhi-integration
```

- Receive alerts from Anurag's detection pipelines
- Send threat scores to Laxit's blockchain for audit
- Send data to Shubhanshu's dashboard

```bash
git commit -m "feat(integration): wire ML ensemble to detection and dashboard"
git push origin feat/abhi-integration
```

---

## 🔗 Who You Work With

| Direction | Person | What |
|-----------|--------|------|
| Anurag → You | Sends detection alerts for ML scoring |
| You → Laxit | Send high-confidence alerts for blockchain audit |
| You → Shubhanshu | Send threat scores, MITRE tags, kill-chains for display |

---

<div align="center">

*Your models separate noise from real attacks. You are the intelligence.* 🟤

</div>
