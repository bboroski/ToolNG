# Building ToolNG

ToolNG has two build surfaces:

- **SAMBU tooling and tests:** portable C17; can be configured on Windows, Linux, or macOS.
- **ToolNG desktop application:** native Win32/GDI; requires Windows.

## Requirements

### Portable core and tests

- CMake 3.20 or newer.
- A C17 compiler (MSVC, GCC, or Clang).

### Windows desktop application

- Windows 10 or Windows 11 x64.
- Visual Studio 2022 / Build Tools with the **Desktop development with C++** workload.
- CMake 3.20 or newer.

## Portable configure, build, and test

    cmake -S . -B build -DBUILD_TESTING=ON
    cmake --build build --parallel
    ctest --test-dir build --output-on-failure

On non-Windows hosts, CMake builds the reusable SAMBU core and its tests, then returns before defining the Win32 application target.

## Windows application

Run the project helper from PowerShell:

    .\scripts\build-windows.ps1

Or invoke CMake directly:

    cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Release
    cmake --build build --config Release --parallel
    ctest --test-dir build -C Release --output-on-failure

The executable is written to `build\Release\ToolNG.exe`.

The application manifest requests the current user's privileges (`asInvoker`); ordinary USB inspection does not require elevation.

## GitHub Actions

- `.github/workflows/windows-ci.yml` runs portable core tests and documentation validation on Ubuntu, and builds/tests the native application on Windows.
- `.github/workflows/build-windows.yml` builds the Windows x64 executable and uploads a 30-day artifact. Tags beginning with `v` publish the downloaded executable to a GitHub Release. Only the tagged release job receives `contents: write`.

## Local application data

Run the Windows executable at 100%, 125%, 150%, and 200% on 1366×768, 1920×1080, 2560×1440, and 3840×2160 displays. Also test moving the window between monitors with different scaling values. Confirm zoom controls and Ctrl +/-/0, nav hit targets, report selection, and text clipping. GitHub CI compiles the binary but does not replace hardware/UI runtime tests.

## Portable SAMBU core tests

The SAMBU engine and workflow interpreter are built as a portable C17 static library. On any host with CMake 3.20+ and a C17 compiler, run:

```sh
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

On non-Windows hosts CMake builds and tests the portable core, then omits the desktop target. On Windows, the same core is linked into the native ToolNG executable:

```powershell
cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

The workflow test suite covers source spans, action allowlisting, duplicate names, manifest requirements, source/step budgets, preflight denial, incremental one-step dispatch and stop-on-first-failure behavior.
- PulseVault: `%LOCALAPPDATA%\ToolNG\usb-pulse.jsonl`, with a rotated `.old` file after 256 KiB.
- LinkDNA baseline: `%LOCALAPPDATA%\ToolNG\link-profile.ini`.
- Display preferences: `%LOCALAPPDATA%\ToolNG\settings.ini`.
- TXT reports and optional SHA-256 sidecars: `%LOCALAPPDATA%\ToolNG\Reports\\`.

The data remains under the current Windows account and is not committed to the source repository.

## DPI smoke-test matrix

On Windows, exercise the app at 100%, 125%, 150%, and 200% scaling on 1366×768, 1920×1080, 2560×1440, and 3840×2160 displays. Also move the window between monitors with different scaling values. Confirm navigation hit targets, text clipping, zoom controls, report selection, and scrolling.

CI compiles the executable and runs portable engine tests. It does not replace manual runtime checks with a real iPhone/iPad.
