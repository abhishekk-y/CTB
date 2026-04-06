#!/usr/bin/env python3
"""
AI Runner — Called by C++ UI via QProcess
Reads events from /tmp/logsentinel_events.json, calls AI, prints report to stdout.
"""
import sys
import json
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from core.ai_explainer import generate_explanation

def main():
    risk_score = float(sys.argv[1]) if len(sys.argv) > 1 else 0
    risk_level = sys.argv[2] if len(sys.argv) > 2 else "low"

    events = []
    try:
        with open("/tmp/logsentinel_events.json", "r") as f:
            events = json.load(f)
    except Exception as e:
        print(f"Error reading events: {e}", file=sys.stderr)
        sys.exit(1)

    result = generate_explanation(events, [], risk_score, risk_level)
    print(result['explanation'])

if __name__ == "__main__":
    main()
