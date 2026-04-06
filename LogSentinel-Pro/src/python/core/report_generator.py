"""
PDF Report Generator
Creates professional forensic PDF reports using reportlab (or fallback text).
"""
import json
import time
import os

def generate_text_report(events, blockchain, risk_score, risk_level, ai_text="", filepath="report.txt"):
    """Generate a professional text-based forensic report."""
    lines = []
    lines.append("=" * 80)
    lines.append("  LOGSENTINEL PRO - FORENSIC INCIDENT REPORT")
    lines.append("=" * 80)
    lines.append(f"  Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}")
    lines.append(f"  Risk Score: {risk_score}/100 ({risk_level.upper()})")
    lines.append(f"  Total Events Analyzed: {len(events)}")
    
    suspicious = [e for e in events if e.get("is_suspicious")]
    lines.append(f"  Threats Detected: {len(suspicious)}")
    lines.append(f"  Blockchain Blocks Mined: {len(blockchain.chain)}")
    lines.append("=" * 80)
    
    # Executive Summary
    lines.append("\n1. EXECUTIVE SUMMARY")
    lines.append("-" * 40)
    
    severity_counts = {"low": 0, "medium": 0, "high": 0, "critical": 0}
    for e in events:
        sev = e.get("severity", "low").lower()
        if sev in severity_counts:
            severity_counts[sev] += 1
    
    lines.append(f"  Critical: {severity_counts['critical']}")
    lines.append(f"  High:     {severity_counts['high']}")
    lines.append(f"  Medium:   {severity_counts['medium']}")
    lines.append(f"  Low:      {severity_counts['low']}")
    
    # IOCs
    lines.append("\n2. INDICATORS OF COMPROMISE (IOCs)")
    lines.append("-" * 40)
    
    unique_ips = set(e.get("ip_address") or e.get("ip", "") for e in suspicious if (e.get("ip_address") or e.get("ip", "")))
    unique_users = set(e.get("user", "") for e in suspicious if e.get("user"))
    unique_rules = set(e.get("detection_rule", "") for e in suspicious if e.get("detection_rule"))
    
    if unique_ips:
        lines.append(f"  Source IPs: {', '.join(unique_ips)}")
    if unique_users:
        lines.append(f"  Target Users: {', '.join(unique_users)}")
    if unique_rules:
        lines.append(f"  Detection Rules: {', '.join(unique_rules)}")
    
    # MITRE
    lines.append("\n3. MITRE ATT&CK MAPPING")
    lines.append("-" * 40)
    
    mitre_set = set()
    for e in suspicious:
        t = e.get("mitre_technique")
        if t:
            mitre_set.add(t)
    for t in sorted(mitre_set):
        lines.append(f"  - {t}")
    if not mitre_set:
        lines.append("  No MITRE techniques mapped.")
    
    # Event Log Table
    lines.append("\n4. EVENT LOG (TOP 50 THREATS)")
    lines.append("-" * 40)
    lines.append(f"  {'#':>4}  {'Severity':<10}  {'Action':<25}  {'User':<15}  {'IP':<18}")
    lines.append(f"  {'—'*4}  {'—'*10}  {'—'*25}  {'—'*15}  {'—'*18}")
    
    for i, e in enumerate(suspicious[:50]):
        lines.append(f"  {i+1:>4}  {e.get('severity','?'):<10}  {e.get('action','?'):<25}  {e.get('user','?'):<15}  {(e.get('ip_address') or e.get('ip','?')):<18}")
    
    # Blockchain Proof
    lines.append(f"\n5. BLOCKCHAIN AUDIT TRAIL ({len(blockchain.chain)} blocks)")
    lines.append("-" * 40)
    for block in blockchain.chain[:20]:
        b = block.to_dict()
        lines.append(f"  Block #{b['index']} | Hash: {b['hash'][:32]}... | Nonce: {b['nonce']}")
    
    # AI Analysis
    if ai_text:
        lines.append("\n6. AI-GENERATED ANALYSIS")
        lines.append("-" * 40)
        lines.append(ai_text)
    
    # Recommendations
    lines.append("\n7. RECOMMENDATIONS")
    lines.append("-" * 40)
    lines.append("  1. Block identified malicious source IPs at the firewall")
    lines.append("  2. Reset credentials for all targeted user accounts")
    lines.append("  3. Enable multi-factor authentication")
    lines.append("  4. Review and harden sudo/privilege policies")
    lines.append("  5. Implement intrusion detection/prevention systems")
    lines.append("  6. Conduct forensic disk imaging for affected hosts")
    
    lines.append("\n" + "=" * 80)
    lines.append("  END OF REPORT - LogSentinel Pro")
    lines.append("=" * 80)
    
    report_text = "\n".join(lines)
    
    with open(filepath, 'w') as f:
        f.write(report_text)
    
    return filepath, report_text
