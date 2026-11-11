# NEON SYNDICATE — interface and visual specification

**Repository:** ToolNG  
**Project focus:** WINDOWS DEVICE EVIDENCE / READ-ONLY DIAGNOSTICS  
**Visual category:** WINDOWS UTILITY / SAMPLE DATA

## Brand foundation
NEON SYNDICATE is an original cyber-noir visual system built around precise technical information, industrial geometry and restrained neon. It borrows a broad cyberpunk mood, not protected game artwork, logos or interface assets.

## Color tokens
| Name | HEX | Role |
|---|---|---|
| Void Black | `#050505` | Main canvas |
| Warning Yellow | `#FCEE0A` | Primary action, selection and high-priority attention |
| System Red | `#FF003C` | Critical errors and blocked states |
| Data Blue | `#00F0FF` | Links, technical highlights and informational signals |
| Interface White | `#FFFFFF` | Primary text |
| Carbon | `#0D1115` | Panels and structural surfaces |
| Raised Carbon | `#11171C` | Cards and controls |
| Muted | `#A6B2BC` | Supporting text |

Always pair semantic colors with a label or icon. Red is not decoration; cyan is informational, not success.

## Typography and component language
Use Orbitron or Rajdhani for uppercase headings, Rajdhani/system sans-serif for body text and Roboto Mono/Share Tech Mono for IDs, code and measurements. Provide local fallbacks. Prefer clipped/chamfered panels, thin separators, hexagonal background texture, precise grids and tabular numerals. Keep long-form copy readable and avoid excessive uppercase in body text.

## Motion, effects and accessibility
Use small hover lifts, restrained edge glow and transitions in the 120–260 ms range. Glitch and scan effects must never obscure data, labels, errors or focus. Respect `prefers-reduced-motion`. Keep keyboard focus visible, controls at least 40 px high and touch targets at least 44 px. Do not rely on color alone to communicate state.

## Repository-specific information architecture
WINDOWS DEVICE EVIDENCE / READ-ONLY DIAGNOSTICS

**Focus areas**
- USB INVENTORY
- DEVICE HEALTH
- EVIDENCE TRACE
- LOCAL REPORTS

**Reference flow**
- DISCOVER → OBSERVE → CORRELATE → REPORT

These labels describe the presentation concept for this repository. They do not assert that a graphical interface exists or that every visual panel is implemented.

## Screenshot quality policy
- Real screenshots must be captured from a running application at a stated viewport and scale.
- Prefer 1920×1080 or higher for desktop captures; retain a 1366×768 compact view when useful.
- Use PNG/WebP for actual captures. Do not upscale tiny raster sources and present them as native-resolution screenshots.
- Clearly identify synthetic sample data, and remove personal paths, access tokens and machine identifiers.
- The 1920×1200 SVG in `assets/neon-syndicate/overview.svg` is a vector design/architecture preview, not a runtime screenshot.

## Assets
- `assets/neon-syndicate/overview.svg` — project-specific visual concept.
- `assets/neon-syndicate/theme.css` — reusable web theme layer.
- `design/neon-syndicate.tokens.json` — machine-readable design tokens.
