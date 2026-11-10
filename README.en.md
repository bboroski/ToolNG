<p align="center">
  <img src="assets/duck-ui/duck-mark.svg" alt="Duck mark" width="104" />
</p>
<h1 align="center">ToolNG</h1>
<p align="center"><strong>Windows Device Evidence Probe</strong></p>
<p align="center">
  <img src="https://img.shields.io/badge/STATUS-WINDOWS%20DESKTOP-FFD600?style=for-the-badge&labelColor=111111" alt="Project status" />
  <img src="https://img.shields.io/badge/CORE-C17%20%2B%20WIN32-FFD600?style=for-the-badge&labelColor=111111" alt="Core technology" />
  <img src="https://img.shields.io/badge/LICENSE-MIT-FFD600?style=for-the-badge&labelColor=111111" alt="MIT license" />
</p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<hr />

**Local Windows diagnostics that show which device signals are confirmed and which facts remain unknown.**

## Product position

ToolNG is a native C17/Win32 probe for support engineers, administrators, and teams that need cautious device-connection checks without automatic system intervention. Its value is transparent evidence, not a magical green status.

## Capabilities

- Detect visible Windows PnP/USB device interfaces.
- Check the state of the relevant system service.
- Inspect local backup markers in read-only mode.
- Observe connection and disconnection events within bounded limits.
- Provide an explainable readiness assessment with separate evidence coverage.
- Produce local reports and event logs, export data, and redact sensitive fields.
- Recommend safe next steps without attempting automatic repair.

## Build and run

Requires Windows 10/11, CMake, and a compatible C17 compiler.

~~~powershell
cmake -S . -B build
cmake --build build --config Release
~~~

Use the one-knob entry point:

~~~powershell
python one.py
python one.py check
~~~

The <code>one.cmd</code> script is also available on Windows.

## Evidence model

ToolNG distinguishes a confirmed signal, a completed check with no finding, an unknown state, and a failed check. A high score with incomplete coverage is not presented as full confirmation.

Reports stay local. Hardware identifiers and long local paths may be hidden. A sidecar checksum can help detect later report changes, but it is not a digital signature.

## Project management and release gates

| Gate | Rule |
|---|---|
| Platform | Windows 10/11, C17, Win32 |
| Evidence | Report check outcome separately from coverage |
| Privacy | Reports are local; sensitive fields can be redacted |
| Safe action | A GUI recommendation is not an executed operation |
| Release | Verify builds, PnP/service scenarios, and report export integrity |

## Safety boundaries

ToolNG does not flash, repair, reset, or change device state; does not read private backup contents; and does not upload diagnostic data to the cloud. The operator remains responsible for any system change.

## Technical identity

**ToolNG** is the canonical name of the Windows utility. The public policy/workflow core uses <code>ToolNG*</code> types, <code>toolng_*</code> functions, and <code>ToolNG_*</code> constants. Historical file paths and old CMake targets remain for compatibility only.

## Ecosystem

[ SAMBU ](https://github.com/bboroski/SAMBU) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUN](https://github.com/bboroski/NUN) · [NUNLauncher](https://github.com/bboroski/NUNLauncher).

## License

MIT — see [LICENSE](LICENSE).

## Academic biology glossary

- [500 academic terms EN–RU](data/biology/academic_terms_500.tsv)
- [Data contract and usage boundaries](docs/BIOLOGY_DOMAIN_PACK.md)

An offline reference pack for education and metadata; it does not control systems, devices or diagnostics.
