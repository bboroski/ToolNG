# Amber–Obsidian UI system — ToolNG

**Status:** visual contract adopted for the Windows C17 diagnostic product. The repository already contains vector UI previews in `assets/screenshots/`; these remain illustrative SAMPLE DATA and are not live device scans.

## Identity and visual hierarchy

ToolNG is a cautious Windows diagnostics workspace. Use the duck identity as a compact brand marker. Prioritise device evidence, explainable readiness, safe next steps and report integrity. Never turn an informational diagnostic surface into a destructive “repair” dashboard.

## Color tokens

The canonical palette lives in [`design/duck-ui.tokens.json`](../design/duck-ui.tokens.json): canvas `#0B0B0D`, surfaces `#1C1C1E` / `#141416`, amber `#FFB020`, muted amber `#C47B13`, selected state `#3A2408`, text `#F5F3EE`, secondary text `#A6A3AB`, border `#34343A`, success `#64C787`, warning `#FFB020`, error `#F16D65`.

amber is reserved for primary scan actions, current navigation, focus and progress. Readiness and diagnostic states must include an icon or label in addition to color.

## Windows UI component rules

- Keep header, navigation, active workspace and status bar consistent across every screen.
- Support keyboard navigation, visible 2 px focus, high-contrast Windows mode and 100–200% system DPI.
- Provide independent UI zoom (80–160%) without bitmap scaling; use DPI-aware fonts and vector icons.
- Keep reports and diagnostics in an in-app preview. External browser launches are reserved for explicit support/product links.
- Use sortable/filterable grids with a visible horizontal scrollbar when needed. Long paths must be selectable and copyable.
- Toasts are non-blocking and do not replace durable status or report history.

## Native implementation boundary

The existing C17 / Win32 GDI desktop shell already uses the obsidian / amber palette, Per-Monitor DPI handling and persisted application zoom. The refreshed `assets/screenshots/` SVG files are scalable 1920 × 1354 canvas previews containing synthetic data, not captures from a running executable. Do not replace or overstate that evidence. The C17 / Win32 core must remain independent of presentation-only state. Put visual tokens in a UI resource/header layer; never inject color, animation or windowing dependencies into device-discovery, evidence scoring or report serialization. If a native UI control is not yet implemented, label the corresponding image as a preview.

## Vector UI previews

The existing assets in `assets/screenshots/` are scalable SVG previews and explicitly use synthetic SAMPLE DATA. Treat them as design references, not proof of a live scan. New runtime screenshots should be captured from a real Windows build at 1920×1080 and 1366×768, with device identifiers and paths anonymised.


## Amber–Obsidian v2 alignment

This legacy design contract is aligned with the repository-specific palette and visual system v2. See [the current interface specification](INTERFACE_DESIGN.md) and the machine-readable [design tokens](../design/duck-ui.tokens.json). The project accent is #FFB020; secondary data signal is #79C8FF. The embedded target/library core must remain presentation-free where the product boundary requires it.
