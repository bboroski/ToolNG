# Reaper-Domain Integration — ToolNG

**Role:** Windows device diagnostics and evidence reporting  
**Canonical catalogue:** [SAMBU — 50 curated engineering patterns](https://github.com/bboroski/SAMBU/blob/main/data/reaper/engineering_patterns_50.tsv)  
**Related study:** [Reaper cycle case study](REAPER_CYCLE_CASE_STUDY.md)

## What transfers into ToolNG

| Reaper concept family | ToolNG engineering use |
|---|---|
| Sub-perceptual signals and indoctrination | Threat-model low-amplitude, stale, recurrent, or contradictory observations without inferring a hidden cause |
| Relay network and Citadel dependency | Explain diagnostic dependency coverage and identify unavailable observation paths |
| Kinetic barriers and ablative protection | Keep read-only boundaries, privilege boundaries, and evidence redaction explicit |
| Harvest cycles and evolutionary deadlocks | Detect repeated finding transitions over a bounded observation window |
| AI consensus and code integrity | Preserve provenance, stable finding codes, and integrity checks for local reports |

## Required behavior

1. Distinguish `observed`, `not_found`, `unknown`, `failed`, and `passed`; missing evidence must never be promoted to a healthy result.
2. Treat recurrence as a count of observed state transitions, not as a probability of device failure or proof of root cause.
3. Keep report data local and redact hardware identifiers and private paths unless specifically required.
4. Recommendations remain advisory. This domain pack never authorizes repair, firmware flashing, reset, privilege escalation, or network upload.

## Acceptance

Existing C17/Win32 checks remain authoritative. Add no fake Reaper devices, fictional sensors, or lore-specific product modes. Any new signal class must have a documented source, timestamp semantics, confidence/coverage meaning, and tests for unknown or failed collection.
