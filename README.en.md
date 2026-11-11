<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="ToolNG mark" width="92"></p>
<h1 align="center">ToolNG</h1>
<p align="center"><strong>Windows Device Evidence Probe</strong></p>
<p align="center"><strong>Windows desktop utility · read-only diagnostics</strong></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<sub>Updated: 2026-11-11</sub>


## Contents

- [Current capabilities](#current-capabilities)
- [Quick start and verification](#quick-start-and-verification)
- [Architecture and verified scope](#architecture-and-verified-scope)
- [Interface previews](#interface-previews)
- [Engineering quality](#engineering-quality)
- [Documentation map](#documentation-map)
- [NUN Mesh and related repositories](#nun-mesh-and-related-repositories)
- [Design and documentation](#design-and-documentation)
- [License](#license)
- [Duck Black / Yellow palette](#duck-black-yellow-palette)

> **ToolNG** — ToolNG is a local, read-only Windows diagnostic utility that separates confirmed device signals from unknown state and incomplete evidence coverage.

## Current capabilities
- Observe Windows PnP/USB devices and related services.
- Read-only inspection of local backup markers and connection events.
- Local reports, logs, exports and sensitive-field redaction.
- Explainable readiness and evidence-coverage reporting.

## Quick start and verification
```bash
cmake -S . -B build
cmake --build build --config Release
python one.py check
```

## Architecture and verified scope
ToolNG does not repair devices, modify drivers or services, inspect private backup contents or upload diagnostics. A recommendation is not an executed action.

## Interface previews

These are static vector UI previews, not screenshots of a live scan. All device and diagnostic values are illustrative SAMPLE DATA.

<p align="center"><img src="assets/screenshots/overview.svg" alt="ToolNG overview dashboard with sample data" width="960"></p>
<p align="center"><em>Overview dashboard</em></p>

<p align="center"><img src="assets/screenshots/usb-devices.svg" alt="ToolNG USB device list with sample data" width="960"></p>
<p align="center"><em>USB devices</em></p>

<p align="center"><img src="assets/screenshots/reports.svg" alt="ToolNG local reports with sample data" width="960"></p>
<p align="center"><em>Local reports</em></p>

## Engineering quality
- Run the checks appropriate to the target platform; host CI is not hardware qualification.
- Mark capabilities as implemented, partial or planned according to code, tests and evidence.
- A mockup, specification or open pull request is not proof of a production-ready feature.
- External integrations must be explicit, versioned, isolated and testable.

## Documentation map

- [Build and verification](docs/BUILDING.md)
- [Safety and scope](docs/SAFETY_AND_SCOPE.md)
- [GUI design](docs/GUI_DESIGN.md)
- [Duck UI system](docs/DUCK_UI_SYSTEM.md)

## NUN Mesh and related repositories

<p align="center"><img src="assets/nun-mesh.svg" alt="NUN Mesh architecture diagram showing independent repositories connected by versioned contracts" width="960"></p>
[SAMBU](https://github.com/bboroski/SAMBU) · [ToolNG](https://github.com/bboroski/ToolNG) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUN](https://github.com/bboroski/NUN) · [NUNLauncher](https://github.com/bboroski/NUNLauncher)

Each repository retains its canonical name, code, APIs, release lifecycle and ownership boundaries. Integration means versioned contracts, not automatic merging of independent runtimes.

## Design and documentation
The shared interface direction is **Duck Black / Yellow**: dark surfaces, yellow accents for actions and state, readable typography, accessible contrast, keyboard navigation and reduced-motion support. See [design/DUCK_UI_SYSTEM.md](design/DUCK_UI_SYSTEM.md) when present.

## License
MIT

---
**Built with care for engineering, clarity and people.**


## Duck Black / Yellow palette
The legacy badge and mark compatibility colors remain `#FFD600` (yellow) and `#111111` (near-black). The current interface canvas is `#0D0D0D`. The complete six-color palette, fourteen functional shades and RGB/HSL values are maintained in `design/duck-ui.tokens.json` (schema 3.0.0). Yellow is reserved for primary actions, focus and selection; red and cyan identify semantic error and information states.
