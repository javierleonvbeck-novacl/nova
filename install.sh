#!/bin/bash

echo "=========================================="
echo "  Installing Nova (.nv) Programming Language"
echo "=========================================="

# Check if gcc is installed
if ! command -v gcc &> /dev/null; then
    echo "[!] Error: 'gcc' is required but not found. Please install it using: sudo apt install build-essential"
    exit 1
fi

# Verify folder structure
if [ ! -d "src" ] || [ ! -d "include" ] || [ ! -d "modules" ]; then
    echo "[!] Error: Please run this script from the root directory of your Nova project folder."
    exit 1
fi

echo "[*] Compiling Nova core and active modules..."
gcc src/main.c src/environment.c src/registry.c modules/*.c -Iinclude -o nova

if [ $? -ne 0 ]; then
    echo "[!] Compilation failed. Please check your source files."
    exit 1
fi

echo "[*] Installing 'nova' globally to /usr/local/bin/..."
sudo mv nova /usr/local/bin/nova
sudo chmod +x /usr/local/bin/nova

if [ -f "nv-modules" ]; then
    echo "[*] Linking 'nv-modules' manager globally..."
    chmod +x nv-modules
    sudo ln -sf "$(pwd)/nv-modules" /usr/local/bin/nv-modules
fi

echo "=========================================="
echo " SUCCESS: Nova is successfully installed!"
echo " You can now run 'nova <script.nv>' from anywhere."
echo "=========================================="
