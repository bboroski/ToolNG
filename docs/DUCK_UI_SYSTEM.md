# Duck UI system — ToolzNG

**Status:** visual contract adopted for the Windows C17 diagnostic product. The repository already contains vector UI previews in `assets/screenshots/`; these remain illustrative SAMPLE DATA and are not live device scans.

## Identity and visual hierarchy

ToolzNG is a cautious Windows diagnostics workspace. Use the duck identity as a compact brand marker. Prioritise device evidence, explainable readiness, safe next steps and report integrity. Never turn an informational diagnostic surface into a destructive “repair” dashboard.

## Color tokens

The canonical palette lives in [`design/duck-ui.tokens.json`](../design/duck-ui.tokens.json): canvas `#0D0D0D`, surfaces `#1A1A1A` / `#141414`, yellow `#FFD600`, muted yellow `#BFA100`, selected state `#332B00`, text `#E6E6E6`, secondary text `#999999`, border `#262626`, success `#4CAF50`, warning `#FFD600`, error `#FF4444`.

Yellow is reserved for primary scan actions, current navigation, focus and progress. Readiness and diagnostic states must include an icon or label in addition to color.

## Windows UI component rules

- Keep header, navigation, active workspace and status bar consistent across every screen.
- Support keyboard navigation, visible 2 px focus, high-contrast Windows mode and 100–200% system DPI.
- Provide independent UI zoom (85–150%) without bitmap scaling; use DPI-aware fonts and vector icons.
- Keep reports and diagnostics in an in-app preview. External browser launches are reserved for explicit support/product links.
- Use sortable/filterable grids with a visible horizontal scrollbar when needed. Long paths must be selectable and copyable.
- Toasts are non-blocking and do not replace durable status or report history.

## Native implementation boundary

The C17 / Win32 core must remain independent of presentation-only state. Put visual tokens in a UI resource/header layer; never inject color, animation or windowing dependencies into device-discovery, evidence scoring or report serialization. If a native UI control is not yet implemented, label the corresponding image as a preview.

## Vector UI previews

The existing assets in `assets/screenshots/` are scalable SVG previews and explicitly use synthetic SAMPLE DATA. Treat them as design references, not proof of a live scan. New runtime screenshots should be captured from a real Windows build at 1920×1080 and 1366×768, with device identifiers and paths anonymised.
