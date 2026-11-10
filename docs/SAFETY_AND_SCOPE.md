# 🦆 Safety and technical scope

ToolzNG is a Windows-side connection diagnostics and safe-maintenance assistant, not an iOS system optimizer.

## Five differentiated ToolzNG features

1. **Duck Readiness Score** — a transparent 0–100 estimate based on visible Apple USB PnP presence, Apple service state, backup folder/marker presence, and a short USB stability observation window. It does not score iOS performance or device health.
2. **USB Pulse** — listens for Windows device-interface arrival/removal notifications and records recent events whose interface path contains `VID_05AC`. It is not a cable electrical test or throughput benchmark.
3. **Backup Sentinel** — read-only checks for `Info.plist` plus `Manifest.db` or `Manifest.plist` inside backup subfolders. It does not verify backup contents, cryptographic integrity, completeness, or restore-ability.
4. **Privacy Shield** — offers a redacted report or a full technical report, followed by a preview/confirmation. The redacted report omits hardware IDs and the exact backup path.
5. **DuckFix** — selects a suggested next step from observed connection/service/backup signals. It never applies repairs automatically.

These are ToolzNG's product differentiators, not a verified claim that no other vendor has ever implemented similar ideas.

## Supported read-only operations

- Enumerate present Windows PnP device records.
- Filter hardware IDs for Apple USB vendor ID `05AC`.
- Query Apple Mobile Device service state when the service exists and the current account can query it.
- Check conventional local backup directories and expected marker-file names.
- Subscribe to Apple USB device-interface notifications exposed by Windows.
- Write a local text report to Documents after the user chooses its privacy level and confirms the preview.

## Explicitly out of scope

- Changing iOS CPU/GPU frequency or scheduler behavior.
- Clearing private iOS caches or application sandboxes over ordinary USB.
- Reading private files, messages, credentials, photos, or other personal device content.
- Jailbreak, passcode removal, activation-lock bypass, firmware downgrade, restore, or factory reset.
- Deleting or modifying local backups.
- Automatically killing or disabling Windows/Apple services.

## Product promise

ToolzNG provides evidence and cautious guidance. It does not claim to overclock iOS, guarantee backup integrity, or repair every connection problem automatically. Keep UI labels, README copy, and release notes aligned with behavior actually implemented.
