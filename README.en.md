<div align="center">

<img src="assets/readme-hero.svg" alt="ToolzNG — native Apple-device diagnostics for Windows" width="100%" />

# ToolzNG

### Apple-device connection diagnostics for Windows

**Inspect. Understand. Act safely.**

A local-first C17 / Win32 utility that explains Windows-visible Apple signals, USB events and backup markers — without silent device changes or cloud uploads.

[Українська](README.md) · [English](README.en.md) · [Русский](README.ru.md)

![ToolzNG 0.6.1](https://img.shields.io/badge/ToolzNG-0.6.1-FFD600?style=for-the-badge&labelColor=171923)
![C17](https://img.shields.io/badge/C-17-FFD600?style=for-the-badge&labelColor=171923)
![Windows](https://img.shields.io/badge/Windows-10%20%7C%2011-FFD600?style=for-the-badge&labelColor=171923)
![MIT](https://img.shields.io/badge/License-MIT-FFD600?style=for-the-badge&labelColor=171923)
[![CI](https://github.com/bboroski/ToolNG/actions/workflows/windows-ci.yml/badge.svg)](https://github.com/bboroski/ToolNG/actions/workflows/windows-ci.yml)

[Overview](#-what-toolzng-is) · [Interface](#-inside-the-app) · [Assessment](#-how-assessment-works) · [Build](#-build-from-source) · [Privacy](#-privacy-and-limits)

</div>

---

## What ToolzNG is

An iPhone or iPad connection can fail at several Windows layers: the device may not appear in Plug and Play, Apple Mobile Device Service may be unavailable, USB attachment may be intermittent, or a local backup folder may have incomplete structural markers. ToolzNG separates these signals instead of reducing everything to “connected / disconnected”.

It is a **Windows-side diagnostic assistant**, not an iPhone repair suite, backup manager or iOS health monitor.

## Inside the app

<p align="center">
  <img src="assets/toolzng-duck.svg" alt="ToolzNG duck engineer mascot" width="220" />
</p>

<p align="center">
  <img src="assets/screenshots/overview.svg" alt="ToolzNG dashboard preview with Duck Readiness; sample data" width="100%" />
  **01 · Overview**<br />
  Readiness, evidence coverage, priority finding and module navigation.
</p>

<p align="center">
  <img src="assets/screenshots/usb-devices.svg" alt="ToolzNG Apple USB inspection preview; sample data" width="100%" />
  **02 · Apple USB inspection**<br />
  PnP visibility, Apple service state and check completeness.
</p>

<p align="center">
  <img src="assets/screenshots/reports.svg" alt="ToolzNG local reports preview; sample data" width="100%" />
  **03 · Local reports**<br />
  Report history, evidence fields, quality flags and private preview.
</p>

<img src="assets/diagnostic-flow.svg" alt="ToolzNG workflow: observe, validate evidence, assess, guide and prepare a user-requested report" width="100%" />

> **Image note:** the three interface panels are static SVG previews containing illustrative SAMPLE DATA. They are not screenshots of a live diagnostic session or actual device measurements.

## Capabilities

| Module | Practical value |
|---|---|
| **Apple USB Discovery** | Finds Windows PnP interfaces with Apple Vendor ID <code>05AC</code> and reports Apple Mobile Device Service state. |
| **Duck Readiness** | Explainable 0–100 score based on measurable Windows signals, with evidence coverage shown separately. |
| **Evidence Quality Gate** | Detects incomplete scans, inconsistent backup counts and unsupported stability conclusions. |
| **USB Pulse** | Observes Apple USB arrival/removal notifications and retains a bounded recent event window. |
| **Recurrence Radar** | Distinguishes isolated, repeated and persistent state transitions over a 10-minute window. |
| **Backup Sentinel** | Read-only check for expected <code>Info.plist</code> and <code>Manifest.db</code> / <code>Manifest.plist</code> markers. |
| **Backup Freshness** | Shows the modification age of the newest <code>Info.plist</code> marker. |
| **DuckFix Guide** | Prioritizes a safe next step; it does not apply repairs. |
| **LinkDNA** | Compares a local FNV-1a fingerprint of Windows-visible Apple signals; not cryptographic device identity. |
| **PulseVault** | Bounded local JSONL USB event journal with rotation. |
| **Privacy Shield & Evidence Seal** | Redacts Hardware ID and full backup paths; SHA-256 sidecar detects later report changes. |
| **Offline Reports & Help** | Local history (up to 32 reports), in-app preview, copy, TXT export and F1 help. |

## Interface model: progressive disclosure

ToolzNG uses progressive disclosure: show the user the main result first, then reveal the inspection details and supporting evidence.

| Level | What appears | Location |
|---|---|---|
| **Overview** | Latest scan, readiness, evidence coverage and highest-priority finding | Dashboard |
| **Inspection** | Device visibility, service state, backup markers, USB events and quality flags | USB / Backup / Safe Care pages |
| **Evidence detail** | Stable finding code, score contribution, unknown-state reason and measurement limits | Local report preview and export |

This is an information-architecture model, not a claim of hardware-accelerated animation or a web UI. The shipped application is native Win32/GDI.

## Internal architecture boundary

- **Diagnostic core** — native C17 modules for Windows-signal assessment, evidence-quality validation and bounded diagnostic workflows.
- **ToolzNG** is a standalone C17 / Win32 desktop utility. It does not embed Python, a general-purpose scripting runtime or creative-text processing.
- Engineering principles include evidence-first decisions, bounded work, explicit capabilities, local-first handling and a strict distinction between “not found” and “not checked”.
- User-authored creative text and lyric corpora are outside the diagnostic pipeline and never influence device findings.

## How assessment works

ToolzNG uses a fixed, inspectable score model:

| Confirmed signal | Maximum points |
|---|---:|
| Completed scan and visible Apple USB interface | 40 |
| Apple Mobile Device Service confirmed running | 25 |
| Local backup root found | 10 |
| Every discovered backup folder contains expected markers | 15 |
| Device confirmed, watcher active, observation window elapsed, no recent removals | 10 |
| **Total** | **100** |

The score is normalized over weights that could actually be measured. **Evidence coverage** reports what share of the full model was measurable; incomplete coverage makes the result provisional. A perfect score with partial coverage is not a complete pass.

The Quality Gate distinguishes:

- **Observed** — the check completed and the signal was confirmed.
- **Not found** — the check completed, but the expected signal was absent.
- **Unknown** — access failed, enumeration was incomplete or the observation window did not finish.

A denied Service Control Manager query does not mean the service is stopped. An incomplete backup scan does not prove a backup is absent. A watcher that never started cannot prove USB stability.

## Privacy and limits

ToolzNG is local-first:

- no account, telemetry service or cloud upload is required;
- backup payload contents are not read for a structural marker check;
- private reports omit Hardware ID and full backup paths;
- report creation, clipboard access and export require an explicit user action;
- device mutation and network transfer are denied by the local capability policy;
- the event journal and report history have fixed storage bounds;
- ordinary operation does not require administrator privileges.

**What the measurements do not prove:** VID <code>05AC</code> visibility is not an iOS pairing session; marker files do not prove backup integrity or restorability; an <code>Info.plist</code> timestamp is not the age of every backup datum; FNV-1a is not cryptographic identity; a SHA-256 sidecar does not prove report truth.

ToolzNG does not jailbreak, bypass passcodes or Activation Lock, flash or reset an iPhone, read personal device content, repair drivers automatically, or measure cable electrical quality, USB throughput, battery health or iOS temperature.

## Architecture

| Layer | Implementation |
|---|---|
| Desktop UI | C17 · Win32 API · GDI |
| Device discovery | Windows SetupAPI / Plug and Play |
| Apple service status | Windows Service Control Manager |
| USB events | Windows device notifications |
| Diagnostic core | Portable C17 diagnostic policy and workflow core |
| Reports | Local TXT · SHA-256 sidecar |
| Build | CMake 3.20+ |

The portable core owns deterministic assessment, capability checks, evidence-quality rules and bounded finding memory. Windows APIs remain at the platform boundary. The embedded workflow DSL invokes five fixed diagnostic actions; it is not a general-purpose interpreter, shell or scripting language.

## Build from source

**Windows x64:** Windows 10/11, Visual Studio 2022 or compatible MSVC Build Tools with *Desktop development with C++*, and CMake 3.20+.

~~~powershell
cmake -S . -B build -A x64 -DBUILD_TESTING=ON
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
~~~

Executable: <code>build\Release\ToolzNG.exe</code>.

The GUI is Windows-only. A portable-core build does not validate Apple drivers, pairing state, a physical device or a cable.

## Keyboard shortcuts

| Shortcut | Action |
|---|---|
| <code>F5</code> | Repeat USB scan |
| <code>Ctrl+R</code> | Create local report |
| <code>F1</code> | Open in-app help |
| <code>Ctrl++</code> / <code>Ctrl+-</code> | Zoom in / out |
| <code>Ctrl+0</code> | Restore 100% zoom |
| <code>A−</code> / <code>A+</code> | Change toolbar zoom |

The UI includes Per-Monitor DPI v2 and saved 80–160% zoom. Full UI Automation support and complete keyboard navigation remain roadmap work.

## Roadmap

1. Extract dashboard drawing, report composition and navigation/event routing from the Win32 host into focused C modules while preserving the portable-core boundary.
2. Optional Windows adapter for a supported <code>libimobiledevice</code> build with explicit pairing checks.
2. Show device model and iOS version only when a trusted pairing API returns them.
3. Expand Apple USB driver diagnostics without silent system changes.
4. Improve RU/EN localization, accessibility and keyboard navigation.
5. Signed installer, reproducible packaging and physical-device smoke tests.

## Engineering standards

- [Duck Design Rules](docs/DUCK_DESIGN_RULES.md) — evidence, privacy, bounded behavior and visual standards.
- [Engineering Quality Gates](docs/ENGINEERING_QUALITY_GATES.md) — tests, resilience and release criteria.

For material UI changes, provide real application captures or label preview images explicitly as illustrative.

## License

MIT — see [LICENSE](LICENSE).

<div align="center">

**ToolzNG — fewer guesses, clearer signals.** 🦆

</div>