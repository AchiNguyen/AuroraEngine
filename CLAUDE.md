# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

C++20 real-time rendering engine targeting Windows + OpenGL 4.6 core. Currently at **Stage 1**: platform layer only (GLFW window, GL context, GLAD loader, spdlog). No rendering primitives, asset loading, or scene system yet — `shaders/`, `assets/`, `vendor/` are placeholders for later stages.

## Build

Dependencies are resolved via **vcpkg manifest mode** (`vcpkg.json`): `glfw3`, `glad` (with `gl-api-46` feature), `glm`, `spdlog`. `VCPKG_ROOT` must be set; first configure pulls + builds these and takes several minutes.

PowerShell (CLI):

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake"
cmake --build build --config Debug          # or --config Release
.\build\bin\Debug\AuroraEngine.exe
```

Visual Studio: **Open Folder** on the repo root — CMake auto-configures, pick `x64-Debug` / `x64-Release`, build and run `AuroraEngine.exe`.

There is no test suite, no linter config, and no CI. MSVC warnings are `/W4 /permissive- /Zc:preprocessor /MP`; treat warnings as something to fix, not silence.

**Debug-config define `AURORA_DEBUG`** (set automatically by CMake for Debug builds) does three things: requests a `GLFW_OPENGL_DEBUG_CONTEXT`, raises spdlog level to `debug`, and enables the `glDebugMessageCallback` path. Behavior differs between Debug and Release — verify against both when changing init or logging.

## Runtime working directory

CMake sets `VS_DEBUGGER_WORKING_DIRECTORY` to the source root, and the README's PowerShell run command launches the exe from the repo root. Any future file IO (shaders, assets) should resolve paths **relative to the project root**, not relative to the exe location, to stay consistent across both launchers.

## Architecture

Two translation units (`src/main.cpp`, `src/window.cpp`) plus one public header (`include/aurora/window.hpp`). Everything lives in namespace `aurora`. CMake adds `include/` to the include path; new headers go under `include/aurora/`.

**Construction order is load-bearing.** `aurora::GlfwContext` is an RAII wrapper around `glfwInit`/`glfwTerminate` and **must be constructed before** any `aurora::Window` and outlive it — `Window` calls GLFW APIs in its destructor. `main.cpp` shows the canonical pattern. Both classes delete copy and move (rule of 5); they own process-global / GPU resources and are not relocatable.

`Window`'s constructor does the full GL bring-up in one shot: window hints → create window → make context current → vsync on → install key/framebuffer callbacks → `gladLoadGLLoader` → log GL strings → install `glDebugMessageCallback` (only if the context actually has `GL_CONTEXT_FLAG_DEBUG_BIT`) → enable MSAA → set viewport. If you split this up, preserve the ordering — GLAD must load before any `gl*` call, and the debug callback must be installed before code that might emit messages worth seeing.

**Header hygiene:** the build defines `GLFW_INCLUDE_NONE` globally, so `glad/glad.h` must be included **before** `<GLFW/glfw3.h>` in any TU that uses both. The public `window.hpp` keeps GLFW out of its interface via a forward declaration of `GLFWwindow` — preserve that to avoid leaking GLFW into headers.

**GL debug callback** (`gl_debug_callback` in `window.cpp`) maps GL severity → spdlog level (`HIGH`→error, `MEDIUM`→warn, `LOW`→info, `NOTIFICATION`→debug) and silently drops a hardcoded set of NVIDIA notification IDs (`131169, 131185, 131218, 131204`) that are noise. Add to that filter list rather than lowering severity globally if a new driver message turns out to be spam.

## Adding source files

New `.cpp` files must be added to the `add_executable(AuroraEngine ...)` list in `CMakeLists.txt` — there is no glob. Headers in `include/aurora/` are picked up automatically via the include directory.
