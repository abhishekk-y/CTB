#!/usr/bin/env python3
"""
AI Runner — Called by C++ UI via QProcess
Reads events from /tmp/logsentinel_events.json, runs ML ensemble,
generates AI-powered attack explanation, outputs structured JSON report to stdout.

ML Models in Ensemble:
- Isolation Forest: Global outlier/anomaly detection
- LSTM: Temporal sequence pattern analysis
- GNN: Entity relationship graph analysis
"""
import sys
import json
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from core.ai_explainer import generate_explanation

def main():
    """Main entry point for AI analysis pipeline with ML ensemble."""
    risk_score = float(sys.argv[1]) if len(sys.argv) > 1 else 0
    risk_level = sys.argv[2] if len(sys.argv) > 2 else "low"
    enable_ml = sys.argv[3].lower() == "true" if len(sys.argv) > 3 else True

    events = []
    try:
        with open("/tmp/logsentinel_events.json", "r") as f:
            events = json.load(f)
    except Exception as e:
        error_output = {
            "success": False,
            "error": f"Failed to read events: {e}",
            "explanation": None,
            "ml_ensemble": None,
        }
        print(json.dumps(error_output, indent=2))
        sys.exit(1)

    # Generate comprehensive analysis with ML ensemble
    result = generate_explanation(
        events,
        chains=[],
        risk_score=risk_score,
        risk_level=risk_level,
        enable_isolation_forest=enable_ml,
        enable_lstm=enable_ml,
        enable_gnn=enable_ml,
    )

    # Prepare structured output
    output = {
        "success": True,
        "explanation": result.get("explanation"),
        "model_used": result.get("model_used"),
        "risk_score": result.get("risk_score"),
        "risk_level": result.get("risk_level"),
        "ml_ensemble": result.get("ml_ensemble"),
    }

    # Output as JSON (C++ UI will parse this)
    print(json.dumps(output, indent=2))

if __name__ == "__main__":
    main()


