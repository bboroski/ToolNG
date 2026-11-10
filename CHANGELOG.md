# Changelog

## 0.6.1 — SAMBU Workflow Runtime

- Added a bounded C17 workflow lexer/parser and source-aware AST with task manifests and explicit autonomy classes.
- Restricted executable actions to five allowlisted Apple diagnostic host operations; arbitrary commands, network actions and device mutation are not expressible.
- Added a dual preflight/capability gate for local LinkDNA profile recording.
- Added one-action-per-Win32-timer cooperative dispatch and per-step evidence in local TXT reports.
- Split the portable SAMBU core from the Win32 desktop target so CTest can run on non-Windows hosts.
- Added parser/runtime tests and CI execution of the CTest suite.
- Updated SAMBU integration, build and scope documentation to distinguish implemented, limited and planned capabilities.

## 0.5.0 — Duck Desktop Experience

- Added per-monitor DPI-awareness v2 and retained user zoom (80–160%) with A−/A+ controls and Ctrl +/-/0 shortcuts.
- Replaced blue UI accents with the black/graphite/yellow design tokens.
- Added local private-by-default TXT report generation, an in-app history list, scrollable preview, clipboard copy, optional TXT Save As export, and SHA-256 sidecars.
- Moved DuckFix route output into the application-local Reports directory.
- Added native offline Help navigation and F1 shortcut; removed routine Apple support/store browser launch buttons.
- Kept legacy LinkDNA and PulseVault files in place; new display settings use a separate settings.ini file.


## 0.4.0 — Duck Intelligence Plus

- Added LinkDNA local baseline signature to detect changes in the Windows-visible Apple USB/service profile. The FNV-1a digest is a change detector, not a security identity.
- Added PulseVault JSONL persistence with bounded 256 KiB rotation.
- Added Backup Freshness Radar based on the newest backup Info.plist last-write timestamp.
- Added exportable DuckFix Route checklist.
- Added SHA-256 sidecar for generated diagnostic reports (Evidence Seal).
- Expanded privacy and measurement-boundary documentation.

## 0.3.0 — Duck Intelligence Edition

- Added transparent Duck Readiness Score based only on observable Windows-side connection signals.
- Added USB Pulse event monitoring for Apple USB interface arrival/removal notifications.
- Added read-only Backup Sentinel checking expected backup marker files (`Info.plist` and `Manifest.db`/`Manifest.plist`).
- Added Privacy Shield with a private-by-default report choice, technical-detail mode, and save preview.
- Added DuckFix context-sensitive next-step guidance.
- Updated the interface and documentation to state measurement limits and avoid unsupported iOS speed claims.

## 0.2.0 — Duck Edition

- Added a custom GDI-rendered duck mascot to the native Win32 interface.
- Switched the UI to a graphite-and-gold visual system.
- Added a duck-themed SVG README banner and illustrations.

## 0.1.0 — Initial MVP

- Added native Win32/GDI interface written in C17.
- Added Apple USB PnP detection using `VID_05AC`.
- Added Apple Mobile Device Service status inspection.
- Added local backup folder discovery and UTF-8 diagnostic reports.
- Added CMake and Windows GitHub Actions build workflow.
