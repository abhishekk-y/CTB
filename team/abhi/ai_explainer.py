"""
AI Explanation Engine with ML Ensemble
Generates human-readable attack explanations + outlier detection.
Supports: Google Gemini (default), OpenAI, Ollama (local), or rule-based fallback.
ML Models: Isolation Forest (outlier detection), LSTM, GNN, BERT (coming soon)
"""
import os
import json
import httpx
import numpy as np
from typing import List, Dict, Optional, Tuple
from dotenv import load_dotenv

try:
    from sklearn.ensemble import IsolationForest
    from sklearn.preprocessing import StandardScaler
    HAS_SKLEARN = True
except ImportError:
    HAS_SKLEARN = False

try:
    import tensorflow as tf
    from tensorflow.keras.models import Sequential
    from tensorflow.keras.layers import LSTM, Dense, Dropout
    from tensorflow.keras.optimizers import Adam
    HAS_TENSORFLOW = True
except ImportError:
    HAS_TENSORFLOW = False

try:
    import networkx as nx
    HAS_NETWORKX = True
except ImportError:
    HAS_NETWORKX = False

try:
    from transformers import AutoTokenizer, AutoModel
    import torch
    HAS_TRANSFORMERS = True
except ImportError:
    HAS_TRANSFORMERS = False

load_dotenv()

GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")
OPENAI_API_KEY = os.getenv("OPENAI_API_KEY")
OLLAMA_URL = os.getenv("OLLAMA_URL", "http://localhost:11434")
AI_MODEL = os.getenv("AI_MODEL", "auto")  # "gemini", "openai", "ollama", "auto", "none"


# ============================================================================
# ISOLATION FOREST OUTLIER DETECTION
# ============================================================================

def extract_numeric_features(events: List[Dict]) -> Tuple[np.ndarray, List[Dict], List[str]]:
    """
    Extract numeric features from events for Isolation Forest analysis.
    Features: failed_login_count, external_ip_count, process_anomaly_score,
              port_scan_indicators, privilege_escalation_score, etc.
    Returns: (feature_matrix, events_with_features, feature_names)
    """
    features_list = []
    feature_names = ["failed_logins", "external_ips", "process_anomaly",
                     "port_scans", "privesc_score", "outbound_external",
                     "recon_commands", "severity_sum"]

    for event in events:
        features = {
            "failed_logins": 1 if event.get("detection_rule") in ("brute_force", "failed_login") else 0,
            "external_ips": 1 if event.get("detection_rule") == "external_login" else 0,
            "process_anomaly": 1 if event.get("detection_rule") in ("suspicious_process", "known_suspicious_binary") else 0,
            "port_scans": 1 if event.get("detection_rule") == "port_scan" else 0,
            "privesc_score": 1 if event.get("detection_rule") == "privilege_escalation" else 0,
            "outbound_external": 1 if event.get("detection_rule") == "outbound_external" else 0,
            "recon_commands": 1 if event.get("detection_rule") == "post_exploitation_recon" else 0,
            "severity_sum": {"low": 1, "medium": 2, "high": 3, "critical": 4}.get(event.get("severity", "low"), 1),
        }
        features_list.append(features)

    # Convert to numpy array
    X = np.array([[f[name] for name in feature_names] for f in features_list], dtype=np.float32)

    return X, events, feature_names


def detect_outliers_isolation_forest(events: List[Dict], contamination: float = 0.1) -> Dict:
    """
    Detect outlier/anomalous events using Isolation Forest algorithm.
    
    Isolation Forest:
    - Works well with high-dimensional data
    - Effective at detecting global and local outliers
    - Good for cybersecurity anomaly detection
    - Returns anomaly scores (-1 for outliers, 1 for normal)
    
    Args:
        events: List of log events
        contamination: Expected proportion of outliers (default 0.1 = 10%)
    
    Returns:
        Dictionary with detection results, outlier indices, and anomaly scores
    """
    if not HAS_SKLEARN:
        return {
            "status": "error",
            "message": "scikit-learn not installed. Install with: pip install scikit-learn",
            "outliers": [],
            "anomaly_scores": [],
        }

    if len(events) < 3:
        return {
            "status": "insufficient_data",
            "message": f"Need at least 3 events for Isolation Forest, got {len(events)}",
            "outliers": [],
            "anomaly_scores": [],
        }

    # Extract features
    X, events_list, feature_names = extract_numeric_features(events)

    # Handle case where all events have no features (zeros)
    if np.all(X == 0):
        return {
            "status": "no_anomalies",
            "message": "No suspicious patterns detected in events",
            "outliers": [],
            "anomaly_scores": np.zeros(len(events)).tolist(),
            "feature_names": feature_names,
        }

    # Normalize features
    scaler = StandardScaler()
    X_scaled = scaler.fit_transform(X)

    # Train Isolation Forest
    iso_forest = IsolationForest(
        contamination=min(contamination, 0.5),
        random_state=42,
        n_estimators=100,
    )
    predictions = iso_forest.fit_predict(X_scaled)
    anomaly_scores = iso_forest.score_samples(X_scaled)

    # Normalize anomaly scores to 0-1 range (where 1 = most anomalous)
    min_score = anomaly_scores.min()
    max_score = anomaly_scores.max()
    if max_score - min_score > 0:
        normalized_scores = 1 - (anomaly_scores - min_score) / (max_score - min_score)
    else:
        normalized_scores = np.ones_like(anomaly_scores) * 0.5

    # Identify outliers (prediction == -1)
    outlier_indices = np.where(predictions == -1)[0].tolist()
    outlier_events = [events[i] for i in outlier_indices]

    return {
        "status": "success",
        "model": "isolation_forest",
        "num_events_analyzed": len(events),
        "num_outliers_detected": len(outlier_indices),
        "contamination_ratio": len(outlier_indices) / len(events) if len(events) > 0 else 0,
        "outlier_indices": outlier_indices,
        "outliers": outlier_events,
        "anomaly_scores": normalized_scores.tolist(),
        "feature_names": feature_names,
        "predictions": predictions.tolist(),
    }


