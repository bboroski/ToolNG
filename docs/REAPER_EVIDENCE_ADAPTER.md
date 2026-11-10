# ToolNG — Reaper cycle evidence boundary

## Product fit

ToolNG is a Windows device-evidence probe. Its Reaper-themed integration is deliberately read-only: it can inspect local execution evidence and report whether a simulation artifact or report exists, but it must not execute firmware, manipulate device state, tune clocks, alter power rails, or perform manufacturing operations.

## Evidence states

Map the simulation report into ToolNG's existing evidence vocabulary:

- confirmed — a report was read and its schema, declared model version, and required fields passed validation;
- unknown — the report was not available or a required check could not be completed;
- invalid — the report was present but malformed or internally inconsistent;
- not applicable — the selected device workflow has no simulation artifact.

Do not convert missing telemetry into a negative hardware finding. Do not infer physical CPU temperature, voltage, or readiness from synthetic model values.

## Required safeguards

- Keep local inspection under ToolNG's existing authorization policy.
- Keep export and clipboard actions explicitly user-initiated.
- Never run a ReaperCripple executable as part of a passive device scan.
- Redact sensitive paths and hardware identifiers in exported evidence.
- Label every model-derived field simulated and retain provenance.

## Relationship

The bounded reference model lives in ReaperCripple. ToolNG consumes reports only; it does not own the simulator or device control.
