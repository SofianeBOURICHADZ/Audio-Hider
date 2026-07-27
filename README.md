Messages Hider in Audio.
Want to send a message inside an audio file hidden and encrypted? This is the right tool for you.
You load an audio file. You set up your password and then enter message. You hit encrypt and done. MAKE SURE TO SAVE YOUR KEYS AND PIN CODE OR ELSE YOU WILL LOSE THE HIDDEN DATA FOREVER.
It uses QT6 and also a secure way to encrypt.
Feel free to upgrade its security or functionality.

## 🔨 Building & Running from Source

### Prerequisites
Make sure you have CMake, a C++17/20 compiler, and Qt6 installed:

* **Arch Linux / CachyOS:**
  ```bash
  sudo pacman -S cmake gcc qt6-base

 * **UBUNTU / DIBIAN:**
```bash
sudo apt update
sudo apt install cmake g++ qt6-base-dev
```
* **FEDORA::**
```bash
sudo dnf install cmake gcc-c++ qt6-qtbase-devel
```

*  **BUILD INSTRUCTIONS:**
  ```bash
# 1. Clone the repository
git clone [https://github.com/YOUR_USERNAME/AudioStegoTool.git](https://github.com/YOUR_USERNAME/AudioStegoTool.git)
cd AudioStegoTool

# 2. Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# 3. Launch
./build/AudioStegoTool
```