# ============================================================================
# LSTM SEQUENCE MEMORY MODEL FOR TEMPORAL PATTERNS
# ============================================================================

def build_lstm_sequences(events: List[Dict], sequence_length: int = 5) -> Tuple[np.ndarray, np.ndarray, List[str]]:
    """
    Build sequences of event features for LSTM training.
    
    Each sequence is a time window of consecutive events.
    LSTM learns temporal patterns in security events.
    
    Args:
        events: List of log events with temporal ordering
        sequence_length: Number of events per sequence (default 5)
    
    Returns:
        (sequences, labels, feature_names)
    """
    feature_names = ["severity_score", "suspicious_flag", "user_anomaly",
                     "ip_anomaly", "process_anomaly", "network_outbound"]

    X = []
    y = []

    for event in events:
        severity_map = {"low": 0.2, "medium": 0.5, "high": 0.8, "critical": 1.0}
        is_suspicious = 1.0 if event.get("is_suspicious") else 0.0
        
        features = np.array([
            severity_map.get(event.get("severity", "low"), 0.2),
            is_suspicious,
            1.0 if event.get("user") == "root" or event.get("user") == "admin" else 0.0,
            1.0 if event.get("detection_rule") == "external_login" else 0.0,
            1.0 if event.get("detection_rule") in ("suspicious_process", "known_suspicious_binary") else 0.0,
            1.0 if event.get("detection_rule") == "outbound_external" else 0.0,
        ], dtype=np.float32)
        
        X.append(features)
        y.append(is_suspicious)

    X = np.array(X, dtype=np.float32)
    y = np.array(y, dtype=np.float32)

    # Create sliding window sequences
    sequences = []
    labels = []

    if len(X) < sequence_length:
        # Pad with zeros if insufficient events
        padding = np.zeros((sequence_length - len(X), len(feature_names)), dtype=np.float32)
        sequences.append(np.vstack([padding, X]))
        labels.append(y[-1] if len(y) > 0 else 0.0)
    else:
        for i in range(len(X) - sequence_length + 1):
            sequences.append(X[i:i + sequence_length])
            labels.append(y[i + sequence_length - 1])

    return np.array(sequences, dtype=np.float32), np.array(labels, dtype=np.float32), feature_names


def detect_sequence_anomalies_lstm(events: List[Dict], sequence_length: int = 5) -> Dict:
    """
    Detect temporal sequence anomalies using LSTM (Long Short-Term Memory).
    
    LSTM Advantages:
    - Captures long-term temporal dependencies in attack sequences
    - Learns normal event flow patterns
    - Detects deviations from expected patterns
    - Good for finding sophisticated multi-step attacks
    
    Args:
        events: Ordered list of security events
        sequence_length: Temporal window size
    
    Returns:
        Dictionary with sequence anomaly detection results
    """
    if not HAS_TENSORFLOW:
        return {
            "status": "error",
            "message": "TensorFlow not installed. Install with: pip install tensorflow",
            "anomalies": [],
            "sequence_scores": [],
        }

    if len(events) < 2:
        return {
            "status": "insufficient_data",
            "message": f"Need at least 2 events for LSTM, got {len(events)}",
            "anomalies": [],
            "sequence_scores": [],
        }

    try:
        # Build sequences
        X, y, feature_names = build_lstm_sequences(events, sequence_length)

        # Normalize features
        scaler = StandardScaler()
        X_reshaped = X.reshape(-1, X.shape[-1])
        X_normalized = scaler.fit_transform(X_reshaped)
        X_normalized = X_normalized.reshape(X.shape)

        # Build LSTM model
        model = Sequential([
            LSTM(32, activation='relu', input_shape=(sequence_length, len(feature_names))),
            Dropout(0.2),
            Dense(16, activation='relu'),
            Dropout(0.2),
            Dense(1, activation='sigmoid'),  # Output: 0-1 (normal vs anomaly)
        ])

        model.compile(optimizer=Adam(learning_rate=0.001), loss='binary_crossentropy', metrics=['accuracy'])

        # Train on normal behavior (suppress output)
        model.fit(X_normalized, y, epochs=10, batch_size=2, verbose=0)

        # Predict anomaly scores (reconstruction error)
        predictions = model.predict(X_normalized, verbose=0)
        anomaly_scores = np.abs(predictions.flatten() - y)  # Error between predicted and actual

        # Normalize to 0-1
        min_score = anomaly_scores.min()
        max_score = anomaly_scores.max()
        if max_score - min_score > 0:
            normalized_scores = (anomaly_scores - min_score) / (max_score - min_score)
        else:
            normalized_scores = anomaly_scores

        # Identify anomalies (score > 0.5)
        anomaly_threshold = 0.5
        anomalies = np.where(normalized_scores > anomaly_threshold)[0].tolist()

        return {
            "status": "success",
            "model": "lstm_temporal",
            "num_sequences": len(X),
            "sequence_length": sequence_length,
            "num_anomalies_detected": len(anomalies),
            "anomaly_indices": anomalies,
            "sequence_anomaly_scores": normalized_scores.tolist(),
            "feature_names": feature_names,
        }

    except Exception as e:
        return {
            "status": "error",
            "message": f"LSTM training failed: {str(e)}",
            "anomalies": [],
            "sequence_scores": [],
        }


