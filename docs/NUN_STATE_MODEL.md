# NUN-inspired state and resource model

**Status:** engineering design model, not a new runtime or a claim about mythology as science.  
**Applies to:** ToolzNG 0.6.x · portable SAMBU C17 core · Windows diagnostic host.  
**Core revision:** 1.4.0  
**Report schema:** `toolzng.diagnostic.v1` (unchanged).

## Purpose

The supplied account of Nun describes a primordial body of water, the conditions from which ordered creation emerges, the complementary pair Nun/Naunet, recurring water and solar cycles, and the solar barque. ToolzNG uses these motifs only as a design metaphor for a bounded, deterministic diagnostic lifecycle.

The engineering translation is deliberately literal: initialize from a known state, collect only observable evidence, validate it, retain a small amount of sanitized history, derive a reproducible assessment, and expose the result to the user. No deity, ritual, supernatural force, quantum mechanism or mythological claim is implemented in software.

## Concept-to-system mapping

| Motif in the supplied text | Engineering principle | Existing ToolzNG boundary |
|---|---|---|
| Nun as primordial waters | A minimal substrate from which higher-level work can proceed | Portable SAMBU C17 core; Win32 APIs stay in the platform host |
| Nun and Naunet as a complementary pair | Keep observations distinct from collection context and policy | Evidence structures are separate from Windows adapters and authorization |
| Life-giving potential in the waters | Treat collected signals as inputs, not as conclusions | `sambu_assess()` awards points only for completed checks |
| Cycles, flooding and seasonal recurrence | Model time windows, repeated transitions and stale observations explicitly | USB Pulse, monotonic recurrence windows and exponential relaxation |
| Ra emerging and beginning ordered creation | Derive a stable result from validated inputs | Deterministic readiness assessment and stable finding identifiers |
| Solar barque | Move a bounded set of operations through a controlled sequence | SAMBU workflow preflight and one-allowlisted-action-per-tick dispatch |
| No dedicated temples or cult sites | Avoid hidden global dependencies and mandatory external services | Explicit state objects, local-first processing and no required cloud account |

## Reliability rules

### 1. Bounded evidence memory

The finding ledger is a fixed 32-entry ring. It stores numeric finding codes, occurrence counters and monotonic timestamps; it does not retain hardware identifiers, backup paths, personal files or backup payloads.

The new `sambu_ledger_validate()` contract checks:

- the used-entry count does not exceed capacity;
- the ring cursor is valid and, before the ring fills, points to the next free slot;
- every used entry has a non-zero finding code and valid counters;
- the recent-window count cannot exceed the lifetime count;
- the most recent timestamp is not earlier than the active window start;
- a finding code is not duplicated among active entries.

Mutation APIs become no-ops for a malformed ledger. Read APIs return their neutral result rather than indexing outside the fixed array. Callers can use `sambu_ledger_validate()` to distinguish a valid empty ledger from an invalid structure.

### 2. Explicit time lifecycle

Recurrence remains measured against a 10-minute monotonic window. An expired window or a backward clock jump starts a new observation episode; it does not fabricate continuity across incomparable timestamps.

The exponential relaxation filter accepts only a valid lifecycle marker. An initialized state must have at least one sample and finite mean/variance values, with non-negative variance. Invalid state or non-finite calculations are rejected without committing partial updates.

These are deterministic statistical filters over observed USB events, not machine learning and not a claim about physical water or solar cycles.

### 3. Evidence before interpretation

Unknown, not found and failed remain different states. An unavailable scan does not prove device absence; a denied service query does not prove that the service is stopped; an incomplete backup enumeration does not prove that no backup exists.

No result may be promoted from provisional to complete merely because the available subset scored 100%.

### 4. Controlled transitions

A workflow is compiled and preflighted before execution. The dispatcher runs no more than one allowlisted action per tick. A host failure stops later work while preserving already collected step evidence. The mythological “barque” metaphor therefore maps to an ordinary bounded state machine, not autonomous system control.

### 5. Safety and user control

- Device mutation and network upload remain denied.
- Reports, clipboard access and export remain explicit user-origin operations.
- The portable core remains independent of Win32 and does not create hidden background services.
- The report schema and readiness weights are unchanged by this revision.

## Implemented in core revision 1.4.0

1. Added a public ledger invariant validator.
2. Guarded ledger insertion and lookup against malformed counters, cursors, duplicate codes and impossible time-window state.
3. Rejected invalid relaxation lifecycle markers and initialized states with zero samples.
4. Added regression tests for malformed ring metadata, duplicate entries, counter ordering, timestamp ordering and invalid relaxation states.

## Verification

Run the portable test suite:

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

A passing portable suite validates the C17 core contracts. It does not by itself prove the Windows GUI, Apple driver installation, device pairing or physical USB cable behavior.

## Non-goals

This model does not add a general-purpose plugin system, dynamic allocator, network service, autonomous repair, iOS modification, private Apple protocol support, machine-learning classifier, new diagnostic score, or mythological branding to the user interface. The value is in stronger state boundaries and testable invariants—not decorative terminology.
