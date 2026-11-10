# 🦆 Safety and technical scope

ToolNG is a Windows-side connection diagnostics and safe-maintenance assistant, not an iOS system optimizer.

## Five differentiated ToolNG features

1. **Duck Readiness Score** — a transparent 0–100 Windows-side estimate normalized over successfully measured signals. Evidence coverage is shown separately; incomplete service, backup or USB checks remain unknown rather than being misreported as negative. A partial score is marked provisional. It does not score iOS performance or device health.
2. **USB Pulse** — listens for Windows device-interface arrival/removal notifications and records recent events whose interface path contains `VID_05AC`. It is not a cable electrical test or throughput benchmark.
3. **Backup Sentinel** — read-only checks for `Info.plist` plus `Manifest.db` or `Manifest.plist` inside backup subfolders. It does not verify backup contents, cryptographic integrity, completeness, or restore-ability.
4. **Privacy Shield** — current 0.5 reports are private by default; Hardware IDs and exact backup paths are omitted and the report opens in the in-app preview. There is no full-detail report mode in 0.5.
5. **DuckFix** — selects a suggested next step from observed connection/service/backup signals. It never applies repairs automatically.

These are ToolNG's product differentiators, not a verified claim that no other vendor has ever implemented similar ideas.

## Supported read-only operations

- Enumerate present Windows PnP device records.
- Filter hardware IDs for Apple USB vendor ID `05AC`.
- Query Apple Mobile Device service state when the service exists and the current account can query it.
- Check conventional local backup directories and expected marker-file names.
- Subscribe to Apple USB device-interface notifications exposed by Windows.
- Write a private local TXT report to %LOCALAPPDATA%\\ToolNG\\Reports, list it in the in-app history, and preview it without launching an external viewer.

## Explicitly out of scope

- Changing iOS CPU/GPU frequency or scheduler behavior.
- Clearing private iOS caches or application sandboxes over ordinary USB.
- Reading private files, messages, credentials, photos, or other personal device content.
- Jailbreak, passcode removal, activation-lock bypass, firmware downgrade, restore, or factory reset.
- Deleting or modifying local backups.
- Automatically killing or disabling Windows/Apple services.

## Product promise

ToolNG provides evidence and cautious guidance. It does not claim to overclock iOS, guarantee backup integrity, or repair every connection problem automatically. Keep UI labels, README copy, and release notes aligned with behavior actually implemented.


## ToolNG 0.4 local intelligence

- LinkDNA stores only a local FNV-1a digest of Windows-visible Apple hardware-ID/manufacturer strings and Apple service state. It is a local change detector, not a cryptographic identity and not a cable fingerprint. The baseline is stored in %LOCALAPPDATA%\ToolNG\link-profile.ini.
- PulseVault appends timestamps, arrival/removal event type, and visible Apple-interface count to %LOCALAPPDATA%\ToolNG\usb-pulse.jsonl. At 256 KiB, the previous journal is rotated to .old. It does not record serial numbers or personal files.
- Backup Freshness Radar reads only the last-write timestamp of Info.plist; it does not open backup payloads or prove backup completeness.
- Evidence Seal writes a SHA-256 sidecar for the exported report. This detects later file changes but does not authenticate the source observations.
- DuckFix Route Export writes a checklist. It does not make repairs or execute commands.


### LinkDNA baseline reset

The baseline is initialized only after at least one Apple USB interface is visible. To reset the local baseline, close ToolNG and delete `%LOCALAPPDATA%/ToolNG/link-profile.ini`. The next scan with an Apple USB interface present will create a new local profile. The file contains only the digest, not the original hardware IDs.


## ToolNG 0.5 desktop experience and SAMBU tooling 1.2.0

- The UI declares Per-Monitor DPI v2 and handles monitor DPI changes. User zoom 80–160% is persisted separately in %LOCALAPPDATA%\\ToolNG\\settings.ini; legacy LinkDNA/PulseVault data is retained.
- Diagnostic TXT reports and DuckFix route exports are stored under %LOCALAPPDATA%\\ToolNG\\Reports. The report list previews the latest files; content can be scrolled, copied, or explicitly exported as TXT.
- The portable engine separates query completion from a negative result: a service that cannot be queried is not labelled stopped, and an inaccessible or partially enumerated backup directory is not labelled missing or structurally invalid.
- Readiness is normalized over checks that completed, with a score-coverage percentage and five weighted contributions (40/25/10/15/10). The legacy confidence field is a coverage alias, not a probability.
- USB stability contributes only when a device was enumerated, the Windows observer registered, and the warm-up window elapsed. An incomplete observation is explicitly unconfirmed.
- The report engine currently creates TXT only. PDF/XLSX generation, report templates, content indexing/search, threaded cancellation, and server-side features are not supported in this version.
- Local Help, Reports, and About pages use native GDI. The Apple backup folder action opens Windows Explorer; routine documentation navigation does not launch a browser.
- SHA-256 sidecars detect later modifications to a report file but do not provide a trusted digital signature or authenticate the observations.

## ToolNG 0.6.1 — bounded SAMBU workflow

The native C17 host now compiles a restricted local workflow into a fixed-size AST and dispatches only five allowlisted diagnostic actions. Source size is limited to 3072 bytes and a workflow may contain at most eight steps; the shipped Apple audit manifest caps its run at five.

Workflow preflight checks the task manifest, autonomy class, step budget and action registry. Local LinkDNA profile recording additionally passes the SAMBU capability gate. This does not constitute an HSM-backed verifier or authorize device changes. The dispatcher performs at most one action per Win32 timer tick, on the UI thread, and stops when an action fails.

Workflow evidence is bounded and contains step identifiers, source coordinates and short findings. It does not retain workflow source text or raw hardware identifiers. A user-requested TXT report can include partial evidence from a failed run.

The workflow engine is not a full SAMBU language runtime. General functions, scopes, loops, conditionals, OOP, actors, channels, Python scheduler APIs, scientific/voxel libraries, Magento inspection, game helpers and scaffold generation are not shipped as ToolNG capabilities. Native compilation, self-hosting, ownership/effect analysis, fibers, multicore execution, IDE/LSP and web-framework features remain outside this desktop product.