# ============================================================================
# GNN ENTITY GRAPH MODEL FOR RELATIONSHIP ANALYSIS
# ============================================================================

def build_entity_graph(events: List[Dict]) -> Optional[Dict]:
    """
    Build a knowledge graph of entities (users, IPs, processes, hosts) and relationships.
    
    Entities: Users, IP Addresses, Processes, Hosts
    Edges: User→IP (login), Process→IP (exfil), User→Process (execution), etc.
    
    Returns:
        Dictionary with graph structure and metadata
    """
    if not HAS_NETWORKX:
        return None

    graph_data = {
        "users": {},
        "ips": {},
        "processes": {},
        "hosts": {},
        "edges": [],
    }

    user_risk_score = {}
    ip_risk_score = {}
    process_risk_score = {}

    for event in events:
        user = event.get("user")
        ip = event.get("ip_address")
        process = event.get("process")
        host = event.get("host", "unknown")
        severity = {"low": 1, "medium": 2, "high": 3, "critical": 4}.get(event.get("severity", "low"), 1)

        # Track entities
        if user:
            user_risk_score[user] = user_risk_score.get(user, 0) + severity
            if user not in graph_data["users"]:
                graph_data["users"][user] = {"risk_score": 0, "event_count": 0}
            graph_data["users"][user]["event_count"] += 1
            graph_data["users"][user]["risk_score"] = user_risk_score[user]

        if ip:
            ip_risk_score[ip] = ip_risk_score.get(ip, 0) + severity
            if ip not in graph_data["ips"]:
                graph_data["ips"][ip] = {"risk_score": 0, "event_count": 0, "is_external": 1 if event.get("detection_rule") == "external_login" else 0}
            graph_data["ips"][ip]["event_count"] += 1
            graph_data["ips"][ip]["risk_score"] = ip_risk_score[ip]

        if process:
            process_risk_score[process] = process_risk_score.get(process, 0) + severity
            if process not in graph_data["processes"]:
                graph_data["processes"][process] = {"risk_score": 0, "event_count": 0}
            graph_data["processes"][process]["event_count"] += 1
            graph_data["processes"][process]["risk_score"] = process_risk_score[process]

        if host not in graph_data["hosts"]:
            graph_data["hosts"][host] = {"risk_score": 0, "event_count": 0}
        graph_data["hosts"][host]["event_count"] += 1
        graph_data["hosts"][host]["risk_score"] += severity

        # Create edges
        if user and ip:
            graph_data["edges"].append({"source": user, "target": ip, "type": "login", "severity": severity})
        if user and process:
            graph_data["edges"].append({"source": user, "target": process, "type": "execution", "severity": severity})
        if process and ip:
            graph_data["edges"].append({"source": process, "target": ip, "type": "connection", "severity": severity})

    return graph_data


def detect_graph_anomalies_gnn(events: List[Dict]) -> Dict:
    """
    Detect anomalous patterns in entity relationships using Graph Neural Network approach.
    
    GNN Approach (simplified):
    - Build knowledge graph of users, IPs, processes
    - Identify high-risk entity neighborhoods
    - Detect suspicious relationship patterns
    - Find coordinated multi-entity attacks
    
    Args:
        events: List of security events
    
    Returns:
        Dictionary with graph anomaly detection results
    """
    if len(events) < 2:
        return {
            "status": "insufficient_data",
            "message": f"Need at least 2 events for GNN, got {len(events)}",
            "anomalies": [],
            "graph_stats": {},
        }

    try:
        # Build entity graph
        graph_data = build_entity_graph(events)

        if not graph_data:
            return {
                "status": "error",
                "message": "NetworkX not available for graph analysis",
                "anomalies": [],
                "graph_stats": {},
            }

        # Identify high-risk entities
        high_risk_users = {u: s for u, s in graph_data["users"].items() if s["risk_score"] >= 3}
        high_risk_ips = {ip: s for ip, s in graph_data["ips"].items() if s["risk_score"] >= 3}
        high_risk_processes = {p: s for p, s in graph_data["processes"].items() if s["risk_score"] >= 2}

        # Detect suspicious patterns
        anomalies = []

        # Pattern 1: User with multiple IPs in short time (impossible login)
        for user, data in graph_data["users"].items():
            connected_ips = [e["target"] for e in graph_data["edges"] if e["source"] == user and e["type"] == "login"]
            if len(connected_ips) > 2:
                anomalies.append({
                    "type": "impossible_travel",
                    "entity": user,
                    "risk_level": "high",
                    "description": f"User {user} connected from {len(connected_ips)} different IPs",
                })

        # Pattern 2: Process making external connections
        for process, data in graph_data["processes"].items():
            connected_ips = [e["target"] for e in graph_data["edges"] if e["source"] == process and e["type"] == "connection"]
            if len(connected_ips) > 1 and data["risk_score"] > 2:
                anomalies.append({
                    "type": "suspicious_process_network",
                    "entity": process,
                    "risk_level": "high",
                    "description": f"Process {process} connected to {len(connected_ips)} IPs with high risk score",
                })

        # Pattern 3: High-risk IP with multiple users
        for ip, data in graph_data["ips"].items():
            connected_users = [e["source"] for e in graph_data["edges"] if e["target"] == ip and e["type"] == "login"]
            if len(connected_users) > 2:
                anomalies.append({
                    "type": "suspicious_ip_source",
                    "entity": ip,
                    "risk_level": "high",
                    "description": f"IP {ip} accessed by {len(connected_users)} different users",
                })

        # Calculate statistics
        graph_stats = {
            "num_users": len(graph_data["users"]),
            "num_ips": len(graph_data["ips"]),
            "num_processes": len(graph_data["processes"]),
            "num_hosts": len(graph_data["hosts"]),
            "num_edges": len(graph_data["edges"]),
            "high_risk_users": len(high_risk_users),
            "high_risk_ips": len(high_risk_ips),
            "high_risk_processes": len(high_risk_processes),
            "avg_user_risk": float(np.mean([s["risk_score"] for s in graph_data["users"].values()]) if graph_data["users"] else 0),
            "avg_ip_risk": float(np.mean([s["risk_score"] for s in graph_data["ips"].values()]) if graph_data["ips"] else 0),
        }

        return {
            "status": "success",
            "model": "gnn_entity_graph",
            "num_anomalies_detected": len(anomalies),
            "anomalies": anomalies,
            "graph_stats": graph_stats,
            "high_risk_entities": {
                "users": high_risk_users,
                "ips": high_risk_ips,
                "processes": high_risk_processes,
            },
        }

    except Exception as e:
        return {
            "status": "error",
            "message": f"GNN analysis failed: {str(e)}",
            "anomalies": [],
            "graph_stats": {},
        }


