#!/usr/bin/env bash
set -euo pipefail


BUILD_DIR="build"
APP_DIR="AppDir"
OUTPUT_DIR="dist"

EXECUTABLE_NAME="AudioStegoTool"
EXECUTABLE_PATH="$BUILD_DIR/$EXECUTABLE_NAME"
DESKTOP_FILE="assets/AudioStegoTool.desktop"
ICON_FILE="assets/application.png"

echo "==> Cleaning old build artifacts..."
rm -rf "$BUILD_DIR" "$APP_DIR" "$OUTPUT_DIR"
mkdir -p "$BUILD_DIR" \
         "$APP_DIR/usr/bin" \
         "$APP_DIR/usr/lib" \
         "$APP_DIR/usr/plugins/platforms" \
         "$APP_DIR/usr/share/icons/hicolor/256x256/apps" \
         "$OUTPUT_DIR"


echo "==> Configuring with CMake..."
cmake -B "$BUILD_DIR" -S . -DCMAKE_BUILD_TYPE=Release

echo "==> Compiling source code..."
cmake --build "$BUILD_DIR" -j"$(nproc)"

if [ ! -f "$EXECUTABLE_PATH" ]; then
    echo "Error: Executable $EXECUTABLE_PATH was not found after compilation."
    exit 1
fi

if [ ! -f "linuxdeploy-x86_64.AppImage" ]; then
    echo "==> Downloading linuxdeploy..."
    wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    chmod +x linuxdeploy-x86_64.AppImage
fi

if [ ! -f "linuxdeploy-plugin-appimage-x86_64.AppImage" ]; then
    echo "==> Downloading linuxdeploy appimage plugin..."
    wget -q https://github.com/linuxdeploy/linuxdeploy-plugin-appimage/releases/download/continuous/linuxdeploy-plugin-appimage-x86_64.AppImage
    chmod +x linuxdeploy-plugin-appimage-x86_64.AppImage
fi

echo "==> Copying binary and metadata into AppDir..."
cp "$EXECUTABLE_PATH" "$APP_DIR/usr/bin/"
cp "$DESKTOP_FILE" "$APP_DIR/"
cp "$ICON_FILE" "$APP_DIR/usr/share/icons/hicolor/256x256/apps/"

cat << 'EOF' > "$APP_DIR/usr/bin/qt.conf"
[Paths]
Prefix = ..
Plugins = plugins
EOF


echo "==> Deploying dependencies with linuxdeploy..."
NO_STRIP=1 ./linuxdeploy-x86_64.AppImage \
    --appdir "$APP_DIR" \
    --desktop-file "$DESKTOP_FILE" \
    --icon-file "$ICON_FILE"

echo "==> Copying Qt6 XCB platform plugin..."
QT_PLATFORM_PATH=$(find /usr/lib /usr/lib64 -name "libqxcb.so" 2>/dev/null | head -n 1)
if [ -n "$QT_PLATFORM_PATH" ]; then
    cp "$QT_PLATFORM_PATH" "$APP_DIR/usr/plugins/platforms/"
else
    echo "Warning: libqxcb.so not found automatically in system library paths!"
fi

echo "==> Safely stripping binaries with system strip..."
find "$APP_DIR" -type f \( -name "*.so*" -o -perm /111 \) -exec strip --strip-unneeded {} + 2>/dev/null || true

echo "==> Building final AppImage..."
./linuxdeploy-plugin-appimage-x86_64.AppImage --appdir "$APP_DIR"

mv *.AppImage "$OUTPUT_DIR/" 2>/dev/null || true

echo "----------------------------------------------------"
echo "Build complete! AppImage saved in: $OUTPUT_DIR/"
echo "----------------------------------------------------"