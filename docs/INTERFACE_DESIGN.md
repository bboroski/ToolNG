# ToolNG — Amber–Obsidian interface specification

## Product identity

**ToolNG** — Windows device evidence and diagnostic console. This specification defines the visual language and operator-facing presentation for this repository; it does not expand the repository's implementation or maturity claims.

## Visual direction

Use a deep obsidian canvas, restrained graphite surfaces, signal amber for actions and active selections, and a cool secondary hue for diagnostic differentiation. The common family resemblance is deliberate; each repository retains its own monogram, domain accent, information hierarchy and interaction vocabulary.

- **Obsidian:** `#0B0B0D` — application canvas.
- **Graphite:** `#141416`, `#1C1C1E`, `#2C2C30` — panels and elevated surfaces.
- **Amber Radiance:** `#FFB020` — primary action, active marker and key signal.
- **Amber Shadow:** `#C47B13` — pressed/hover state, not body text on dark backgrounds.
- **Secondary signal:** `#79C8FF` — one project-specific information channel.
- **Primary text:** `#F5F3EE`; secondary text: `#C8C3BA`; quiet metadata: `#A6A3AB`.
- **State colours:** pass `#64C787`; caution `#FFBF47`; fail `#F16D65`; unknown `#A78BFA`.

## Layout contract

- A stable top identity rail holds repository identity, current context and high-value actions.
- A compact navigation rail separates workspaces or system domains.
- Main content uses clear, shallow panels rather than nested floating windows.
- Numeric data, time, offsets, capacities and state identifiers use a monospaced face with tabular numerals.
- Status always includes a word or symbol; colour is never the only signal.
- Interactive targets should be at least 36×36 CSS px where the platform permits.
- Active selections use an amber outline plus a soft amber-tinted surface, not a full-saturation block.
- Pressed controls move inward by 1 px and reduce shadow; hover does not shift surrounding layout.
- Dockable inspectors/logs remain bounded and must not obscure the primary work surface.

## Repository-specific surface

1. **OVERVIEW** — 03 OBSERVED.
2. **USB EVIDENCE** — UNKNOWN.
3. **REPORTS** — 72%.
 
ToolNG is a native Win32 application. Keep the design in native painting and controls; do not add a web shell or imply a green readiness state from incomplete evidence.

## Accessibility and motion

Target WCAG AA for ordinary text and controls; use near-white body text on graphite, reserving amber for large labels, icons, outlines and action surfaces with dark text. Provide visible keyboard focus, do not rely on colour alone, respect reduced-motion settings and keep transitions in the 120–220 ms range. The dim mode may reduce decorative glow and ambient contrast, but must not dim text or warning states below readable levels.

## Reference artwork

- [Project mark](../assets/amber-obsidian-mark.svg)
- [Interface concept preview](../assets/amber-obsidian-preview.svg)

The SVG is a design reference, not a screenshot or evidence that every depicted control currently exists.

## Design tokens

```css
:root {
  color-scheme: dark;
  --ao-canvas: #0B0B0D;
  --ao-surface-1: #141416;
  --ao-surface-2: #1C1C1E;
  --ao-surface-3: #2C2C30;
  --ao-border: #34343A;
  --ao-text: #F5F3EE;
  --ao-text-secondary: #C8C3BA;
  --ao-muted: #A6A3AB;
  --ao-accent: #FFB020;
  --ao-accent-pressed: #C47B13;
  --ao-secondary: #79C8FF;
  --ao-success: #64C787;
  --ao-warning: #FFBF47;
  --ao-danger: #F16D65;
  --ao-unknown: #A78BFA;
}
```