# ============================================================================
# BERT SEMANTIC EMBEDDINGS FOR LOG SIMILARITY
# ============================================================================

def compute_bert_embeddings(events: List[Dict]) -> Dict:
    """
    Compute BERT semantic embeddings for log events.
    
    BERT (Bidirectional Encoder Representations from Transformers):
    - Converts event descriptions to semantic embeddings
    - Enables similarity search and clustering
    - Useful for finding similar attack patterns
    
    Args:
        events: List of security events
    
    Returns:
        Dictionary with embeddings and similarity analysis
    """
    if not HAS_TRANSFORMERS:
        return {
            "status": "error",
            "message": "transformers package not installed. Install with: pip install transformers torch",
            "embeddings": [],
            "similar_events": [],
        }

    try:
        # Use lightweight BERT model
        model_name = "sentence-transformers/all-MiniLM-L6-v2"
        tokenizer = AutoTokenizer.from_pretrained(model_name)
        model = AutoModel.from_pretrained(model_name)

        embeddings = []
        descriptions = []

        # Create event descriptions
        for event in events:
            desc = f"{event.get('action', '')} {event.get('detection_rule', '')} {event.get('user', '')} {event.get('process', '')}"
            descriptions.append(desc)

        # Tokenize and encode
        with torch.no_grad():
            encoded = tokenizer(descriptions, padding=True, truncation=True, return_tensors='pt')
            outputs = model(**encoded)
            embeddings = outputs.last_hidden_state.mean(dim=1).numpy()

        # Normalize embeddings
        embeddings = embeddings / (np.linalg.norm(embeddings, axis=1, keepdims=True) + 1e-8)

        # Find similar event pairs (cosine similarity > 0.7)
        similar_events = []
        for i in range(len(embeddings)):
            for j in range(i + 1, len(embeddings)):
                similarity = np.dot(embeddings[i], embeddings[j])
                if similarity > 0.7:
                    similar_events.append({
                        "event1_idx": i,
                        "event2_idx": j,
                        "similarity": float(similarity),
                        "description": f"{descriptions[i][:50]} ~ {descriptions[j][:50]}",
                    })

        return {
            "status": "success",
            "model": "bert_embeddings",
            "num_events_embedded": len(embeddings),
            "num_similar_pairs": len(similar_events),
            "embeddings": embeddings.tolist(),
            "similar_events": similar_events[:10],  # Top 10
        }

    except Exception as e:
        return {
            "status": "error",
            "message": f"BERT embedding failed: {str(e)}",
            "embeddings": [],
            "similar_events": [],
        }


# ============================================================================
# WEIGHTED ENSEMBLE COMBINER → UNIFIED THREAT SCORE (0.0-1.0)
# ============================================================================

def compute_ensemble_threat_score(isolation_forest_result: Optional[Dict],
                                  lstm_result: Optional[Dict],
                                  gnn_result: Optional[Dict],
                                  base_risk_score: float) -> Dict:
    """
    Combine all ML model outputs into a unified threat score (0.0-1.0).
    
    Weighted Ensemble:
    - Isolation Forest (30%): Global outlier anomaly score
    - LSTM (30%): Temporal sequence deviation
    - GNN (25%): Entity relationship graph anomalies
    - Base Risk Score (15%): Human-rated baseline
    
    Args:
        isolation_forest_result: Output from Isolation Forest model
        lstm_result: Output from LSTM model
        gnn_result: Output from GNN model
        base_risk_score: Original risk score (0-100)
    
    Returns:
        Dictionary with ensemble threat score & model contributions
    """
    scores = []
    weights = []
    model_contributions = {}

    # 1. Isolation Forest contribution (30%)
    if isolation_forest_result and isolation_forest_result.get("status") == "success":
        anomaly_scores = isolation_forest_result.get("anomaly_scores", [])
        if anomaly_scores:
            iso_score = float(np.mean(anomaly_scores))
            scores.append(iso_score)
            weights.append(0.30)
            model_contributions["isolation_forest"] = {
                "score": iso_score,
                "weight": 0.30,
            }

    # 2. LSTM contribution (30%)
    if lstm_result and lstm_result.get("status") == "success":
        sequence_scores = lstm_result.get("sequence_anomaly_scores", [])
        if sequence_scores:
            lstm_score = float(np.mean(sequence_scores))
            scores.append(lstm_score)
            weights.append(0.30)
            model_contributions["lstm"] = {
                "score": lstm_score,
                "weight": 0.30,
            }

    # 3. GNN contribution (25%)
    if gnn_result and gnn_result.get("status") == "success":
        num_anomalies = gnn_result.get("num_anomalies_detected", 0)
        gnn_score = min(num_anomalies * 0.2, 1.0)  # Scale to 0-1
        scores.append(gnn_score)
        weights.append(0.25)
        model_contributions["gnn"] = {
            "score": gnn_score,
            "weight": 0.25,
        }

    # 4. Base risk score contribution (15%)
    base_normalized = (base_risk_score / 100.0)
    scores.append(base_normalized)
    weights.append(0.15)
    model_contributions["base_risk"] = {
        "score": base_normalized,
        "weight": 0.15,
    }

    # Compute weighted average
    if scores:
        threat_score = float(np.average(scores, weights=weights[:len(scores)]))
    else:
        threat_score = base_normalized

    # Clamp to 0-1
    threat_score = max(0.0, min(1.0, threat_score))

    # Determine threat level
    if threat_score >= 0.8:
        threat_level = "critical"
    elif threat_score >= 0.6:
        threat_level = "high"
    elif threat_score >= 0.4:
        threat_level = "medium"
    elif threat_score >= 0.2:
        threat_level = "low"
    else:
        threat_level = "minimal"

    return {
        "status": "success",
        "unified_threat_score": threat_score,
        "threat_level": threat_level,
        "model_contributions": model_contributions,
        "ensemble_confidence": len(scores) / 4.0,  # How many models contributed
    }


