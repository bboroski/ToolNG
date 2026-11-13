# LOVE-50 adapter — ToolNG

**Repository:** `bboroski/ToolNG`  
**Role:** Local-first, read-only Windows device-evidence diagnostics  
**Status:** SPECIFIED — this mapping is guidance; it does not certify runtime conformance.  
**Canonical doctrine:** [Klipot LOVE-50](https://github.com/bboroski/Klipot/blob/feat/love-50-chinese-proverbs-20261113/docs/LOVE_50_CHINESE_DOCTRINE.md)  
**Related standard:** the repository's existing engineering and safety standards remain authoritative.

## Why this profile exists

ToolNG's strongest expression of care is telling the operator exactly what was observed, what remains unknown and what is safe to do next. The doctrine reinforces the current read-only and local-first boundary.

## Repository-specific application

### Fate / compatibility
Represent device, service and backup signals with explicit source, timestamp, scope and coverage. Keep unsupported devices distinct from devices that were checked and found absent.

### Fidelity / trust
Do not mutate drivers, services, devices or backups. Keep report fields redacted by default and preserve the boundary between marker-file presence and actual backup integrity.

### Responsiveness / care
Show progress and bounded timeouts for device enumeration. Every recommendation should cite the observed signal and provide a reversible operator-led next step.

### Harmony / integration
Keep correlation between PnP observations, service state, USB arrival/removal and local report generation explicit. Fail one probe independently rather than fabricating a global healthy state.

### Wisdom / restraint
Unknown is not healthy; a readiness score is provisional when evidence coverage is incomplete. No cloud upload, private device-content inspection or automatic repair.

## Local verification

- `cmake -S . -B build`
- `cmake --build build --config Release`
- `ctest --test-dir build --output-on-failure`
- `python one.py check`
- `For probe changes, test unavailable APIs, permission denial, unplug during scan, redaction and incomplete evidence.`

## Non-goals and safety boundaries

- No service shutdown, driver changes, backup edits or device repair.
- No claim of backup completeness, restore-ability or iOS health from marker-file checks.
- No expansion of collected identifiers without a documented need and privacy review.

## Adoption state

This document establishes a repository-specific interpretation only. Promote any item to **IMPLEMENTED** after the relevant code or configuration exists; promote it to **VERIFIED** only after the repository-native checks and target-specific evidence pass. Keep unknown, planned, partial and failed states distinct. Do not weaken existing security, privacy, ABI, platform, licensing or release requirements to satisfy a metaphor.

---
*The Chinese expressions are engineering prompts, not claims that software has human emotions.*
