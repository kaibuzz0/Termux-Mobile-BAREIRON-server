#!/usr/bin/env bash
#
# BAREIRON QUICKSTART
# One-command build and run for the lazy
# Usage: ./quickstart.sh
#
set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR"

echo "═════════════════════════════════════════════════════════════"
echo "  🧟 BAREIRON — Quick Start"
echo "═════════════════════════════════════════════════════════════"

# ── Check registries ─────────────────────────────────────────
if [ ! -f "include/registries.h" ]; then
    echo "[BUILD] Registry data missing. Extracting from Minecraft server..."
    
    # Check for Java
    if ! command -v java >/dev/null 2>&1; then
        echo "[ERROR] Java not found. Install OpenJDK 21+ and retry."
        echo "        Linux: sudo apt install openjdk-21-jdk"
        echo "        Termux: pkg install openjdk-21"
        exit 1
    fi
    
    # Check Java version
    JAVA_VER=$(java -version 2>&1 | head -1 | grep -oP '\d+' | head -1)
    if [ "$JAVA_VER" -lt 21 ]; then
        echo "[WARN] Java $JAVA_VER detected. Need Java 21+."
        echo "       Attempting to use /tmp/jdk-21 if available..."
        if [ -x "/tmp/jdk-21.0.6+7/bin/java" ]; then
            export JAVA_HOME=/tmp/jdk-21.0.6+7
            export PATH="$JAVA_HOME/bin:$PATH"
            echo "[BUILD] Using Java 21 at $JAVA_HOME"
        else
            echo "[ERROR] Java 21+ required. Install from https://adoptium.net/"
            exit 1
        fi
    fi
    
    # Check for server JAR
    if [ ! -f "notchian/server.jar" ]; then
        echo "[BUILD] Downloading Minecraft 1.21.8 server JAR..."
        mkdir -p notchian
        curl -L -o notchian/server.jar \
            "https://piston-data.mojang.com/v1/objects/6bce4ef400e4efaa63a13d5e6f6b500be969ef81/server.jar"
    fi
    
    # Check for node
    if ! command -v node >/dev/null 2>&1; then
        echo "[ERROR] Node.js not found. Install it and retry."
        echo "        Linux: sudo apt install nodejs"
        echo "        Termux: pkg install nodejs"
        exit 1
    fi
    
    # Extract registries
    echo "[BUILD] Extracting registry data..."
    cd notchian
    java -DbundlerMainClass="net.minecraft.data.Main" -jar server.jar --all 2>/dev/null || true
    cd "$DIR"
    
    # Build registries
    echo "[BUILD] Building registry headers..."
    node build_registries.js
    echo "[BUILD] Registry headers generated."
fi

# ── Compile ──────────────────────────────────────────────────
echo "[BUILD] Compiling bareiron..."
rm -f bareiron

if gcc src/*.c -O2 -Iinclude -o bareiron -lm 2> build.err; then
    echo "[BUILD] ✅ Compilation successful."
    rm -f build.err
else
    echo "[BUILD] ❌ Compilation failed:"
    cat build.err
    rm -f build.err
    exit 1
fi

# ── Show info ───────────────────────────────────────────────
echo ""
echo "═════════════════════════════════════════════════════════════"
echo "  ✅ SERVER READY"
echo "═════════════════════════════════════════════════════════════"
echo ""
echo "  Binary size: $(ls -lh bareiron | awk '{print $5}')"
echo "  Port:        25565"
echo "  Protocol:    772 (Minecraft 1.21.8)"
echo ""
echo "  Connect with:"
echo "    • Minecraft Java Edition 1.21.8"
echo "    • IP: $(hostname -I 2>/dev/null | awk '{print $1}' || echo 'your_server_ip')"
echo "    • Port: 25565"
echo ""
echo "  Or test locally:"
echo "    python3 -c \"import socket,json;s=socket.socket();s.connect(('127.0.0.1',25565));...\""
echo ""
echo "═════════════════════════════════════════════════════════════"
echo "  Press Ctrl+C to stop"
echo "═════════════════════════════════════════════════════════════"
echo ""

# ── Run ──────────────────────────────────────────────────────
./bareiron