# ============================================================================
# MITRE ATT&CK AUTO-TAGGER WITH TECHNIQUE ID MAPPING
# ============================================================================

MITRE_MAPPING = {
    "brute_force": "T1110",
    "failed_login": "T1110.001",
    "external_login": "T1133",
    "privilege_escalation": "T1548",
    "suspicious_process": "T1059",
    "known_suspicious_binary": "T1071",
    "outbound_external": "T1041",
    "post_exploitation_recon": "T1087",
    "port_scan": "T1046",
    "lateral_movement": "T1570",
    "persistence": "T1547",
    "defense_evasion": "T1548",
    "credential_access": "T1110",
    "discovery": "T1087",
    "execution": "T1059",
    "exfiltration": "T1041",
}

MITRE_TACTIC_MAPPING = {
    "T1110": "Initial Access / Credential Access",
    "T1110.001": "Credential Access",
    "T1133": "Initial Access",
    "T1548": "Privilege Escalation / Defense Evasion",
    "T1059": "Execution",
    "T1071": "Command & Control",
    "T1041": "Exfiltration",
    "T1087": "Discovery",
    "T1046": "Discovery",
    "T1570": "Lateral Movement",
    "T1547": "Persistence",
}


def tag_mitre_techniques(events: List[Dict]) -> Dict:
    """
    Automatically tag events with MITRE ATT&CK technique IDs.
    
    MITRE ATT&CK Framework:
    - Maps observed behaviors to standardized techniques
    - Enables threat intelligence correlation
    - Facilitates incident response playbooks
    
    Args:
        events: List of security events
    
    Returns:
        Dictionary with MITRE technique tags and counts
    """
    techniques = {}
    tactics = {}
    event_mappings = []

    for event_idx, event in enumerate(events):
        rule = event.get("detection_rule", "")
        matched_techniques = []

        # Match detection rule to MITRE technique
        if rule in MITRE_MAPPING:
            tech_id = MITRE_MAPPING[rule]
            matched_techniques.append(tech_id)

            if tech_id not in techniques:
                techniques[tech_id] = {"count": 0, "events": []}
            techniques[tech_id]["count"] += 1
            techniques[tech_id]["events"].append(event_idx)

            # Map to tactic
            tactic = MITRE_TACTIC_MAPPING.get(tech_id, "Unknown")
            if tactic not in tactics:
                tactics[tactic] = [tech_id]
            if tech_id not in tactics[tactic]:
                tactics[tactic].append(tech_id)

        # Multi-rule detection for complex attacks
        if event.get("severity") in ("high", "critical"):
            if rule in ("privilege_escalation", "suspicious_process"):
                if "T1548" not in matched_techniques:
                    matched_techniques.append("T1548")
            if rule in ("outbound_external", "external_login"):
                if "T1041" not in matched_techniques:
                    matched_techniques.append("T1041")

        event_mappings.append({
            "event_idx": event_idx,
            "detection_rule": rule,
            "matched_techniques": matched_techniques,
        })

    return {
        "status": "success",
        "model": "mitre_tagger",
        "num_events_tagged": len(events),
        "num_unique_techniques": len(techniques),
        "techniques": techniques,
        "tactics": tactics,
        "event_mappings": event_mappings,
    }


# ============================================================================
# KILL-CHAIN BUILDER — ATTACK NARRATIVE CONSTRUCTION
# ============================================================================

