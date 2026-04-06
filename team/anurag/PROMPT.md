# Context

I'm working on **LogSentinel Pro v3.0**, an enterprise SIEM platform built by our team (Dead Coder Society). I'm the **Detection Engineer** — I own all six detection pipelines that analyze incoming log events for security threats and map them to MITRE ATT&CK techniques.

We're on a 20-hour sprint starting **12:30 PM IST (April 6)** → ending **8:30 AM IST (April 7)**.

| Time (IST) | Task | Branch | PR → Issue |
|------------|------|--------|------------|
| 12:30 PM – 1:00 PM | Fork, clone, create 6 Issues | — | — |
| 1:00 PM – 3:00 PM | Network detection (C2, port scan, DNS tunnel, exfil) | `feat/anurag-network-detection` | PR #1 → closes "Network detection" |
| 3:00 PM – 5:00 PM | Auth detection (brute force, impossible travel, priv esc) | `feat/anurag-auth-detection` | PR #2 → closes "Auth detection" |
| 5:00 PM – 6:30 PM | System detection (cron tamper, root cmd, kernel panic) | `feat/anurag-system-detection` | PR #3 → closes "System detection" |
| 6:30 PM – 8:00 PM | App detection (SQLi/XSS/RCE, API abuse, path traversal) | `feat/anurag-app-detection` | PR #4 → closes "App detection" |
| 8:00 PM – 9:00 PM | Cloud detection (IAM abuse, S3 export, region anomaly) | `feat/anurag-cloud-detection` | PR #5 → closes "Cloud detection" |
| 9:00 PM – 10:00 PM | Container/DB detection (pod crash, DROP/TRUNCATE) | `feat/anurag-container-detection` | PR #6 → closes "Container/DB detection" |
| 10:00 PM – 4:30 AM | Integration with Ishan + Abhi | `feat/anurag-integration` | PR #7 → closes integration issue |
| 4:30 AM – 8:30 AM | Testing, bug fixes, final merge | — | — |

## Git Workflow

Every pipeline gets its own branch → PR → linked Issue.
```bash
git fetch upstream && git checkout main && git merge upstream/main
git checkout -b feat/anurag-network-detection
# ... code ...
git add src/python/core/detector.py
git commit -m "feat(detection): network pipeline — C2 beacon, port scan, DNS tunnel, exfil"
git push origin feat/anurag-network-detection
```
Create PR on GitHub → base: main, reviewers: Ishan. Write `Closes #<issue-number>` in PR body.

## Issues I Create

1. `[FEAT] Network detection — C2 beacon, port scan, DNS tunnel, exfil` → Labels: `enhancement`, `detection`, `anurag`
2. `[FEAT] Auth detection — brute force, impossible travel, priv escalation` → Labels: `enhancement`, `detection`, `anurag`
3. `[FEAT] System detection — cron tampering, root cmd, kernel panic` → Labels: `enhancement`, `detection`, `anurag`
4. `[FEAT] App detection — SQLi/XSS/RCE, API abuse, shell injection` → Labels: `enhancement`, `detection`, `anurag`
5. `[FEAT] Cloud detection — IAM abuse, S3 export, region anomaly` → Labels: `enhancement`, `detection`, `anurag`
6. `[FEAT] Container/DB detection — pod crash, privilege mount, schema change` → Labels: `enhancement`, `detection`, `anurag`

## Current Codebase

### `src/python/core/detector.py`
My main file. Currently ~213 lines with 10 detection rules covering SSH brute force, privilege escalation, port scanning, SQL injection. Has `detect_suspicious_events()` and `calculate_risk_score()`. Each event gets: `is_suspicious`, `severity`, `detection_rule`, `mitre_tactic`, `mitre_technique`.

## What I Need Built

Expand `detector.py` with all 6 pipeline categories. Each detection outputs:
```python
{
    "pipeline": "network|auth|system|app|cloud|container_db",
    "detection": "brute_force",
    "mitre_id": "T1110",
    "severity": "critical|high|medium|low",
    "confidence": 0.95
}
```

**Network:** C2 beacon (periodic intervals), port scan (10+ unique ports same IP), DNS tunnel (long queries), exfil (large outbound to external).

**Auth:** Brute force (5+ failures/60s same IP), impossible travel, new device login, off-hours, privilege escalation.

**System:** Cron tampering, root cmd abuse, chmod 777/setuid, kernel panic, repeated service crash.

**Application:** SQLi/XSS/RCE patterns in HTTP params, error spike burst (sudden 5xx), API abuse rate, path traversal (`../`), shell injection (`;`, `|`, backtick).

**Cloud:** IAM abuse, S3 mass export, role escalation, region anomaly, secret access.

**Container/DB:** Pod CrashLoopBackOff, privilege mount (hostPath, privileged), mass DB read, DROP/TRUNCATE, ALTER TABLE in production.

Keep existing code intact — extend `detect_suspicious_events()` and add helper functions per pipeline. Production-quality with type hints and docstrings.
