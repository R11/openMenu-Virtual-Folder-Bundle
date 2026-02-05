#!/bin/bash
# Script to create proper macOS .app bundle from dotnet publish output
# Usage: ./create-macos-bundle.sh <publish_output_dir> <version> <output_dir>

set -e

PUBLISH_DIR=$1
VERSION=$2
OUTPUT_DIR=$3

if [ -z "$PUBLISH_DIR" ] || [ -z "$VERSION" ] || [ -z "$OUTPUT_DIR" ]; then
    echo "Usage: $0 <publish_output_dir> <version> <output_dir>"
    exit 1
fi

APP_NAME="GDMENUCardManager"
BUNDLE_NAME="${APP_NAME}.app"
BUNDLE_PATH="${OUTPUT_DIR}/${BUNDLE_NAME}"

echo "Creating macOS app bundle: ${BUNDLE_NAME}"
echo "Version: ${VERSION}"

# Create the app bundle structure
mkdir -p "${BUNDLE_PATH}/Contents/MacOS"
mkdir -p "${BUNDLE_PATH}/Contents/Resources"

# Copy all published files to Contents/MacOS
echo "Copying application files..."
cp -r "${PUBLISH_DIR}"/* "${BUNDLE_PATH}/Contents/MacOS/"

# Copy Info.plist and update version
echo "Creating Info.plist..."
if [ -f "src/${APP_NAME}.AvaloniaUI/Info.plist" ]; then
    cp "src/${APP_NAME}.AvaloniaUI/Info.plist" "${BUNDLE_PATH}/Contents/Info.plist"

    # Update version in Info.plist if VERSION is provided
    if [ "$(uname)" == "Darwin" ]; then
        # macOS sed syntax
        sed -i '' "s/<string>1.0<\/string>/<string>${VERSION}<\/string>/g" "${BUNDLE_PATH}/Contents/Info.plist"
        sed -i '' "s/<string>1.0.0<\/string>/<string>${VERSION}.0<\/string>/g" "${BUNDLE_PATH}/Contents/Info.plist"
    else
        # Linux sed syntax
        sed -i "s/<string>1.0<\/string>/<string>${VERSION}<\/string>/g" "${BUNDLE_PATH}/Contents/Info.plist"
        sed -i "s/<string>1.0.0<\/string>/<string>${VERSION}.0<\/string>/g" "${BUNDLE_PATH}/Contents/Info.plist"
    fi
else
    echo "Warning: Info.plist template not found at src/${APP_NAME}.AvaloniaUI/Info.plist"
fi

# Make the executable actually executable
echo "Setting executable permissions..."
chmod +x "${BUNDLE_PATH}/Contents/MacOS/${APP_NAME}"

# If icon file exists, copy it to Resources
if [ -f "src/${APP_NAME}.AvaloniaUI/Assets/icon.icns" ]; then
    cp "src/${APP_NAME}.AvaloniaUI/Assets/icon.icns" "${BUNDLE_PATH}/Contents/Resources/"
    echo "Icon file copied."
else
    echo "Warning: Icon file not found at src/${APP_NAME}.AvaloniaUI/Assets/icon.icns"
fi

echo "macOS app bundle created successfully at: ${BUNDLE_PATH}"
echo "Structure:"
find "${BUNDLE_PATH}" -maxdepth 3 -type d

# Create a tar.gz archive
echo "Creating tar.gz archive..."
cd "${OUTPUT_DIR}"
tar -czf "${APP_NAME}.${VERSION}-osx-x64-AppBundle.tar.gz" "${BUNDLE_NAME}"
cd - > /dev/null

echo "Archive created: ${OUTPUT_DIR}/${APP_NAME}.${VERSION}-osx-x64-AppBundle.tar.gz"
echo "Done!"