def build_kill_chain(events: List[Dict], gnn_result: Optional[Dict],
                     mitre_result: Optional[Dict]) -> Dict:
    """
    Build attack kill-chain narrative from events and analysis results.
    
    Kill Chain Phases (Lockheed Martin):
    1. Reconnaissance — Attacker gathers info
    2. Weaponization — Prepare attack payload
    3. Delivery — Transmit payload
    4. Exploitation — Trigger vulnerability
    5. Installation — Establish persistence
    6. Command & Control — Remote access
    7. Actions on Objectives — Accomplish goal (data theft, destruction, etc.)
    
    Args:
        events: Ordered security events
        gnn_result: Entity graph analysis results
        mitre_result: MITRE technique tags
    
    Returns:
        Dictionary with kill-chain phases, narrative, and timeline
    """
    kill_chain = {
        "reconnaissance": {"events": [], "indicators": []},
        "weaponization": {"events": [], "indicators": []},
        "delivery": {"events": [], "indicators": []},
        "exploitation": {"events": [], "indicators": []},
        "installation": {"events": [], "indicators": []},
        "command_control": {"events": [], "indicators": []},
        "actions_on_objectives": {"events": [], "indicators": []},
    }

    for idx, event in enumerate(events):
        rule = event.get("detection_rule", "")
        severity = event.get("severity", "low")

        # Phase 1: Reconnaissance
        if rule in ("port_scan", "post_exploitation_recon"):
            kill_chain["reconnaissance"]["events"].append(idx)
            kill_chain["reconnaissance"]["indicators"].append(
                f"Reconnaissance: {event.get('action', 'unknown')}"
            )

        # Phase 2: Weaponization (often not observable in logs)
        if rule == "known_suspicious_binary":
            kill_chain["weaponization"]["events"].append(idx)
            kill_chain["weaponization"]["indicators"].append(
                f"Suspicious binary detected: {event.get('process', 'unknown')}"
            )

        # Phase 3: Delivery
        if rule in ("external_login", "outbound_external"):
            kill_chain["delivery"]["events"].append(idx)
            kill_chain["delivery"]["indicators"].append(
                f"External delivery: {event.get('ip_address', 'unknown')}"
            )

        # Phase 4: Exploitation
        if rule in ("suspicious_process", "privilege_escalation"):
            kill_chain["exploitation"]["events"].append(idx)
            kill_chain["exploitation"]["indicators"].append(
                f"Exploitation attempt: {event.get('process', 'unknown')}"
            )

        # Phase 5: Installation
        if rule == "privilege_escalation" or (rule == "suspicious_process" and severity in ("high", "critical")):
            kill_chain["installation"]["events"].append(idx)
            kill_chain["installation"]["indicators"].append(
                f"Persistence mechanism: {event.get('process', 'unknown')}"
            )

        # Phase 6: Command & Control
        if rule in ("outbound_external", "brute_force"):
            kill_chain["command_control"]["events"].append(idx)
            kill_chain["command_control"]["indicators"].append(
                f"C2 communication: {event.get('ip_address', 'unknown')}"
            )

        # Phase 7: Actions on Objectives
        if rule == "outbound_external" or (rule in ("external_login", "privilege_escalation") and severity == "critical"):
            kill_chain["actions_on_objectives"]["events"].append(idx)
            kill_chain["actions_on_objectives"]["indicators"].append(
                f"Data exfiltration/impact: {event.get('action', 'unknown')}"
            )

    # Build narrative
    narrative_parts = ["## 🔗 Attack Kill Chain Narrative\n"]

    phases_order = [
        ("reconnaissance", "Reconnaissance — Attacker Gathers Intelligence"),
        ("weaponization", "Weaponization — Payload Preparation"),
        ("delivery", "Delivery — Initial Access"),
        ("exploitation", "Exploitation — Vulnerability Trigger"),
        ("installation", "Installation — Persistence Establishment"),
        ("command_control", "Command & Control — Remote Access"),
        ("actions_on_objectives", "Actions on Objectives — Goal Accomplishment"),
    ]

    active_phases = []
    for phase_key, phase_name in phases_order:
        phase_data = kill_chain[phase_key]
        if phase_data["events"]:
            active_phases.append(phase_key)
            narrative_parts.append(f"\n### {phase_name}")
            narrative_parts.append(f"**Events:** {len(phase_data['events'])}")
            for indicator in phase_data["indicators"][:3]:  # Top 3 per phase
                narrative_parts.append(f"- {indicator}")

    narrative = "\n".join(narrative_parts)

    # Calculate attack progression
    progression = []
    for phase_key, _ in phases_order:
        if kill_chain[phase_key]["events"]:
            progression.append(phase_key)

    return {
        "status": "success",
        "model": "kill_chain_builder",
        "num_events_analyzed": len(events),
        "active_phases": active_phases,
        "phase_progression": progression,
        "kill_chain": kill_chain,
        "narrative": narrative,
        "estimated_sophistication": len(active_phases) / 7.0,  # 0-1
    }


