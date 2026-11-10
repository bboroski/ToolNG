# WDEP — Windows Device Evidence Probe

**A local diagnostic probe for inspecting observable Windows device-connection signals.**

WDEP is a native C17/Win32 application that separates checks for USB/PnP events, system-service state, local backup markers, and evidence quality. It explains what was actually checked and what remains unknown.

[Українська](README.md) · [English](README.en.md) · [Русский](README.ru.md) · [Source repository](https://github.com/bboroski/ToolNG)

## Features

- discovery of visible Windows PnP/USB device interfaces;
- inspection of the relevant system-service state;
- read-only inspection of local backup markers;
- bounded observation of connect/disconnect events;
- explainable readiness scoring with separate evidence coverage;
- local reports, event journals, export, and sensitive-field redaction;
- safe guidance without automatic device repair.

## Build

Requirements: Windows 10/11, CMake, and a compatible C17 compiler.

~~~powershell
cmake -S . -B build
cmake --build build --config Release
~~~

The repository also provides a one-knob runner:

~~~powershell
python one.py
python one.py check
~~~

The exact check set depends on the current workflow and Windows API availability. Host checks are not hardware qualification.

## Evidence model

WDEP distinguishes a confirmed signal, a completed check with no finding, an unknown state, and a failed check. A high score with incomplete coverage is not reported as complete verification.

Reports are stored locally. Hardware identifiers and long local paths can be redacted. A sidecar checksum can detect later report modification, but it is not a digital signature.

## Safety boundaries

WDEP does not flash, repair, reset, or alter device state; does not inspect private backup contents; and does not upload diagnostic data to a cloud service. A UI recommendation is not an executed action.

## Technical identity

**WDEP** is the canonical name of the Windows utility. Historical source-file names and internal symbols remain where changing them would require a breaking migration; the application display identity and manifest now use WDEP.

## License

MIT. See [LICENSE](LICENSE).


## One-knob launch

- **Windows:** double-click `one.cmd` or run it from a terminal.
- **Any OS:** `python one.py` runs this project's default workflow; `python one.py check` runs headless verification.
- Desktop/IDE projects launch their application after checks; system and embedded modules build and test their host version.
- Host tests do not replace qualification on the target board, device, or production fixture.
