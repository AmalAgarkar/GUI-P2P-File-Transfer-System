#!/bin/bash
set -e
echo "=== P2P Qt GUI Build Script ==="

# ── 1. Install dependencies ──────────────────────────────────────────────────
echo "[1/4] Installing dependencies..."
sudo apt update -qq
sudo apt install -y \
    cmake ninja-build \
    qt6-base-dev qt6-base-private-dev \
    libqt6core6 libqt6widgets6 libqt6network6 \
    libssl-dev \
    build-essential

# ── 2. Configure ─────────────────────────────────────────────────────────────
echo "[2/4] Configuring with CMake..."
mkdir -p build
cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release

# ── 3. Build ─────────────────────────────────────────────────────────────────
echo "[3/4] Building..."
cmake --build build --parallel

echo ""
echo "=== Build complete! ==="
echo "Binaries:"
echo "  Tracker: ./build/tracker/p2p_tracker"
echo "  Client:  ./build/client/p2p_client"
echo ""

# ── 4. Launch ────────────────────────────────────────────────────────────────
echo "[4/4] Launching..."
echo "Starting Tracker in background..."
./build/tracker/p2p_tracker &
TRACKER_PID=$!
sleep 1

echo "Starting Client..."
./build/client/p2p_client

# Cleanup tracker on exit
kill $TRACKER_PID 2>/dev/null || true
