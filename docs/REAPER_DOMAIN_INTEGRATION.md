# Engineering Pattern Integration — ToolNG

**Role:** Windows device diagnostics and evidence reporting  
**Reference catalogue:** [SAMBU — 50 engineering patterns](https://github.com/bboroski/SAMBU/blob/main/data/reaper/engineering_patterns_50.tsv)  
**Related technical case:** [Cyclic fault and signal-risk study](REAPER_CYCLE_CASE_STUDY.md)

## Diagnostic mapping

| Technical domain | ToolNG use |
|---|---|
| Signal sampling | Threat-model low-amplitude, stale, recurrent or contradictory observations without inferring a hidden cause |
| Dependency graph | Explain diagnostic coverage and identify unavailable observation paths |
| Access boundaries | Keep read-only behavior, privilege boundaries and evidence redaction explicit |
| Lifecycle recurrence | Detect repeated finding transitions over a bounded observation window |
| Integrity verification | Preserve provenance, stable finding codes and integrity checks for local reports |

## Required behavior

1. Distinguish `observed`, `not_found`, `unknown`, `failed` and `passed`; missing evidence must never become a healthy result.
2. Treat recurrence as a count of observed state transitions, not as device-failure probability or root-cause proof.
3. Keep report data local and redact hardware identifiers and private paths unless specifically required.
4. Keep recommendations advisory. This catalogue never authorizes repair, firmware flashing, reset, privilege escalation or network upload.

## Acceptance

Existing C17/Win32 checks remain authoritative. Any new signal class requires a documented source, timestamp semantics, confidence/coverage meaning and tests for unknown or failed collection.
