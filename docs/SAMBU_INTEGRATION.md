# SAMBU engineering core in ToolzNG

**Integration baseline:** ToolzNG 0.6.1, C17 / Win32  
**Core revision:** 1.1.0 + workflow runtime 1.1.0  
**Integration baseline:** ToolzNG 0.6.1, C17 / Win32  
**Core revision:** 1.2.0  
**Report contract:** `toolzng.diagnostic.v1`

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
| DEVICE_WITHOUT_SCAN | A device signal is present without a completed scan. | Do not credit device visibility; the 40-point scan weight remains unmeasured. |
| BACKUP_COUNTS_INCONSISTENT | Folder counts or the marker compatibility flag disagree. | Withhold marker points and expose an integrity warning; do not rewrite coverage to an arbitrary constant. |
| BACKUP_SET_INCOMPLETE | At least one discovered backup folder lacks expected markers. | Withhold the +15 complete-marker credit; report backup structure as incomplete. |
| STABILITY_WITHOUT_WATCH | An observation window elapsed without a registered USB watcher. | Do not credit the stability weight; the 10-point stability check remains unmeasured. |

Multiple flags map to `multiple_integrity_warnings`; no flags map to `clean`. Quality flags describe consistency problems and are separate from evidence coverage.

### Evidence coverage and normalized readiness

The engine sums the weights of checks that actually completed. The coverage percentage is that measured weight divided by 100. The `confidence_percent` field remains as a backward-compatible alias for this coverage; it is not a statistical probability.

| Measurement state | Coverage contribution |
|---|---:|
| Completed Windows PnP scan | 40% |
| Completed Apple service query | 25% |
| Completed backup directory enumeration | 25% |
| Registered USB watcher plus elapsed observation window for a visible device | 10% |

Readiness is normalized over measured weight: `earned points / measured weight × 100`. A completed check that finds an absent or incomplete item is a measured negative result; a check that could not run is unknown and contributes no coverage. Quality flags never force coverage to 25%.

Examples:
- A PnP scan that completes and finds no Apple device has 40% coverage and a zero readiness score.
- A completed scan with a visible device but an unreadable service and incomplete backup scan is provisional; those unavailable checks do not become verified failures.
- A fully completed backup scan that finds three folders but validates only one has measured coverage for backup, but receives no complete-marker bonus and raises `BACKUP_SET_INCOMPLETE`.
- A missing USB watcher leaves its 10-point weight unmeasured; it does not imply a faulty cable.

### Score contract

| Signal | Contribution |
|---|---:|
| Completed scan and visible Apple USB interface | +40 |
| Apple Mobile Device Service running | +25 |
| Backup root found | +10 |
| Every discovered backup folder has expected markers | +15 |
| Device confirmed, watcher active, observation window elapsed and no recent removals | +10 |

The maximum remains 100. A partial marker set does not receive the +15 marker credit. A scan-incomplete device signal does not receive the +40 device credit. A stability window without its watcher does not receive the +10 stability credit.

Finding priority remains deterministic: scan unavailable → device not visible → Apple service unknown → Apple service confirmed unavailable → unstable USB → backup scan unknown → incomplete backup structure → stale verified backup marker → backup not verified → USB stability unconfirmed → Windows-side checks passed.

## Articulatory families mapped to ToolzNG

| Family | SAMBU semantic field | Concrete ToolzNG role |
|---|---|---|
| K | Structure | Separate C17 engine module and validated dependency graph |
| T | Flow | Ordered diagnostic path and next-step selection |
| R | Value | Typed evidence fields, integer bounds and stable finding codes |
| M | Memory | Bounded, deduplicating finding ledger |
| S | Safeguard | Fail-closed permissions and prohibition of automatic device mutation |
| V | Measure | Readiness score, measured coverage, evidence mask and separate quality flags |
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

The engine retains the original signal weights but now distinguishes **verified negative** from **not measured**. The weights are not silently treated as zero when Windows could not complete a check.

| Signal | Maximum contribution | When it is measurable |
|---|---:|---|
| Apple USB interface visible | 40 | Windows PnP enumeration completed |
| Apple Mobile Device Service running | 25 | Service Control Manager query and service status call completed, or service absence was explicitly confirmed |
| Local backup directory found | 10 | Backup directory enumeration completed |
| Expected backup marker files present | 15 | Backup directory enumeration completed |
| USB connection stable | 10 | Device visible, USB watcher registered, and the warm-up window elapsed without removals |

The score is normalized over the measured weights: `earned points / measurable points × 100`. **Evidence coverage** is the sum of measurable weights, expressed as a percentage of the full 100-point model. If coverage is below 100%, the score is marked provisional. A perfect score with 40% coverage therefore means only that the measured portion passed; it is not a complete all-checks result.

The existing `confidence_percent` field remains as a compatibility alias for coverage. It is explicitly **not** a probability or a statistical confidence estimate.

The finding priority keeps uncertainty explicit: scan unavailable → device not visible → Apple service status unknown → service confirmed unavailable → unstable USB → backup scan incomplete → invalid backup markers → stale backup marker → backup not verified → USB stability unconfirmed → Windows-side checks passed.

SHA-256 sidecar detects later changes to the saved report. It does not prove that the recorded observations were true.

