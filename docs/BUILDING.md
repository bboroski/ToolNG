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
- Diagnostic reports: %LOCALAPPDATA%\ToolzNG\Reports\ToolzNG-report-*.txt plus *.sha256. Legacy Documents reports are copied into the app archive at first launch; originals are kept.
- DuckFix export: Documents\ToolzNG-DuckFix-*.txt.

The journal and baseline remain local to the Windows account. They are not added to the Git repository.


## 0.5 desktop experience

- Per-monitor DPI awareness: v2 manifest, WM_DPICHANGED, logical GDI layout, user zoom 80–160%, and Windows High Contrast palette switching.
- Zoom preferences: %LOCALAPPDATA%\ToolzNG\settings.ini; existing 0.4 LinkDNA and PulseVault profile files are retained. Old reports under Documents are copied once; source files are not deleted.
- Diagnostic report archive: %LOCALAPPDATA%\ToolzNG\Reports\ToolzNG-report-*.txt with optional SHA-256 sidecar. New reports are private by default and preview in the Reports page.
- Report actions: in-app history (up to 32 entries), scrolling preview, copy to clipboard, optional Save As TXT export.
- DuckFix route exports are stored in the same local Reports directory.
- Help and product information are rendered by native GDI pages; routine use does not launch a browser.
- The optional TXT export uses the Windows common file dialog (comdlg32). No external report viewer is required.

The current native report engine intentionally produces TXT only. PDF and XLSX generation, background job cancellation, full-text query/filtering, template loops, server-side licensing, analytics, and automatic updates are not part of this release. Do not advertise those features as available until separate implementations and tests land.

### DPI smoke-test matrix

Run the Windows executable at 100%, 125%, 150%, and 200% on 1366×768, 1920×1080, 2560×1440, and 3840×2160 displays. Also test moving the window between monitors with different scaling values. Confirm zoom controls and Ctrl +/-/0, nav hit targets, report selection, and text clipping. GitHub CI compiles the binary but does not replace hardware/UI runtime tests.
