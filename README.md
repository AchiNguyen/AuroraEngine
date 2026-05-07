# AuroraEngine

A real-time C++20 rendering engine. Stage 1 is the platform layer: GLFW window, OpenGL 4.6 core context, GLAD loader, spdlog diagnostics.

## Prerequisites

- **Windows 10/11** with a GPU + drivers supporting **OpenGL 4.6**.
- **Visual Studio 2022** (17.8+) with the *Desktop development with C++* workload — supplies MSVC, CMake, and Ninja.
- **Git** (for cloning vcpkg).
- **vcpkg**, bootstrapped somewhere persistent (e.g. `C:\dev\vcpkg`).

### One-time vcpkg setup

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\dev\vcpkg
C:\dev\vcpkg\bootstrap-vcpkg.bat
[Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\dev\vcpkg", "User")
```

Restart your shell so `VCPKG_ROOT` is visible. Visual Studio's CMake integration auto-detects vcpkg manifest mode when `VCPKG_ROOT` is set and `vcpkg.json` exists at the project root.

## Build with Visual Studio (Open Folder)

1. Launch Visual Studio 2022.
2. **File → Open → Folder…** and select the `AuroraEngine` folder.
3. Wait for CMake configuration to finish — vcpkg will fetch and build `glfw3`, `glad`, `glm`, and `spdlog` on the first run (a few minutes).
4. In the configuration dropdown pick **x64-Debug** (or **x64-Release**).
5. **Build → Build All** (`Ctrl+Shift+B`).
6. Set **AuroraEngine.exe** as the startup item, then **Debug → Start Debugging** (`F5`) or **Start Without Debugging** (`Ctrl+F5`).

A 1280x720 window titled *Aurora — Stage 1* should appear, cleared to dark blue (`#0a0e27`). ESC or the close button exits cleanly. Startup logs (GL version, GLSL version, vendor, renderer) are printed to the Output window.

## Build from PowerShell (CLI)

If `VCPKG_ROOT` is set:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake"
cmake --build build --config Debug
.\build\bin\Debug\AuroraEngine.exe
```

## Project layout

```
AuroraEngine/
|-- CMakeLists.txt
|-- vcpkg.json
|-- include/aurora/   # public headers
|-- src/              # implementation
|-- shaders/          # GLSL (future stages)
|-- assets/           # textures, models (future stages)
|-- vendor/           # third-party drop-in code (future)
|-- build/            # CMake output (gitignored)
```

## Stage 1 scope

- RAII GLFW init + window creation
- OpenGL 4.6 core profile, forward-compat, 4x MSAA
- GLAD function loading
- GLFW error callback -> spdlog::error
- `glDebugMessageCallback` -> spdlog (severity-mapped)
- Render loop: clear screen, swap, poll
- ESC / close button shutdown

No gameplay or rendering primitives yet — that lands in later stages.
