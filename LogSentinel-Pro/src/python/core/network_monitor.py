"""
Network Monitor Module
Provides real-time network connection tracking using psutil.
"""
import psutil
import socket
from collections import Counter
from typing import List, Dict

def get_active_connections() -> List[Dict]:
    """Get all active network connections with resolved process names."""
    connections = []
    try:
        for conn in psutil.net_connections(kind='inet'):
            try:
                proc = psutil.Process(conn.pid) if conn.pid else None
                proc_name = proc.name() if proc else "unknown"
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                proc_name = "unknown"

            local = f"{conn.laddr.ip}:{conn.laddr.port}" if conn.laddr else "-"
            remote = f"{conn.raddr.ip}:{conn.raddr.port}" if conn.raddr else "-"
            
            connections.append({
                "pid": conn.pid or 0,
                "process": proc_name,
                "local": local,
                "remote": remote,
                "status": conn.status,
                "type": "TCP" if conn.type == socket.SOCK_STREAM else "UDP",
            })
    except psutil.AccessDenied:
        pass
    return connections

def get_listening_ports() -> List[Dict]:
    """Get all listening ports."""
    listeners = []
    try:
        for conn in psutil.net_connections(kind='inet'):
            if conn.status == 'LISTEN':
                try:
                    proc = psutil.Process(conn.pid) if conn.pid else None
                    proc_name = proc.name() if proc else "unknown"
                except (psutil.NoSuchProcess, psutil.AccessDenied):
                    proc_name = "unknown"
                    
                listeners.append({
                    "port": conn.laddr.port,
                    "process": proc_name,
                    "pid": conn.pid or 0,
                    "address": conn.laddr.ip,
                })
    except psutil.AccessDenied:
        pass
    return listeners

def get_connection_summary() -> Dict:
    """Get summary statistics of network connections."""
    try:
        conns = psutil.net_connections(kind='inet')
        status_counts = Counter(c.status for c in conns)
        return {
            "total": len(conns),
            "established": status_counts.get("ESTABLISHED", 0),
            "listening": status_counts.get("LISTEN", 0),
            "time_wait": status_counts.get("TIME_WAIT", 0),
            "close_wait": status_counts.get("CLOSE_WAIT", 0),
        }
    except psutil.AccessDenied:
        return {"total": 0, "established": 0, "listening": 0, "time_wait": 0, "close_wait": 0}

def get_system_info() -> Dict:
    """Get comprehensive system information."""
    cpu_freq = psutil.cpu_freq()
    disk = psutil.disk_usage('/')
    net_io = psutil.net_io_counters()
    boot_time = psutil.boot_time()
    
    return {
        "cpu_count": psutil.cpu_count(),
        "cpu_freq_mhz": round(cpu_freq.current, 0) if cpu_freq else 0,
        "ram_total_gb": round(psutil.virtual_memory().total / (1024**3), 2),
        "ram_used_gb": round(psutil.virtual_memory().used / (1024**3), 2),
        "disk_total_gb": round(disk.total / (1024**3), 2),
        "disk_used_gb": round(disk.used / (1024**3), 2),
        "disk_percent": disk.percent,
        "net_sent_mb": round(net_io.bytes_sent / (1024**2), 2),
        "net_recv_mb": round(net_io.bytes_recv / (1024**2), 2),
        "boot_time": boot_time,
    }
