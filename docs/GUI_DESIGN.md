# ToolzNG GUI design and visual evidence

**Application:** ToolzNG 0.6.1  
**UI stack:** native Win32 + GDI, C17  
**Design model:** progressive disclosure ("Curtain Dynamics")

## What is implemented

ToolzNG is a native Windows desktop application. It does not use Electron, Chromium, React, Radix UI, Tailwind CSS or Framer Motion.

The interface reduces cognitive load by separating the workflow into layers:

1. **Overview — entry state.** Shows the current connection state, Duck Readiness score/coverage, and primary actions.
2. **Evidence pages — inspection state.** The USB page shows Windows-visible Apple PnP interfaces, Apple service status, USB event counts and Quality Gate state. Backup checks are reflected in the diagnosis and report.
3. **Action and explanation state.** DuckFix guidance, local help and the reports page expose the next safe action and the evidence behind it.
4. **Report review state.** The internal report view keeps the local history and selected TXT preview inside the application; copy and export remain explicit user actions.

This is the practical interpretation of the "Curtain Dynamics" concept: the overview stays compact and users reveal the level of detail needed for the task. It is contextual navigation, not an implemented animated-curtain system, workflow editor, terminal emulator or real-time inference-chain visualizer.

## Technical presentation rules

- Keep navigation and high-priority actions consistent across pages.
- Use the existing graphite/black surfaces and restrained yellow accents; do not add color solely for decoration.
- Respect Per-Monitor DPI handling and the persisted 80–160% user zoom.
- Preserve keyboard and mouse access to the existing Win32 controls.
- Distinguish verified results, incomplete checks and advisory text.
- Do not label sample values as live device data or a successful physical-device test.

## Visual evidence

The current repository contains three **static SVG UI previews**:

| Asset | Screen | Evidence type |
|---|---|---|
| `assets/screenshots/overview.svg` | Overview dashboard | Vector preview; synthetic sample data |
| `assets/screenshots/usb-devices.svg` | Apple USB devices | Vector preview; synthetic sample data |
| `assets/screenshots/reports.svg` | Internal reports | Vector preview; synthetic sample data |

These files are documentation mock-ups based on the current Win32/GDI layout. They are not captures of a running executable and do not prove that a physical iPhone or iPad was connected during a test. CI validates their presence, SVG parsing and sample-data labelling.

### Capture standard for future real screenshots

When screenshots of the running application are added, use:

- 1920×1080 or a higher 16:9 capture where practical;
- a complete window frame and visible page context;
- synthetic device names, paths and reports, with hardware identifiers and personal backup paths removed;
- a separate caption stating Windows version, application build, DPI scale and whether the image is an actual capture or a design preview;
- no fabricated scan success, fabricated model/iOS version or fabricated security status.

The present 1120×790 SVG previews remain in the README until genuine 1920×1080 captures can be made from the built Windows application. Do not upscale a vector mock-up and relabel it as a screenshot.

## Architecture boundary and roadmap

The current UI is implemented in `src/main.c` as native Win32/GDI rendering and window-message handling. The portable C17 assessment/workflow core is separated into `src/sambu_engine.c/.h` and `src/sambu_workflow.c/.h`.

A future refactor may move individual page renderers, navigation state and report-preview controls into focused modules. That is a roadmap item, not a completed monolithic-file split. Any extraction should preserve the current CMake targets, DPI behaviour, hit testing and report privacy rules and add regression tests before removing the original implementation.
