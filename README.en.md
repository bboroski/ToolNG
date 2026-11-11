<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="ToolNG mark" width="92"></p>
<h1 align="center">ToolNG</h1>
<p align="center"><strong>Windows Device Evidence Probe</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-WINDOWS%20DESKTOP-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/CORE-C17%20%2B%20WIN32-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/LICENSE-MIT-FFD600?style=for-the-badge&labelColor=111111"></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>

---

**ToolNG is a local Windows diagnostic utility that separates confirmed device signals from unknown or unverified state.** Its value is a clear operator view without automatic system intervention.

## Capabilities

- Discovery of visible Windows PnP/USB device interfaces.
- Inspection of relevant system-service state.
- Read-only inspection of local backup markers.
- Bounded observation of device arrival and removal events.
- Explainable readiness estimate with a separate evidence-coverage indicator.
- Local reports, logs, export, and sensitive-field redaction.
- Safe next-step guidance without automatic repair.

## Build and run

Requires Windows 10/11, CMake, and a compatible C17 compiler.

~~~powershell
cmake -S . -B build
cmake --build build --config Release
python one.py
python one.py check
~~~

The <code>one.cmd</code> entry point is also available on Windows.

## Reading the results

ToolNG distinguishes a confirmed signal, a completed check with no finding, an unknown state, and a failed check. A high score with incomplete coverage is never presented as full confirmation. Reports stay local; hardware identifiers and long local paths can be redacted. A sidecar checksum can detect later report modification, but it is not a digital signature.

## Governance and release control

| Control | Rule |
|---|---|
| Platform | Windows 10/11, C17, Win32 |
| Evidence | Check result and coverage are displayed separately |
| Privacy | Reports are local and sensitive fields can be redacted |
| Safe actions | GUI guidance does not mean an operation was executed |
| Release | Verify build, PnP/service scenarios, and report export |

## Safety boundaries

ToolNG does not flash, repair, reset, or change device state; it does not read private backup contents or send diagnostic data to the cloud. Any system change remains a separate operator-authorized action.

## Ecosystem and license

**ToolNG** is the canonical repository and product name. Related projects: [SAMBU](https://github.com/bboroski/SAMBU), [ReaperCripple](https://github.com/bboroski/ReaperCripple), [Kloz](https://github.com/bboroski/Kloz), [CO](https://github.com/bboroski/CO), [SaintSI](https://github.com/bboroski/SaintSI), [Klipot](https://github.com/bboroski/Klipot), [NUN](https://github.com/bboroski/NUN), [NUNLauncher](https://github.com/bboroski/NUNLauncher), [Fragile-Baby](https://github.com/bboroski/Fragile-Baby), and [Luci](https://github.com/bboroski/Luci).

MIT — see [LICENSE](LICENSE).

---

**Built with love for reliable systems and people.**

## NUN Mesh

<p align="center"><img src="assets/nun-mesh.svg" alt="NUN Mesh — seven connected engineering repositories with versioned contracts" width="100%"></p>

ToolNG is the local Windows diagnostics layer of NUN Mesh. It collects observable device signals and creates reports that disclose evidence coverage. The interface may recommend an action, but it must not silently modify devices, drivers, services, or backups.

Shared workspace rules: [Adaptive Workspace Integration](docs/ADAPTIVE_WORKSPACE_INTEGRATION.md).
