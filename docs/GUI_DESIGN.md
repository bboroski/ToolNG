# ToolNG GUI design and visual evidence

**Application:** ToolNG 0.6.1  
**UI stack:** native Win32 + GDI, C17  
**Design model:** progressive disclosure ("Curtain Dynamics")

## Implemented interaction model

ToolNG is a native Windows desktop application. It does not use Electron, Chromium, React, Radix UI, Tailwind CSS or Framer Motion.

The current interface reduces cognitive load by separating a diagnostic task into a small number of layers:

1. **Overview — entry state.** Shows the current connection state, Duck Readiness score and evidence coverage, the active finding and primary scan/report actions.
2. **USB and device evidence — inspection state.** Shows Windows-visible Apple PnP interfaces, Apple service status and bounded USB event evidence.
3. **Care and guidance — action state.** Presents DuckFix recommendations and safety boundaries without silently changing drivers, device settings or iOS.
4. **Reports and help — review state.** Exposes local report history, an in-app TXT preview, user-requested copy/export and offline help.

This is the practical interpretation of "Curtain Dynamics": the overview stays concise while the user opens the relevant evidence and guidance page. It is contextual navigation, not an implemented animated-curtain system, workflow editor, terminal emulator or real-time inference-chain visualizer.

## Visual system and platform behavior

- Native Win32 controls and GDI drawing.
- Existing graphite/black surfaces with restrained yellow accents.
- Per-Monitor DPI v2 handling and persisted 80–160% user zoom.
- Existing navigation and keyboard shortcuts; full UI Automation and complete keyboard-only navigation remain roadmap work.
- Keep the visual hierarchy consistent: status first, evidence second, next action third.
- Separate verified observations, unknown checks and recommendations. Do not make an unavailable check look like a confirmed failure.

## Visual evidence in the repository

The README currently shows three high-resolution static SVG interface previews (1920×1354 canvas, vector viewBox retained for sharp scaling):

| Asset | Screen | Evidence type |
|---|---|---|
| `assets/screenshots/overview.svg` | Overview dashboard | Vector preview; synthetic sample data |
| `assets/screenshots/usb-devices.svg` | Apple USB inspection | Vector preview; synthetic sample data |
| `assets/screenshots/reports.svg` | Local reports | Vector preview; synthetic sample data |

They are documentation mock-ups, not captures of a running executable and not proof of a successful physical-device scan. The documentation verifier checks that the SVG assets exist, parse as XML and are explicitly labelled as sample data.

## Capture standard for future real screenshots

When genuine captures of the running Windows application are added:

- use 1920×1080 or a higher 16:9 capture where practical;
- preserve the application window frame and the page context;
- use synthetic device names, report values and paths;
- remove Hardware IDs, serial numbers and personal backup paths;
- caption the application version, Windows version and DPI scale;
- explicitly distinguish a real app capture from a vector preview.

Do not upscale a vector mock-up and relabel it as a screenshot. The updated 1920×1354 SVG assets remain labelled as previews until genuine captures are produced from the built desktop application. Their vectors are scalable, but they are still not runtime screenshots.

## Architecture boundary and roadmap

The UI rendering and window-message handling currently live primarily in `src/main.c`. The portable assessment and workflow core is separated into `src/toolng_engine.c/.h` and `src/toolng_workflow.c/.h`.

Extracting individual page renderers, navigation state and report-preview controls into smaller modules is a future refactor, not a completed split. Any extraction should preserve DPI behavior, hit testing, keyboard shortcuts, report privacy and the current CMake targets, with regression tests added before the original implementation is removed.


## Amber–Obsidian v2 visual revision

The native Win32/GDI surface in `src/main.c` now uses the ToolNG domain palette: obsidian `#0B0B0D`, graphite `#141416` / `#1C1C1E`, amber `#FFB020`, pressed amber `#C47B13`, and a cool blue signal `#79C8FF` for secondary diagnostic information. Primary button labels use dark foreground on the full amber action surface. Status colors remain explicit and must still carry text labels; unknown evidence is not rendered as a successful result.

The design target is documented in [INTERFACE_DESIGN.md](INTERFACE_DESIGN.md), with the machine-readable palette in [design/duck-ui.tokens.json](../design/duck-ui.tokens.json). The three `assets/screenshots/*.svg` files remain illustrative sample-data previews, not live device scans or runtime captures.
