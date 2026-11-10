# SAMBU engineering core in ToolzNG

**Integration baseline:** ToolzNG 0.6.x, C17 / Win32  
**Core revision:** 1.2.0  
**Report contract:** toolzng.diagnostic.v1

This integration ports selected engineering rules from SAMBU into ToolzNG as native C behavior. It does not embed the Python interpreter, copy SAMBU's host runtime, or claim that ToolzNG controls private iOS functions.

## The five governing letters

| Letter | SAMBU law | ToolzNG implementation |
|---|---|---|
| S | Safety and staged sovereignty | Fail-closed capability gate. Unknown operation and unknown origin are rejected. Device mutation and network upload are always denied. |
| A | Adaptive exploration | Deterministic assessment from measured Windows evidence; no machine-learning claim. |
| M | Managed recovery and memory | Bounded 32-entry finding ledger with monotonic timestamps and a 10-minute recurrence window. Raw Hardware IDs, backup paths and device payloads are not stored in the ledger. |
| B | Branchable, board-layered design | Portable C17 assessment core, policy gate, platform adapter, memory, presentation and export layers. Dependency graph validated before the Win32 window starts. |
| U | Unforced interoperability and user sovereignty | Stable finding IDs, additive report evidence fields and local user-initiated export. No account or cloud service required. |

The laws form a repeatable loop: S constrains the action → A evaluates evidence → M remembers a bounded result → B isolates the implementation → U preserves the user's ability to inspect and export it.

## SAMBU Quality Gate — revision 1.2.0

The 1.2.0 assessment validates evidence before awarding points.

### Typed backup evidence

SambuEvidence now carries exact discovered and valid folder counts:

- backup_folders_seen: number of discovered backup folders inspected;
- backup_folders_valid: number of inspected folders with both expected marker files;
- backup_marker_valid: compatibility indicator that at least one discovered folder passed marker checks.

A complete marker set requires a backup root, at least one discovered folder, consistent counters, at least one valid folder, and equality between valid and discovered counts. Partial validation never receives the full marker bonus.

The invariant is:

    backup_folders_valid <= backup_folders_seen
    (backup_folders_valid > 0) == backup_marker_valid
    valid backup set = backup root exists AND seen > 0 AND valid == seen

### Integrity flags

| Flag | Meaning | Effect |
|---|---|---|
| DEVICE_WITHOUT_SCAN | A device signal is present without a completed scan. | Device visibility is not credited; confidence becomes 25%. |
| BACKUP_COUNTS_INCONSISTENT | Counts or compatibility marker disagree. | Backup marker evidence is not credited; confidence becomes 25%. |
| BACKUP_SET_INCOMPLETE | One or more discovered backup folders lack expected markers. | Full marker bonus is withheld; finding becomes backup structure; confidence becomes 25%. |
| STABILITY_WITHOUT_WATCH | A stability window elapsed while no USB watcher was registered. | Stability evidence and bonus are withheld; confidence becomes 25%. |

More than one flag maps to multiple_integrity_warnings. A zero flag set maps to clean.

### Confidence mapping

| Condition | Confidence |
|---|---:|
| Completed scan, active watcher, no integrity warnings | 100% |
| Completed scan, watcher unavailable before the observation window expires, no integrity warnings | 85% |
| Scan incomplete, no integrity warnings | 40% |
| Any Quality Gate integrity warning | 25% |

Confidence is a data-completeness and consistency indicator for this local diagnostic procedure. It is not a statistical probability that the iPhone is healthy.

### Score contract

| Signal | Contribution |
|---|---:|
| Completed scan and visible Apple USB interface | +40 |
| Apple Mobile Device Service running | +25 |
| Backup root found | +10 |
| Every discovered backup folder has expected markers | +15 |
| Device confirmed, watcher active, observation window elapsed and no recent removals | +10 |

The maximum remains 100. A partial marker set does not receive the +15 marker credit. A scan-incomplete device signal does not receive the +40 device credit. A stability window without its watcher does not receive the +10 stability credit.

Finding priority remains deterministic: scan unavailable → device not visible → Apple service unavailable → unstable USB → incomplete backup structure → stale verified backup marker → backup not verified → Windows-side checks passed.

## Articulatory families mapped to ToolzNG

| Family | SAMBU semantic field | Concrete ToolzNG role |
|---|---|---|
| K | Structure | Separate C17 engine module and validated dependency graph |
| T | Flow | Ordered diagnostic path and next-step selection |
| R | Value | Typed evidence fields, integer bounds and stable finding codes |
| M | Memory | Bounded, deduplicating finding ledger |
| S | Safeguard | Fail-closed permissions and prohibition of automatic device mutation |
| V | Measure | Readiness score, confidence, evidence mask and quality flags |
| D | Evidence | Stable assessment identifiers, report schema and SHA-256 report seal |
| N | Coordination | Bounded USB event observation and local PulseVault journal |
| P | Transform | Explicit conversion of assessment results into a local report or user-selected export |
| L | Language | Stable machine identifiers separated from user-facing text |

The five vowel grades map to the diagnostic lifecycle:

- A — initiate: begin a user-requested scan.
- E — inspect: collect Windows-visible PnP, service and backup-marker evidence.
- I — transform: calculate readiness, evidence flags and quality warnings.
- O — compose: select one prioritized finding from the collected signals.
- U — complete: produce a local report or export only after an explicit user action.

The four grammatical endings inform API design roles: operation, resource/entity, dependency/relationship, and observed state/result. They are not inserted as decorative syntax into C.

## Memory and recurrence

The ledger retains numeric finding codes, occurrence counters and monotonic timestamps. It is bounded to 32 distinct codes. Repeated transitions increment lifetime count and an active 10-minute episode window. Repeated UI refreshes of an unchanged finding do not create new episodes. Patterns are single (1), recurrent (2–3), or persistent (4+).

The ledger is in-memory only and resets when the application exits. It does not retain raw Hardware IDs, backup paths, source text or device payloads.

## Compatibility and reporting

The report schema remains toolzng.diagnostic.v1. SAMBU Engine 1.2.0 adds readiness and evidence quality fields to the existing text report; these fields are additive. Consumers should rely on stable finding keys and schema identity, not on localized Russian display strings.

SHA-256 sidecar detects later changes to the saved report. It does not prove that the recorded observations were true.

## Verification

The CTest target checks:

- fail-closed capability rules, including unknown origins;
- incomplete-scan handling and score calculation;
- finding priority for stale backups and unstable USB;
- backup counter consistency and all-folders-complete semantics;
- quality flags for incomplete backup sets and absent USB watcher;
- bounded / deduplicated ledger behavior;
- recurrence windows and single / recurrent / persistent classification;
- dependency graph validation;
- stable engine, finding and report identifiers.

Portable build:

~~~bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
~~~

Windows x64 build:

~~~powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
~~~

## Explicit non-goals

This core does not discover private Apple protocols, bypass pairing or Activation Lock, modify iOS, repair Windows drivers automatically, inspect personal backup payloads, upload data, run arbitrary plugins or perform hardware cable diagnostics. No SAMBU Python component is represented as a native ToolzNG feature.
