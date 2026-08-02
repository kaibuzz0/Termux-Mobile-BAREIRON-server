#!/usr/bin/env bash
#
# BAREIRON INSTALL SCRIPT
# One-command setup: downloads Java 21 if needed, extracts registries, builds server
# Usage: ./install.sh
#
set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR"

echo "═════════════════════════════════════════════════════════════"
echo "  🧟 BAREIRON INSTALLER"
echo "═════════════════════════════════════════════════════════════"
echo ""

# ── Detect platform ─────────────────────────────────────────
ARCH=$(uname -m)
OS=$(uname -s)
echo "[INFO] Platform: $OS $ARCH"

# ── Check gcc ────────────────────────────────────────────────
if ! command -v gcc >/dev/null 2>&1; then
    echo "[ERROR] gcc not found. Install it first:"
    echo "        Debian/Ubuntu: sudo apt install gcc"
    echo "        Termux:        pkg install gcc"
    exit 1
fi
echo "[OK] gcc found"

# ── Check node ───────────────────────────────────────────────
if ! command -v node >/dev/null 2>&1; then
    echo "[ERROR] Node.js not found. Install it first:"
    echo "        Debian/Ubuntu: sudo apt install nodejs"
    echo "        Termux:        pkg install nodejs"
    exit 1
fi
echo "[OK] Node.js found"

# ── Check Java ─────────────────────────────────────────────
NEED_JAVA=0
JAVA_CMD="java"

if command -v java >/dev/null 2>&1; then
    JAVA_VER=$(java -version 2>&1 | head -1 | grep -oP '\d+' | head -1)
    if [ "$JAVA_VER" -ge 21 ]; then
        echo "[OK] Java $JAVA_VER found"
    else
        echo "[WARN] Java $JAVA_VER is too old (need 21+)"
        NEED_JAVA=1
    fi
else
    echo "[WARN] Java not found"
    NEED_JAVA=1
fi

# Download Java 21 if needed
if [ "$NEED_JAVA" -eq 1 ]; then
    echo ""
    echo "[JAVA] Downloading OpenJDK 21 (Temurin)..."
    
    case "$ARCH" in
        aarch64|arm64)
            JDK_URL="https://github.com/adoptium/temurin21-binaries/releases/download/jdk-21.0.6%2B7/OpenJDK21U-jdk_aarch64_linux_hotspot_21.0.6_7.tar.gz"
            ;;
        x86_64|amd64)
            JDK_URL="https://github.com/adoptium/temurin21-binaries/releases/download/jdk-21.0.6%2B7/OpenJDK21U-jdk_x64_linux_hotspot_21.0.6_7.tar.gz"
            ;;
        *)
            echo "[ERROR] Unsupported architecture: $ARCH"
            echo "        Install Java 21 manually from https://adoptium.net/"
            exit 1
            ;;
    esac
    
    JDK_TAR="/tmp/openjdk21.tar.gz"
    JDK_DIR="/tmp/jdk-21.0.6+7"
    
    if [ ! -d "$JDK_DIR" ]; then
        if [ ! -f "$JDK_TAR" ]; then
            echo "[JAVA] Downloading from Eclipse Adoptium..."
            curl -L -o "$JDK_TAR" "$JDK_URL"
        fi
        echo "[JAVA] Extracting..."
        tar -xzf "$JDK_TAR" -C /tmp
    fi
    
    export JAVA_HOME="$JDK_DIR"
    export PATH="$JAVA_HOME/bin:$PATH"
    JAVA_CMD="$JAVA_HOME/bin/java"
    echo "[OK] Java 21 ready at $JAVA_HOME"
fi

# ── Check registries ──────────────────────────────────────
if [ ! -f "include/registries.h" ]; then
    echo ""
    echo "[BUILD] Registry headers missing. Building them now..."
    
    # Download server JAR if needed
    if [ ! -f "notchian/server.jar" ]; then
        echo "[BUILD] Downloading Minecraft 1.21.8 server JAR..."
        mkdir -p notchian
        curl -L -o notchian/server.jar \
            "https://piston-data.mojang.com/v1/objects/6bce4ef400e4efaa63a13d5e6f6b500be969ef81/server.jar"
    fi
    
    # Extract registries
    echo "[BUILD] Extracting registry data from Minecraft server..."
    cd notchian
    $JAVA_CMD -DbundlerMainClass="net.minecraft.data.Main" -jar server.jar --all 2>/dev/null || true
    cd "$DIR"
    
    # Build registry headers
    echo "[BUILD] Generating C registry headers..."
    node build_registries.js
    echo "[OK] Registry headers generated"
else
    echo "[OK] Registry headers found"
fi

# ── Compile ────────────────────────────────────────────────
echo ""
echo "[BUILD] Compiling bareiron server..."
rm -f bareiron

if gcc src/*.c -O2 -Iinclude -o bareiron -lm 2> build.err; then
    echo "[OK] ✅ Compilation successful"
    rm -f build.err
else
    echo "[ERROR] ❌ Compilation failed:"
    cat build.err
    rm -f build.err
    exit 1
fi

# ── Summary ────────────────────────────────────────────────
BINARY_SIZE=$(ls -lh bareiron | awk '{print $5}')
echo ""
echo "═════════════════════════════════════════════════════════════"
echo "  ✅ BAREIRON INSTALLED"
echo "═════════════════════════════════════════════════════════════"
echo ""
echo "  Binary:     $DIR/bareiron ($BINARY_SIZE)"
echo "  Protocol:   772 (Minecraft 1.21.8)"
echo "  Port:       25565"
echo ""
echo "  RUN THE SERVER:"
echo "    $DIR/bareiron"
echo ""
echo "  QUICK TEST (from another terminal):"
echo "    python3 $DIR/test_server.py"
echo ""
echo "  CONNECT WITH MINECRAFT JAVA 1.21.8:"
echo "    IP:   $(hostname -I 2>/dev/null | awk '{print $1}' || echo 'your_server_ip')"
echo "    Port: 25565"
echo ""
echo "═════════════════════════════════════════════════════════════"
