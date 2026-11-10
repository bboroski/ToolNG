# Changelog

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
