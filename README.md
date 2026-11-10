<div align="center">

<img src="assets/readme-hero.svg" alt="ToolzNG — native Windows Apple-device diagnostics" width="100%" />

# ToolzNG

### Native Apple-device connection diagnostics for Windows

**Проверяй. Понимай. Действуй безопасно.**

A local-first C17 / Win32 utility that explains Windows-visible Apple connection signals, USB events and backup-marker checks — without silent device changes or cloud uploads.

![ToolzNG 0.6.1](https://img.shields.io/badge/ToolzNG-0.6.1-FFD600?style=for-the-badge&labelColor=171923)
![C17](https://img.shields.io/badge/C-17-FFD600?style=for-the-badge&labelColor=171923)
![Windows](https://img.shields.io/badge/Windows-10%20%7C%2011-FFD600?style=for-the-badge&labelColor=171923)
![MIT](https://img.shields.io/badge/License-MIT-FFD600?style=for-the-badge&labelColor=171923)
[![CI](https://github.com/bboroski/ToolNG/actions/workflows/windows-ci.yml/badge.svg)](https://github.com/bboroski/ToolNG/actions/workflows/windows-ci.yml)

[Overview](#-what-toolzng-does) · [Screens](#-inside-the-app) · [Diagnostics](#-how-it-works) · [Build](#-build-from-source) · [Privacy](#-privacy-and-boundaries)

</div>

---

## What ToolzNG does

An iPhone or iPad connection can fail at different Windows layers: the device may not appear in Plug and Play, Apple Mobile Device Service may be unavailable, USB attachment may be intermittent, or a local backup folder may have incomplete markers. ToolzNG separates these observations instead of reducing everything to “connected / disconnected”.

It is a **Windows-side diagnostic assistant**, not an iPhone repair suite, backup manager or iOS health monitor.

## Inside the app

<p align="center">
  <img src="assets/toolzng-duck.svg" alt="ToolzNG duck engineer mascot" width="220" />
</p>

<p align="center">
  <img src="assets/screenshots/overview.svg" alt="ToolzNG overview UI preview with Duck Readiness and quality indicators; sample data" width="100%" />
  <strong>01 · Overview</strong><br />
  Readiness, evidence coverage, current finding and module navigation.
</p>

<p align="center">
  <img src="assets/screenshots/usb-devices.svg" alt="ToolzNG Apple USB devices UI preview; sample data" width="100%" />
  <strong>02 · Apple USB inspection</strong><br />
  PnP visibility, Apple service state and measured-check status.
</p>

<p align="center">
  <img src="assets/screenshots/reports.svg" alt="ToolzNG local reports UI preview; sample data" width="100%" />
  <strong>03 · Local reports</strong><br />
  Report history, evidence fields, quality flags and private preview.
</p>

<img src="assets/diagnostic-flow.svg" alt="ToolzNG workflow: observe, validate, assess, guide and prepare a user-requested report" width="100%" />

> **Visual note:** the three interface panels are static SVG previews with illustrative SAMPLE DATA. They are not screenshots of a live scan or real device measurements.

## Capabilities

| Module | Value |
|---|---|
| **Apple USB Discovery** | Finds Windows PnP interfaces with Apple Vendor ID <code>05AC</code> and reports Apple Mobile Device Service state. |
| **Duck Readiness** | Explainable 0–100 score from measurable Windows-side signals, with evidence coverage shown separately. |
| **SAMBU Quality Gate** | Flags incomplete scans, inconsistent backup counts and unsupported stability claims. |
| **USB Pulse** | Observes Apple USB arrival/removal notifications and retains a bounded recent event window. |
| **Recurrence Radar** | Classifies isolated, repeated or persistent state transitions over a 10-minute window. |
| **Backup Sentinel** | Read-only check for expected <code>Info.plist</code> and <code>Manifest.db</code> / <code>Manifest.plist</code> markers. |
| **Backup Freshness** | Shows the modification age of the newest <code>Info.plist</code> marker. |
| **DuckFix Guide** | Prioritizes a safe next step; it does not apply repairs. |
| **LinkDNA** | Compares a local FNV-1a fingerprint of Windows-visible Apple signals; not cryptographic identity. |
| **PulseVault** | Bounded local JSONL USB event journal with rotation. |
| **Privacy Shield & Evidence Seal** | Redacts Hardware ID and full backup paths; SHA-256 sidecar detects later report changes. |
| **Offline Reports & Help** | Local history (up to 32 files), in-app preview, copy, TXT export and F1 help. |

## How it works

ToolzNG uses an inspectable assessment model:

| Confirmed signal | Maximum points |
|---|---:|
| Completed scan and visible Apple USB interface | 40 |
| Apple Mobile Device Service confirmed running | 25 |
| Local backup root found | 10 |
| All discovered backup folders contain expected markers | 15 |
| Device confirmed, watcher active, observation window elapsed, no recent removals | 10 |
| **Total** | **100** |

The score is normalized over weights that could actually be measured. **Evidence coverage** shows what share of the full model was measurable; incomplete coverage makes the result provisional. A perfect score with partial coverage is not a complete pass.

The Quality Gate distinguishes:
- **Observed** — check completed and signal confirmed.
- **Not found** — check completed, expected signal absent.
- **Unknown** — access failed, enumeration incomplete or observation window unfinished.

A denied service query does not mean the service is stopped. An incomplete backup scan does not mean a backup is absent. A watcher that never started cannot prove USB stability.

## Privacy and boundaries

ToolzNG is local-first:

- no account, telemetry service or cloud upload is required;
- backup payload contents are not read for a structural marker check;
- private reports omit Hardware ID and full backup paths;
- report creation, clipboard access and export require explicit user action;
- device mutation and network transfer are denied by the SAMBU capability policy;
- event journal and report history have fixed storage bounds;
- ordinary operation does not require administrator privileges.

**What the measurements do not prove:** VID <code>05AC</code> is not an iOS pairing session; marker files do not prove backup integrity or restorability; an <code>Info.plist</code> timestamp is not the age of every backup datum; FNV-1a is not cryptographic identity; a SHA-256 sidecar does not prove report truth.

ToolzNG does not jailbreak, bypass passcodes or Activation Lock, flash/reset an iPhone, read personal device content, repair drivers automatically, or measure cable electrical quality, USB throughput, battery health or iOS temperature.

## Architecture

| Layer | Implementation |
|---|---|
| Desktop | C17 · Win32 API · GDI |
| Device discovery | Windows SetupAPI / Plug and Play |
| Apple service | Windows Service Control Manager |
| Events | Windows device notifications |
| Diagnostic core | Portable C17 SAMBU policy and workflow |
| Reports | Local TXT · SHA-256 sidecar |
| Build | CMake 3.20+ |

The portable core owns deterministic assessment, capability checks, evidence-quality rules and bounded finding memory. Windows APIs remain at the platform boundary. The embedded workflow DSL invokes five fixed diagnostic actions; it is not a general-purpose interpreter, shell or scripting runtime.

## Build from source

**Windows x64:** Windows 10/11, Visual Studio 2022 or compatible MSVC Build Tools (*Desktop development with C++*), CMake 3.20+.

~~~powershell
cmake -S . -B build -A x64 -DBUILD_TESTING=ON
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
~~~

Executable: <code>build\Release\ToolzNG.exe</code>.

**Portable core tests** (no Win32 desktop target):

~~~bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
~~~

The GUI is Windows-only. A portable-core build does not validate Apple drivers, pairing state, an actual device or a physical cable.

## Keyboard shortcuts

| Shortcut | Action |
|---|---|
| <code>F5</code> | Repeat USB scan |
| <code>Ctrl+R</code> | Create local report |
| <code>F1</code> | Open in-app help |
| <code>Ctrl++</code> / <code>Ctrl+-</code> | Zoom in / out |
| <code>Ctrl+0</code> | Restore 100% zoom |
| <code>A−</code> / <code>A+</code> | Change toolbar zoom |

The UI includes Per-Monitor DPI v2 and saved 80–160% zoom. Full UI Automation support and finished keyboard navigation remain roadmap work.

## Roadmap

1. Optional Windows adapter for a supported <code>libimobiledevice</code> build with explicit pairing checks.
2. Show device model and iOS version only when a trusted pairing API returns them.
3. Expand Apple USB driver diagnostics without silent system changes.
4. Improve RU/EN localization, accessibility and keyboard navigation.
5. Signed installer, reproducible packaging and physical-device smoke tests.

## Engineering standards

- [Duck Design Rules](docs/DUCK_DESIGN_RULES.md) — evidence, privacy, bounded behavior and visual standards.
- [SAMBU Integration](docs/SAMBU_INTEGRATION.md) — assessment model and restricted workflow.
- [Engineering Quality Gates](docs/ENGINEERING_QUALITY_GATES.md) — tests, resilience and release criteria.
- [GUI Design and Visual Evidence](docs/GUI_DESIGN.md) — current Win32/GDI navigation model and preview/screenshot rules.

For material UI changes, provide real application captures or label previews explicitly as illustrative.

## License

MIT — see [LICENSE](LICENSE).

<div align="center">

**ToolzNG — меньше догадок, больше понятных сигналов.** 🦆

</div>
