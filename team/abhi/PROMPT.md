# Context

I'm working on **LogSentinel Pro v3.0**, an enterprise SIEM platform built by our team (Dead Coder Society). I'm the **AI/ML Engineer** — I own the universal ML ensemble (4 models), the MITRE ATT&CK auto-tagger, and the kill-chain narrative builder.

We're on a 20-hour sprint starting **12:30 PM IST (April 6)** → ending **8:30 AM IST (April 7)**.

| Time (IST) | Task | Branch | PR → Issue |
|------------|------|--------|------------|
| 12:30 PM – 1:00 PM | Fork, clone, create 7 Issues | — | — |
| 1:00 PM – 3:00 PM | Isolation Forest outlier model | `feat/abhi-isolation-forest` | PR #1 → closes "Isolation Forest" |
| 3:00 PM – 5:00 PM | LSTM sequence memory model | `feat/abhi-lstm-model` | PR #2 → closes "LSTM model" |
| 5:00 PM – 7:00 PM | GNN entity graph model | `feat/abhi-gnn-model` | PR #3 → closes "GNN model" |
| 7:00 PM – 8:00 PM | BERT semantic embeddings | `feat/abhi-bert-embeddings` | PR #4 → closes "BERT model" |
| 8:00 PM – 9:00 PM | Weighted ensemble combiner | `feat/abhi-ensemble` | PR #5 → closes "Ensemble combiner" |
| 9:00 PM – 12:00 AM | MITRE ATT&CK auto-tagger | `feat/abhi-mitre-tagger` | PR #6 → closes "MITRE tagger" |
| 12:00 AM – 3:00 AM | Kill-chain builder | `feat/abhi-killchain` | PR #7 → closes "Kill-chain builder" |
| 3:00 AM – 6:00 AM | Integration with Anurag + Laxit + Shubhanshu | `feat/abhi-integration` | PR #8 |
| 6:00 AM – 8:30 AM | Testing, bug fixes, final merge | — | — |

## Git Workflow

Each model/feature gets its own branch → PR → linked Issue.
```bash
git fetch upstream && git checkout main && git merge upstream/main
git checkout -b feat/abhi-isolation-forest
# ... code ...
git add src/python/core/ai_explainer.py src/python/ai_runner.py
git commit -m "feat(ml): isolation forest outlier model with sklearn"
git push origin feat/abhi-isolation-forest
```
Create PR on GitHub → base: main, reviewers: Laxit. Write `Closes #<issue-number>` in PR body.

## Issues I Create

1. `[FEAT] Isolation Forest outlier detection model` → Labels: `enhancement`, `ai-ml`, `abhi`
2. `[FEAT] LSTM sequence memory model for temporal patterns` → Labels: `enhancement`, `ai-ml`, `abhi`
3. `[FEAT] GNN entity graph model for relationship analysis` → Labels: `enhancement`, `ai-ml`, `abhi`
4. `[FEAT] BERT embeddings for semantic log similarity` → Labels: `enhancement`, `ai-ml`, `abhi`
5. `[FEAT] Weighted ensemble combiner → threat score 0.0–1.0` → Labels: `enhancement`, `ai-ml`, `abhi`
6. `[FEAT] MITRE ATT&CK auto-tagger with technique ID mapping` → Labels: `enhancement`, `ai-ml`, `abhi`
7. `[FEAT] Kill-chain builder — chain alerts into attack narrative` → Labels: `enhancement`, `ai-ml`, `abhi`

## Current Codebase

### `src/python/core/ai_explainer.py` (311 lines)
AI explanation engine — supports Gemini, OpenAI, Ollama, rule-based fallback. Has `generate_explanation()`, `_build_context()`, model-specific callers, and `_rule_based_explanation()`.

### `src/python/ai_runner.py` (30 lines)
QProcess entry point — reads events from temp JSON, runs AI analysis, prints to stdout.

## What I Need Built

### 1. ML Ensemble (4 models)

**Isolation Forest:** `sklearn.ensemble.IsolationForest`, contamination=0.1. Features: event frequency, byte count, time deltas, error rates. Output: anomaly_score 0.0–1.0.

**LSTM:** PyTorch, hidden_size=64, 2 layers. Sliding window of 50 events encoded as feature vectors. High prediction error = anomalous. Output: sequence_anomaly_score 0.0–1.0.

**GNN:** PyTorch Geometric GCN. Nodes = entities (users, IPs, services). Edges = interactions. Unusual patterns get high scores. Output: graph_anomaly_score 0.0–1.0.

**BERT:** sentence-transformers all-MiniLM-L6-v2. Cosine similarity to known-threat clusters. Output: semantic_threat_score 0.0–1.0.

**Combiner:** `threat_score = 0.30*iso + 0.25*lstm + 0.25*gnn + 0.20*bert`

### 2. MITRE ATT&CK Tagger
Map detections → technique IDs, tactic names. Redis 1-hour correlation window.

### 3. Kill-chain Builder
Group alerts by source IP + time window. Map to Lockheed Martin stages (Recon → C2 → Actions). Build narrative string.

Each model should be its own class with `fit()` and `predict()` methods. Graceful degradation if a model fails.