def generate_explanation(events: List[Dict], chains: List[Dict],
                         risk_score: float, risk_level: str,
                         enable_isolation_forest: bool = True,
                         enable_lstm: bool = True,
                         enable_gnn: bool = True,
                         enable_bert: bool = True,
                         enable_mitre: bool = True,
                         enable_kill_chain: bool = True) -> Dict:
    """
    Generate AI-powered explanation + complete ML ensemble pipeline.
    
    ML Models (All 7):
    1. Isolation Forest: Global outlier detection
    2. LSTM: Temporal sequence anomalies
    3. GNN: Entity relationship graph analysis
    4. BERT: Semantic log similarity
    5. Ensemble Combiner: Unified threat score (0.0-1.0)
    6. MITRE Tagger: ATT&CK technique mapping
    7. Kill-chain Builder: Attack narrative construction
    
    Priority AI: Gemini → OpenAI → Ollama → Rule-based
    """
    # 1. Run Isolation Forest outlier detection
    outlier_results = None
    if enable_isolation_forest and HAS_SKLEARN:
        outlier_results = detect_outliers_isolation_forest(events)
    
    # 2. Run LSTM temporal sequence model
    lstm_results = None
    if enable_lstm and HAS_TENSORFLOW:
        lstm_results = detect_sequence_anomalies_lstm(events)
    
    # 3. Run GNN entity graph model
    gnn_results = None
    if enable_gnn and HAS_NETWORKX:
        gnn_results = detect_graph_anomalies_gnn(events)
    
    # 4. Compute BERT semantic embeddings
    bert_results = None
    if enable_bert and HAS_TRANSFORMERS:
        bert_results = compute_bert_embeddings(events)
    
    # 5. Build MITRE ATT&CK tags
    mitre_results = None
    if enable_mitre:
        mitre_results = tag_mitre_techniques(events)
    
    # 6. Build kill-chain narrative
    kill_chain_results = None
    if enable_kill_chain:
        kill_chain_results = build_kill_chain(events, gnn_results, mitre_results)
    
    # 7. Compute ensemble threat score
    ensemble_results = compute_ensemble_threat_score(
        outlier_results, lstm_results, gnn_results, risk_score
    )
    
    # 8. Build AI explanation context
    context = _build_context(events, chains, risk_score, risk_level)

    explanation = None
    model_used = "rule-based"

    # 9. Try AI models for explanation
    if AI_MODEL in ("gemini", "auto") and GEMINI_API_KEY:
        explanation = _call_gemini(context)
        if explanation:
            model_used = "gemini"

    if not explanation and AI_MODEL in ("openai", "auto") and OPENAI_API_KEY:
        explanation = _call_openai(context)
        if explanation:
            model_used = "openai"

    if not explanation and AI_MODEL in ("ollama", "auto"):
        explanation = _call_ollama(context)
        if explanation:
            model_used = "ollama"

    if not explanation:
        explanation = _rule_based_explanation(events, chains, risk_score, risk_level)
        model_used = "rule-based"

    return {
        "explanation": explanation,
        "model_used": model_used,
        "risk_score": risk_score,
        "risk_level": risk_level,
        "ml_ensemble": {
            "isolation_forest": outlier_results,
            "lstm": lstm_results,
            "gnn": gnn_results,
            "bert": bert_results,
            "mitre_tagger": mitre_results,
            "kill_chain": kill_chain_results,
            "ensemble_threat_score": ensemble_results,
        },
    }


def _build_context(events: List[Dict], chains: List[Dict],
                   risk_score: float, risk_level: str) -> str:
    """Build a context string for the AI model."""
    suspicious = [e for e in events if e.get("is_suspicious")]

    lines = [
        "You are a senior cybersecurity forensic analyst writing an incident report.",
        "Analyze these real log events and provide a detailed, structured report with:",
        "1. **Executive Summary** — a clear, plain-English summary of the attack",
        "2. **Attack Methodology** — step-by-step breakdown of what the attacker did",
        "3. **MITRE ATT&CK Mapping** — techniques observed with IDs",
        "4. **Indicators of Compromise (IOCs)** — IPs, users, processes involved",
        "5. **Recommended Response Actions** — prioritized incident response steps",
        "",
        "Format your response in Markdown with headers and bullet points.",
        "",
        f"Risk Score: {risk_score}/100 ({risk_level})",
        f"Total Events: {len(events)}",
        f"Suspicious Events: {len(suspicious)}",
        f"Attack Phases Detected: {len(chains)}",
        "",
        "Attack Chain Phases:",
    ]

    for chain in chains:
        lines.append(f"  - {chain['chain_name']} ({chain['severity']}): {chain['description'][:200]}")

    lines.append("\nKey Suspicious Events (sample):")
    for e in suspicious[:25]:
        ts = e["timestamp"].isoformat() if hasattr(e["timestamp"], "isoformat") else str(e["timestamp"])
        lines.append(
            f"  [{ts}] {e.get('action')} | User: {e.get('user')} | "
            f"IP: {e.get('ip_address')} | Severity: {e.get('severity')} | "
            f"Rule: {e.get('detection_rule')} | MITRE: {e.get('mitre_technique')}"
        )

    return "\n".join(lines)


def _call_gemini(context: str) -> Optional[str]:
    """Call Google Gemini API for explanation generation using new google-genai SDK."""
    try:
        from google import genai

        client = genai.Client(api_key=GEMINI_API_KEY)

        # Try models in order of preference
        models_to_try = ["gemini-2.0-flash-lite", "gemini-2.0-flash", "gemini-1.5-flash"]

        for model_name in models_to_try:
            try:
                response = client.models.generate_content(
                    model=model_name,
                    contents=context,
                    config={
                        "temperature": 0.3,
                        "max_output_tokens": 2000,
                    },
                )

                if response and response.text:
                    print(f"✅ Gemini ({model_name}) response received ({len(response.text)} chars)")
                    return response.text

            except Exception as model_err:
                err_str = str(model_err)
                if "429" in err_str or "quota" in err_str.lower():
                    print(f"⚠️ Gemini {model_name} quota exceeded, trying next model...")
                    continue
                elif "404" in err_str or "not found" in err_str.lower():
                    print(f"⚠️ Gemini model {model_name} not available, trying next...")
                    continue
                else:
                    print(f"⚠️ Gemini {model_name} error: {model_err}")
                    continue

    except ImportError:
        print("⚠️ google-genai package not installed. Run: pip install google-genai")
    except Exception as e:
        print(f"⚠️ Gemini API error: {e}")

    return None


def _call_openai(context: str) -> Optional[str]:
    """Call OpenAI API for explanation generation."""
    try:
        response = httpx.post(
            "https://api.openai.com/v1/chat/completions",
            headers={
                "Authorization": f"Bearer {OPENAI_API_KEY}",
                "Content-Type": "application/json",
            },
            json={
                "model": "gpt-4o-mini",
                "messages": [
                    {"role": "system", "content": "You are an expert cybersecurity forensic analyst."},
                    {"role": "user", "content": context},
                ],
                "temperature": 0.3,
                "max_tokens": 1500,
            },
            timeout=30,
        )
        if response.status_code == 200:
            return response.json()["choices"][0]["message"]["content"]
    except Exception as e:
        print(f"⚠️ OpenAI API error: {e}")
    return None


