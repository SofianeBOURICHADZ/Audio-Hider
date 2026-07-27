#!/usr/bin/env bash
set -euo pipefail

# 1. Setup Directories
APP_DIR="AppDir"
OUTPUT_DIR="dist"
rm -rf "$APP_DIR" "$OUTPUT_DIR"
mkdir -p "$APP_DIR/usr/bin" \
         "$APP_DIR/usr/lib" \
         "$APP_DIR/usr/plugins/platforms" \
         "$APP_DIR/usr/share/icons/hicolor/256x256/apps" \
         "$OUTPUT_DIR"

# 2. Download linuxdeploy tools (Qt plugin omitted)
if [ ! -f "linuxdeploy-x86_64.AppImage" ]; then
    echo "Downloading linuxdeploy..."
    wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    chmod +x linuxdeploy-x86_64.AppImage
fi

if [ ! -f "linuxdeploy-plugin-appimage-x86_64.AppImage" ]; then
    echo "Downloading linuxdeploy appimage plugin..."
    wget -q https://github.com/linuxdeploy/linuxdeploy-plugin-appimage/releases/download/continuous/linuxdeploy-plugin-appimage-x86_64.AppImage
    chmod +x linuxdeploy-plugin-appimage-x86_64.AppImage
fi

# 3. Copy Application Assets
EXECUTABLE="build/AudioStegoTool"
DESKTOP_FILE="assets/AudioStegoTool.desktop"
ICON_FILE="assets/application.png"

cp "$EXECUTABLE" "$APP_DIR/usr/bin/"
cp "$DESKTOP_FILE" "$APP_DIR/"
cp "$ICON_FILE" "$APP_DIR/usr/share/icons/hicolor/256x256/apps/"

# 4. Deploy dependencies without the Qt plugin
echo "Deploying application dependencies..."
NO_STRIP=1 ./linuxdeploy-x86_64.AppImage \
    --appdir "$APP_DIR" \
    --desktop-file "$DESKTOP_FILE" \
    --icon-file "$ICON_FILE"

# 5. Copy Qt Platform Plugins (xcb) so the GUI can initialize
echo "Copying Qt6 XCB platform plugin..."
QT_PLATFORM_PATH=$(find /usr/lib /usr/lib64 -name "libqxcb.so" 2>/dev/null | head -n 1)
if [ -n "$QT_PLATFORM_PATH" ]; then
    cp "$QT_PLATFORM_PATH" "$APP_DIR/usr/plugins/platforms/"
else
    echo "Warning: libqxcb.so not found automatically in standard system library paths!"
fi

# 6. Safely strip binaries using system binutils
echo "Safely stripping binaries with system strip..."
find "$APP_DIR" -type f \( -name "*.so*" -o -perm /111 \) -exec strip --strip-unneeded {} + 2>/dev/null || true

# 7. Package AppImage
echo "Building final AppImage..."
./linuxdeploy-plugin-appimage-x86_64.AppImage --appdir "$APP_DIR"

mv *.AppImage "$OUTPUT_DIR/" 2>/dev/null || true
echo "Build complete! Output saved in: $OUTPUT_DIR/"