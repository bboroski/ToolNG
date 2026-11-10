# 🦆 Building ToolzNG

## Visual Studio 2022

Install the Desktop development with C++ workload and CMake 3.20+. Then run:

```powershell
.\scripts\build-windows.ps1
```

## CMake commands

```powershell
cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

## CI artifact

The workflow in `.github/workflows/build-windows.yml` runs on pushes to `main`/`master`, pull requests, manual dispatch, and version tags. It uploads `ToolzNG.exe` as a 30-day artifact. For tags beginning with `v`, it also attaches the executable to the GitHub Release.

## Link dependencies

The executable uses Windows system libraries only:

- `setupapi` — PnP device enumeration
- `advapi32` — Service Control Manager queries
- `shell32` — Known Folders and shell launch
- `ole32` — COM task-memory release for Known Folder paths
- `uuid` — Known Folder GUID definitions

No third-party runtime is required for the current feature set.


## 0.4 intelligence data

- PulseVault: %LOCALAPPDATA%\ToolzNG\usb-pulse.jsonl with .old rotation at 256 KiB.
- LinkDNA baseline: %LOCALAPPDATA%\ToolzNG\link-profile.ini.
- Diagnostic export: Documents\ToolzNG-report-*.txt plus *.sha256.
- DuckFix export: Documents\ToolzNG-DuckFix-*.txt.

The journal and baseline remain local to the Windows account. They are not added to the Git repository.
