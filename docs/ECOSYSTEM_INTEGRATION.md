# NUN Mesh integration — ToolNG

ToolNG is the operator-facing Windows diagnostics application. It observes Windows-visible Apple USB and service signals, classifies evidence coverage and prepares local reports. It does not repair iOS or silently change drivers/device settings.

## Existing graphical application

ToolNG is a native C17/Win32/GDI desktop program, not a web wrapper. Its existing UI has overview, device inspection, care/guidance, local reports and help surfaces; it also supports DPI-aware layout, zoom, keyboard shortcuts and explicit copy/export actions. The repository's SVG screen previews remain labelled as sample data, not screenshots of a live scan.

## Mesh connections

- **SAMBU:** ToolNG contains a portable policy/workflow core from the SAMBU project family. It does not load the Python interpreter or the creative-text subsystem.
- **SaintSI / ReaperCripple:** ToolNG's evidence vocabulary aligns with the broader mesh distinction between observed, not found, unknown, failed and passed. It does not claim to supervise MCU firmware or manufacturing fixtures.
- **Klipot:** the portal links to this app and its GUI guide; its state demos are browser-only simulations.

## UX and evidence rules

Keep the primary result visible, then progressively reveal the underlying evidence. A button that exports or copies data must remain a user-initiated action. Unknown service access, incomplete scans and missing USB observations must not be displayed as confirmed device failures. Any future transition effects must respect reduced-motion and high-contrast settings.

See [GUI design and visual evidence](GUI_DESIGN.md), [SAMBU integration](SAMBU_INTEGRATION.md), and the [NUN Mesh protocol](https://github.com/bboroski/Klipot/blob/main/docs/INTEGRATION_PROTOCOL.md).
