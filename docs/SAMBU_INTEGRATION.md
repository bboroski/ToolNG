# SAMBU engineering core in ToolzNG

**Integration baseline:** ToolzNG 0.6.1, C17 / Win32  
**Core revision:** 1.1.0 + workflow runtime 1.1.0  
**Report contract:** `toolzng.diagnostic.v1`

This integration ports engineering rules from SAMBU into ToolzNG as native C behavior. It does not embed the Python interpreter, copy SAMBU's runtime, or claim that ToolzNG can control private iOS functions.

## The five governing letters

| Letter | SAMBU law | ToolzNG implementation |
|---|---|---|
| S | Safety and staged sovereignty | A fail-closed action gate. Read-only inspection is available to the application; report writing, clipboard use and export require a user-origin action. Device mutation and network upload remain denied. |
| A | Adaptive exploration | A deterministic assessment selects the highest-priority next step from measured Windows evidence. It is a rules engine, not a machine-learning claim. |
| M | Managed recovery and memory | A 32-entry in-memory ledger deduplicates finding codes and tracks first/last observation plus occurrence count. It never stores raw Hardware IDs, backup paths, source text or device content. |
| B | Branchable, board-layered design | The assessment/policy/ledger core is isolated in portable `sambu_engine.c/.h`. A dependency graph is validated before the Win32 window starts. Presentation and export are separate graph branches. |
| U | Unforced interoperability and user sovereignty | Stable finding IDs and a versioned report schema support future consumers. Export stays local and user initiated; there is no required account, cloud service or external runtime. |

The laws form a repeatable loop: **S** constrains the action → **A** evaluates evidence → **M** remembers a bounded result → **B** isolates the implementation → **U** preserves the user's ability to inspect and export it.

## Articulatory families mapped to ToolzNG

SAMBU's articulatory syntax defines ten consonant families. ToolzNG uses each family as an architectural lens:

| Family | SAMBU semantic field | Concrete ToolzNG role |
|---|---|---|
| K | Structure | The separate C17 engine module and validated dependency graph |
| T | Flow | The ordered diagnostic path and next-step selection |
| R | Value | Typed evidence fields, integer bounds and stable finding codes |
| M | Memory | The bounded, deduplicating finding ledger |
| S | Safeguard | Fail-closed permissions and prohibition of automatic device mutation |
| V | Measure | Readiness score, confidence and evidence bitmask |
| D | Evidence | Stable assessment identifiers, report schema and SHA-256 report seal |
| N | Coordination | Bounded USB event observation and local PulseVault journal |
| P | Transform | Explicit conversion of assessment results into a local report or user-selected export |
| L | Language | Stable machine identifiers separated from Russian presentation text, leaving room for RU/EN labels |

The five vowel grades also map cleanly to a diagnostic lifecycle:

- **A — initiate:** begin a user-requested scan.
- **E — inspect:** collect Windows-visible PnP, service and backup-marker evidence.
- **I — transform:** calculate readiness and evidence flags.
- **O — compose:** select one prioritized finding from the collected signals.
- **U — complete:** produce a local report or export only after an explicit user action.

The four grammatical endings are treated as API design rules: `-va` names an operation, `-ven` a resource or entity, `-vir` a dependency/relationship, and `-vum` an observed state/result. They inform naming and boundaries; they are not inserted as decorative syntax into C.

## Assessment contract

The engine uses the established, explainable readiness weights:

| Signal | Contribution |
|---|---:|
| Apple USB interface visible after a completed scan | 40 |
| Apple Mobile Device Service running | 25 |
| Local backup directory found | 10 |
| Expected backup marker files present | 15 |
| USB watch registered, warm-up complete and no recent removals | 10 |

A scan failure is classified as **unknown**, not as proof that no device exists. The finding priority is deterministic: scan unavailable → device not visible → Apple service unavailable → unstable USB → invalid backup markers → stale backup marker → backup not verified → Windows-side checks passed.

Confidence is 40% when device scanning is unavailable, 85% when the scan completes but the USB watcher is unavailable, and 100% when both are available. This confidence is a completeness indicator for these local checks, not a probability that the iPhone is healthy.

## Memory, privacy and compatibility

The ledger retains only numeric finding codes, occurrence counters and monotonic timestamps. It is bounded to 32 distinct codes; repeated observations increment a counter. It is in-memory only and resets when the application exits, matching SAMBU's explicit-export memory model.

The report schema is additive and versioned. Consumers should rely on stable finding codes and schema version, not on Russian display strings. The SHA-256 sidecar detects later file changes; it does not prove that the observations themselves are true.

## Verification

The CTest target checks:
- fail-closed capability rules;
- incomplete-scan handling and score calculation;
- finding priority for stale backups and unstable USB;
- bounded/deduplicated ledger behavior;
- dependency-graph validation;
- stable engine and report identifiers.

Build the application and run the tests on Windows with:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

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
