# 🦆 Safety and technical scope

ToolzNG is a Windows-side diagnostic helper, not an iOS system optimizer.

## Supported operations

1. Read present Windows PnP device records.
2. Filter hardware IDs for Apple USB vendor ID `05AC`.
3. Query the Apple Mobile Device service state, when the service exists and the current account can query it.
4. Check a small set of conventional local backup paths.
5. Write a local text report to Documents.
6. Open Apple support pages and Microsoft Store search for Apple Devices.

## Explicitly out of scope

- Changing iOS CPU/GPU frequency or scheduler behavior.
- Clearing private iOS caches or application sandboxes over an ordinary USB connection.
- Reading private files, messages, credentials, photos, or other personal device content.
- Jailbreak, passcode removal, activation-lock bypass, firmware downgrade, restore, or factory reset.
- Deleting or modifying local backups.
- Automatically killing or disabling Windows/Apple services.

## Why this boundary exists

A normal Windows desktop application cannot use a supported universal API to rewrite internal iOS performance policy. Device visibility through SetupAPI confirms only that Windows exposes a corresponding PnP node. It does not prove that iOS pairing is complete, nor does it expose battery health, temperature, storage breakdown, or process state.

The application therefore gives diagnostic evidence and directs the user to supported Apple workflows for backup, update, and device care.

## Product promise

ToolzNG is a diagnostic and safe-maintenance assistant. It does not claim to overclock iOS or clear private system caches. Keep UI labels, README copy, and release notes aligned with this limitation.
