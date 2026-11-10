# SAMBU engineering core in ToolzNG

**Integration baseline:** ToolzNG 0.5.x, C17 / Win32  
**Core revision:** 1.2.0  
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

## Memory, privacy and compatibility

The ledger retains only numeric finding codes, occurrence counters and monotonic timestamps. It is bounded to 32 distinct codes; repeated finding transitions increment a lifetime counter and an active 10-minute episode window. Repeated UI refreshes of an unchanged finding do not create new episodes. Patterns are labelled single (1), recurrent (2–3), or persistent (4+). The ledger is in-memory only and resets when the application exits, matching SAMBU's explicit-export memory model.

The report schema is additive and versioned. Consumers should rely on stable finding codes and schema version, not on Russian display strings. The SHA-256 sidecar detects later file changes; it does not prove that the observations themselves are true.

## Verification

The CTest target checks:
- fail-closed capability rules;
- incomplete-scan handling, evidence coverage and normalized score contributions;
- distinction between unknown service/backup/stability states and verified negative findings;
- finding priority for stale backups and unstable USB;
- bounded/deduplicated ledger behavior;
- recurrence windows and single/recurrent/persistent pattern classification;
- dependency-graph validation;
- stable engine and report identifiers.

The portable C17 engine can be built and tested without the Win32 GUI (for example, in Linux CI):

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Build the desktop application and run the same tests on Windows with:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

## Explicit non-goals

This core does not discover private Apple protocols, bypass pairing or Activation Lock, modify iOS, repair Windows drivers automatically, inspect personal backup contents, upload data, or run arbitrary plugins. No SAMBU Python component is represented as a native ToolzNG feature.
