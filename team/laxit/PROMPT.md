# Context

I'm working on **LogSentinel Pro v3.0**, an enterprise SIEM platform built by our team (Dead Coder Society). I'm the **Blockchain & Security Engineer** — I own the CP-003 hash chain, Polygon root anchoring, and the SOAR auto-response engine.

We're on a 20-hour sprint starting **12:30 PM IST (April 6)** → ending **8:30 AM IST (April 7)**.

| Time (IST) | Task | Branch | PR → Issue |
|------------|------|--------|------------|
| 12:30 PM – 1:00 PM | Fork, clone, create 6 Issues | — | — |
| 1:00 PM – 4:00 PM | CP-003 hash chain implementation | `feat/laxit-hash-chain` | PR #1 → closes "CP-003 hash chain" |
| 4:00 PM – 6:00 PM | Polygon root anchor (Merkle → testnet) | `feat/laxit-polygon-anchor` | PR #2 → closes "Polygon anchor" |
| 6:00 PM – 7:00 PM | Chain integrity verifier | `feat/laxit-chain-verifier` | PR #3 → closes "Chain verifier" |
| 7:00 PM – 11:00 PM | SOAR auto-response engine (3 actions) | `feat/laxit-soar-engine` | PR #4 → closes "SOAR Block IP", "SOAR Isolate", "SOAR Quarantine" |
| 11:00 PM – 4:30 AM | Integration with Abhi + Shubhanshu | `feat/laxit-integration` | PR #5 |
| 4:30 AM – 8:30 AM | Testing, bug fixes, final merge | — | — |

## Git Workflow

Each feature gets its own branch → PR → linked Issue.
```bash
git fetch upstream && git checkout main && git merge upstream/main
git checkout -b feat/laxit-hash-chain
# ... code ...
git add src/python/core/blockchain.py src/python/core/network_monitor.py
git commit -m "feat(blockchain): CP-003 hash chain with SHA256(prev+time+actor+action)"
git push origin feat/laxit-hash-chain
```
Create PR on GitHub → base: main, reviewers: Abhi. Write `Closes #<issue-number>` in PR body.

## Issues I Create

1. `[FEAT] CP-003 hash chain — SHA256(prev+time+actor+action)` → Labels: `enhancement`, `blockchain`, `laxit`
2. `[FEAT] Polygon root anchor for on-chain immutability` → Labels: `enhancement`, `blockchain`, `laxit`
3. `[FEAT] Chain integrity verifier` → Labels: `enhancement`, `blockchain`, `laxit`
4. `[FEAT] SOAR — Block IP / Revoke Token` → Labels: `enhancement`, `security`, `laxit`
5. `[FEAT] SOAR — Isolate Host / Kill Session` → Labels: `enhancement`, `security`, `laxit`
6. `[FEAT] SOAR — Quarantine Container` → Labels: `enhancement`, `security`, `laxit`

## Current Codebase

### `src/python/core/blockchain.py` (72 lines)
Working `Block` and `Blockchain` classes with PoW mining, genesis block, `add_event()`, `is_chain_valid()`, `export_ledger()`. Hash formula is basic — needs CP-003 upgrade.

### `src/python/core/network_monitor.py` (92 lines)
Network monitoring via psutil — `get_active_connections()`, `get_listening_ports()`, `get_connection_summary()`, `get_system_info()`. Needs SOAR auto-response capabilities.

## What I Need Built

### 1. CP-003 Hash Chain
Change hash formula to: `SHA256(previous_hash + timestamp + actor + action)`. Every event gets chained (not just alerts). Actor = username/IP, Action = event type. Keep PoW mining.

### 2. Polygon Root Anchor
Every N blocks (default 100): compute Merkle root from block hashes, submit to Polygon Mumbai testnet via web3.py, store tx hash as proof.

### 3. Chain Verifier
Recompute all hashes from genesis, verify PoW nonces, cross-ref with Polygon anchors, report tampered blocks.

### 4. SOAR Auto-Response
Three actions triggered by Abhi's ML threat scores:
- Block IP (threat_score ≥ 0.9) — generate iptables command, log to blockchain
- Isolate Host (threat_score ≥ 0.85) — network isolation, session kill, log to blockchain
- Quarantine Container (critical severity from container pipeline) — docker pause, k8s isolate, log to blockchain

Every SOAR action gets recorded in the blockchain before execution. `ResponseEngine` class with `evaluate_threat()` and `execute_response()` methods.