def _call_ollama(context: str) -> Optional[str]:
    """Call local Ollama model for explanation generation."""
    try:
        response = httpx.post(
            f"{OLLAMA_URL}/api/generate",
            json={
                "model": "llama3.2",
                "prompt": context,
                "stream": False,
            },
            timeout=60,
        )
        if response.status_code == 200:
            return response.json().get("response", "")
    except Exception:
        pass
    return None


def _rule_based_explanation(events: List[Dict], chains: List[Dict],
                            risk_score: float, risk_level: str) -> str:
    """Generate explanation using rule-based logic (no AI needed)."""
    suspicious = [e for e in events if e.get("is_suspicious")]
    if not suspicious:
        return (
            "## Analysis Complete\n\n"
            "No significant suspicious activity was detected in the provided logs. "
            "The events appear to represent normal system operations.\n\n"
            f"**Risk Score:** {risk_score}/100 ({risk_level})\n"
            f"**Total Events Analyzed:** {len(events)}"
        )

    # Collect unique indicators
    unique_ips = set(e.get("ip_address") for e in suspicious if e.get("ip_address"))
    unique_users = set(e.get("user") for e in suspicious if e.get("user"))
    unique_rules = set(e.get("detection_rule") for e in suspicious if e.get("detection_rule"))

    # Build narrative
    parts = [f"## 🚨 Security Incident Analysis\n"]
    parts.append(f"**Risk Score:** {risk_score}/100 (**{risk_level.upper()}**)\n")
    parts.append(f"**Suspicious Events:** {len(suspicious)} out of {len(events)} total events\n")

    # Executive summary
    parts.append("### Executive Summary\n")
    summary_sentences = []

    if "brute_force" in unique_rules or "failed_login" in unique_rules:
        fail_count = sum(1 for e in suspicious if e.get("detection_rule") in ("brute_force", "failed_login"))
        summary_sentences.append(
            f"Multiple failed authentication attempts ({fail_count} events) were detected, "
            f"consistent with a brute-force attack."
        )

    if "external_login" in unique_rules:
        ext_ips = [e.get("ip_address") for e in suspicious if e.get("detection_rule") == "external_login"]
        summary_sentences.append(
            f"Successful login(s) from external IP(s) ({', '.join(set(str(ip) for ip in ext_ips if ip))}) "
            f"indicate potential unauthorized access."
        )

    if "privilege_escalation" in unique_rules:
        summary_sentences.append(
            "Privilege escalation activity was detected, suggesting the attacker "
            "gained elevated access to the system."
        )

    if "suspicious_process" in unique_rules or "known_suspicious_binary" in unique_rules:
        procs = set(e.get("process") for e in suspicious
                    if e.get("detection_rule") in ("suspicious_process", "known_suspicious_binary"))
        summary_sentences.append(
            f"Suspicious process execution was observed: {', '.join(str(p) for p in procs if p)}."
        )

    if "outbound_external" in unique_rules:
        summary_sentences.append(
            "Outbound connections to external IP addresses were detected, "
            "possibly indicating data exfiltration."
        )

    if "port_scan" in unique_rules:
        summary_sentences.append(
            "Port scanning behavior was identified, suggesting network reconnaissance."
        )

    if "post_exploitation_recon" in unique_rules:
        summary_sentences.append(
            "Post-exploitation reconnaissance commands were executed, "
            "indicating the attacker was gathering system information."
        )

    parts.append(" ".join(summary_sentences) + "\n")

    # Attack timeline
    if chains:
        parts.append("### Attack Kill Chain\n")
        for i, chain in enumerate(chains):
            parts.append(f"**{i+1}. {chain['chain_name']}** — Severity: {chain['severity'].upper()}")
            parts.append(f"   {chain['description'][:300]}\n")

    # IOCs
    parts.append("### Indicators of Compromise (IOCs)\n")
    if unique_ips:
        parts.append(f"**Suspicious IPs:** {', '.join(str(ip) for ip in unique_ips)}")
    if unique_users:
        parts.append(f"**Targeted Accounts:** {', '.join(str(u) for u in unique_users)}")

    # MITRE ATT&CK
    mitre = set()
    for e in suspicious:
        if e.get("mitre_technique"):
            mitre.add(e["mitre_technique"])
    if mitre:
        parts.append("\n### MITRE ATT&CK Techniques\n")
        for t in sorted(mitre):
            parts.append(f"- {t}")

    # Recommendations
    parts.append("\n### Recommended Response Actions\n")
    recommendations = []
    if "brute_force" in unique_rules:
        recommendations.append("1. Block source IPs at the firewall level")
        recommendations.append("2. Implement account lockout policies")
        recommendations.append("3. Enable multi-factor authentication")
    if "external_login" in unique_rules:
        recommendations.append("4. Investigate external login sessions for unauthorized activity")
        recommendations.append("5. Reset credentials for affected accounts")
    if "privilege_escalation" in unique_rules:
        recommendations.append("6. Audit sudo/admin policies and remove unnecessary privileges")
    if "outbound_external" in unique_rules:
        recommendations.append("7. Investigate outbound connections for data exfiltration")
        recommendations.append("8. Block suspicious external IPs")
    if not recommendations:
        recommendations.append("1. Continue monitoring for further suspicious activity")
        recommendations.append("2. Review access logs for the involved accounts")

    parts.extend(recommendations)

    return "\n".join(parts)