## Verification

The CTest target checks:
- fail-closed capability rules;
- incomplete-scan handling, evidence coverage and normalized score contributions;
- distinction between unknown service/backup/stability states and verified negative findings;
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

This core does not discover private Apple protocols, bypass pairing or Activation Lock, modify iOS, repair Windows drivers automatically, inspect personal backup contents, upload data, or run arbitrary plugins. No SAMBU Python component is represented as a native ToolzNG feature.

## SAMBU workflow runtime 1.1

The 1.1 workflow layer extends the five-letter core without embedding SAMBU's Python runtime. It consists of a bounded lexer/parser, a compiled workflow AST, a task manifest, a preflight verifier, a typed host callback table, and a one-step cooperative dispatcher.

### Grammar and limits

A workflow has one manifest and between one and eight ordered steps:

```sambu
workflow apple_connection {
  manifest {
    purpose "Windows-side Apple connection audit";
    max_steps 5;
    autonomy local_record;
  }
  step usb = observe.apple_usb;
  step service = observe.apple_service;
  step profile = record.link_profile;
  step backup = observe.backup_markers;
  step readiness = compute.readiness;
}
```

The source is capped at 3072 bytes; names and text literals have fixed capacities; the AST contains at most eight steps; duplicate names are rejected; every step stores byte offset, token length, line and column. The parser accepts whitespace and `#` / `//` line comments. It rejects unknown syntax and trailing source.

The allowlist is closed:

| Action | Host behavior |
|---|---|
| `observe.apple_usb` | Enumerate Windows-present PnP devices matching Apple USB VID `05AC` |
| `observe.apple_service` | Query Apple Mobile Device service state |
| `record.link_profile` | Compare or initialize the local LinkDNA digest; raw hardware identifiers are not retained by this operation |
| `observe.backup_markers` | Inspect expected local backup marker filenames, read-only |
| `compute.readiness` | Calculate the existing explainable Windows-side readiness assessment |

There is no action accepting a command string, launching a process, modifying iOS, deleting backups, uploading information or running an arbitrary plugin.

### Manifest and dual preflight

The task manifest declares a short purpose, an explicit maximum step count and one autonomy class: `observe_only` or `local_record`. The host policy independently caps the run at five steps and states whether observation and local profile recording are allowed.

Workflow preflight checks the parsed manifest, action allowlist and resource budget. A local profile write must also pass `sambu_authorize(SAMBU_OP_RECORD_LOCAL_PROFILE, SAMBU_ORIGIN_INTERNAL)`. This is a narrow application permission, not an HSM-backed signature, trusted external verifier or permission to mutate the Apple device. Report creation/export continues to require a user-origin action.

### Cooperative execution and evidence

`SambuWorkflowSession` owns a fixed-size copy of its program, policy, host binding, cursor and bounded result array. Each `sambu_workflow_tick()` invokes no more than one host action. ToolzNG schedules ticks with `WM_TIMER` on the existing UI thread; it does not create OS threads, native fibers or Python generators. A host failure stops the sequence immediately; later steps remain unexecuted.

Each evidence record contains a step number, allowlisted action enum, success state, source span, step name and a short host finding. The report writer appends the preflight decision, attempted/passed/failed counts and per-step findings to the local TXT report. No source text or raw device identifiers are added to the workflow ledger.

### Capability transfer matrix

| Capability family from the supplied SAMBU inventory | ToolzNG status |
|---|---|
| Lexer, parser and source-aware AST | Implemented for the restricted workflow grammar |
| Functions, scopes, general control flow, arrays and records | Not implemented as a general language runtime |
| OOP, interfaces, traits and game helpers | Not applicable to the Apple diagnostics host |
| Actor mailbox, supervision and sequential actor state | Not implemented; ordered workflow steps are not an actor runtime |
| Typed bounded channels | Not implemented |
| Python generator scheduler | Not embedded; Win32 timer tick is cooperative UI-thread dispatch only |
| Resource scope and dual-verifier gate | Local policy + workflow preflight exists; no external HSM verifier or general RAII scope |
| Bounded autonomy, task manifest, evidence ledger, preflight | Implemented for fixed local diagnostic actions only |
| SI/scientific helpers and sparse voxel/mesh algorithms | Not integrated into ToolzNG |
| Starter engine bridge | Not needed by this host |
| Judo/Aikido/Jiu-Jitsu, Magento XML inspection, scaffold generator | Not integrated; remain separate host/tooling domains |
| Native compiler, self-hosting, static type/effect/ownership checking, green fibers, multicore, LSP/IDE, Rails-style web stack | Planned language/toolchain capabilities; not claimed by ToolzNG |

### Failure semantics

Lexical, syntax, unknown-action, duplicate-name, source-size, step-budget and policy errors are rejected before dispatch. Host errors are reported against the originating step span and terminate the run. A failed workflow can still produce a user-requested diagnostic report containing the partial evidence already collected.
This core does not discover private Apple protocols, bypass pairing or Activation Lock, modify iOS, repair Windows drivers automatically, inspect personal backup payloads, upload data, run arbitrary plugins or perform hardware cable diagnostics. No SAMBU Python component is represented as a native ToolzNG feature.
