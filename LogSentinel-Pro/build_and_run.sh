#!/bin/bash

# ═══════════════════════════════════════════════════════
#  LogSentinel Pro v3.0 — Build & Launch
#  C++ Qt6 UI + C++ Log Parser + Python AI Backend
# ═══════════════════════════════════════════════════════

APP_DIR="$(cd "$(dirname "$0")" && pwd)"
echo "╔══════════════════════════════════════════════════════╗"
echo "║       LogSentinel Pro v3.0 — Enterprise SIEM        ║"
echo "║       C++ Qt6 UI + Native Log Engine                ║"
echo "╚══════════════════════════════════════════════════════╝"

# Setup Python backend for AI (minimal)
if [ -d "$APP_DIR/venv" ]; then
    source "$APP_DIR/venv/bin/activate"
    echo "[+] Python venv activated (for AI backend only)."
else
    echo "[*] Creating Python venv for AI backend..."
    python3 -m venv "$APP_DIR/venv"
    source "$APP_DIR/venv/bin/activate"
    pip install google-genai httpx python-dotenv 2>/dev/null
fi

# Build C++ Qt6 Application
echo "[*] Building C++ Qt6 UI + Log Engine..."
cd "$APP_DIR/src/ui"
mkdir -p build
cd build

qmake6 ../logsentinel.pro
if [ $? -ne 0 ]; then
    echo "[!] qmake6 failed. Ensure qt6-base-dev is installed."
    exit 1
fi

make -j$(nproc)
if [ $? -ne 0 ]; then
    echo "[!] C++ compilation failed."
    exit 1
fi

echo "[+] Build successful."
echo "[*] Launching LogSentinel Pro..."
./logsentinel_pro
