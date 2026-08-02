#!/bin/bash
# Android Build Script for BAREIRON Content Manager
# Builds APK with Gradle wrapper (or system gradle)

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR/android/BareironApp"

echo "═════════════════════════════════════════════════"
echo "  🧟 BAREIRON Android Content Manager Builder"
echo "═════════════════════════════════════════════════"

if [ ! -d "$PROJECT_DIR" ]; then
    echo "❌ Project not found at $PROJECT_DIR"
    exit 1
fi

cd "$PROJECT_DIR"

# Check for gradle
if command -v gradle >/dev/null 2>&1; then
    GRADLE="gradle"
elif [ -f "./gradlew" ]; then
    GRADLE="./gradlew"
    chmod +x gradlew
elif [ -f "../gradlew" ]; then
    GRADLE="../gradlew"
    chmod +x ../gradlew
else
    echo "⚠️  No Gradle found. Installing wrapper..."
    mkdir -p gradle/wrapper
    cat > gradle/wrapper/gradle-wrapper.properties <<'EOF'
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-8.1-bin.zip
networkTimeout=10000
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
EOF
    
    # Download wrapper jar
    WRAPPER_URL="https://raw.githubusercontent.com/gradle/gradle/v8.1.0/gradle/wrapper/gradle-wrapper.jar"
    curl -sL "$WRAPPER_URL" -o gradle/wrapper/gradle-wrapper.jar 2>/dev/null || true
    
    cat > gradlew <<'EOF'
#!/bin/bash
cd "$(dirname "$0")"
exec java -cp gradle/wrapper/gradle-wrapper.jar org.gradle.wrapper.GradleWrapperMain "$@"
EOF
    chmod +x gradlew
    GRADLE="./gradlew"
fi

echo "📦 Building APK with: $GRADLE"

# Build debug APK
$GRADLE assembleDebug || {
    echo "❌ Build failed"
    exit 1
}

APK_PATH="app/build/outputs/apk/debug/app-debug.apk"
if [ -f "$APK_PATH" ]; then
    SIZE=$(ls -lh "$APK_PATH" | awk '{print $5}')
    echo ""
    echo "✅ BUILD SUCCESS"
    echo "═════════════════════════════════════════════════"
    echo "  APK: $APK_PATH"
    echo "  Size: $SIZE"
    echo ""
    echo "  Install on Android:"
    echo "    adb install -r $APK_PATH"
    echo ""
    echo "  Or copy to phone and install manually"
    echo "═════════════════════════════════════════════════"
else
    echo "❌ APK not found after build"
    exit 1
fi
