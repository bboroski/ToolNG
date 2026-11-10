# Reaper Domain Transfer Profile — ToolNG

**Status:** read-only evidence-domain design  
**Role:** Windows device observation, local evidence records and explainable quality gates

## Objective

Borrow the *Mass Effect* reference's vocabulary of signals, sensors, interference, relays, memory and system integrity as a classification lens for diagnostics. The Reapers remain fictional; ToolNG must never interpret lore terms as real Windows mechanisms.

## Transfer map

| In-universe phrase family | ToolNG engineering analogue | Required behavior |
|---|---|---|
| Sub-perceptual signal / signal relay | low-level or indirect observable | Record the actual Windows API and observation method; do not infer an unseen signal |
| Sensor spoofing / disruption | conflicting, incomplete or suspicious evidence | Mark the discrepancy and preserve both observations |
| Memory extraction / genetic archive | reportable evidence fields | Collect only declared diagnostic metadata; redact sensitive identifiers |
| Kinetic barrier / hull integrity | protection or integrity status | Report the tested condition and coverage, not a universal “safe” verdict |
| Distributed synthetic intelligence | multiple independent signal sources | Keep source identity, sequence and timestamps separate |

## Evidence record extension

A diagnostic observation should be traceable to: source ID, source kind, collection method, monotonic timestamp and timebase, sequence number, coverage, result state, redaction policy and integrity metadata. A digest can reveal later changes to a report; a non-cryptographic checksum is not proof of origin or authenticity.

Preserve ToolNG's existing distinctions:

- **Observed:** a check ran and produced a value.
- **Not found:** a check ran and the target was absent.
- **Unknown:** access or coverage was insufficient.
- **Failed:** the check itself returned an error.
- **Passed:** all declared requirements were measured and satisfied.

A spoofing fixture should use synthetic, deterministic test data. It must not tamper with real devices or Windows services.

## Safety boundaries

- Keep device inspection read-only. Do not add silent reset, repair, firmware, backup-content or account-data operations.
- Do not upload reports or raw identifiers. Redact before export.
- A suspicious signal is an anomaly requiring review, not proof of compromise.
- CRC-style checks detect accidental corruption only; use a separately reviewed cryptographic implementation for authenticity.
- Never describe indoctrination, neural rewiring or cognitive degradation as real diagnoses.

## Suggested validation

Add tests for duplicate source IDs, out-of-order sequence numbers, missing coverage, redaction, corrupted report sidecars, conflicting observations and stable serialization. The user-facing report must explain exactly which checks ran and which remained unknown.

This profile extends the evidence vocabulary; it does not claim that ToolNG currently detects Reaper signals, malware, or all classes of device compromise.
