# Multi-Axis Layer Architecture

**Architecture:** NUN Mesh — Multi-Axis Layer Fabric (MALF)  
**Repository:** `bboroski/ToolNG`  
**Role:** Read-only Windows device and host evidence diagnostics  
**Maturity:** C17/Win32 desktop utility; real device and OS scenarios require Windows target evidence.  
**Status:** Target boundary contract; implementation claims remain evidence-based.

## Why this model

MALF is not a folder-renaming exercise. It is a multi-axis design: structural layers, vertical feature slices, trust boundaries, lifecycle stages and independent assurance/observability planes. Existing source layout stays intact until a tested refactor has a clear benefit.

## Structural layers

| ID | Layer | Scope |
|---|---|---|
| L0 | **Experience** | CLI, GUI and operator experience; presentation only. |
| L1 | **Interface contracts** | APIs, ABIs, schemas, events and versioned messages. |
| L2 | **Application orchestration** | Use cases, workflow ordering, cancellation and transaction boundaries. |
| L3 | **Domain kernel** | Core models, invariants and deterministic algorithms. |
| L4 | **Policy and capabilities** | Authorization, safety gates, resource limits and allowed operations. |
| L5 | **Ports and contracts** | Abstract clocks, storage, devices, networks, compilers and platform services. |
| L6 | **Adapters and platform** | Concrete OS, board, browser, hardware and external-service implementations. |
| L7 | **State and data** | Schemas, registries, persistence, caches, artifacts and provenance. |
| L8 | **Runtime and delivery** | Build, boot, process composition, packaging, deployment and recovery. |
| L9 | **Assurance and evidence** | Tests, static analysis, benchmarks, diagnostics and qualification evidence. |

A layer is a responsibility boundary, not a required directory. Small components may combine layers; complexity should be split only where ownership, testing, isolation or deployment benefits.

## Dependency law

- L0 calls use cases at L2 through L1. Presentation owns no domain decisions.
- L2 coordinates L3 and L4 through L5 ports.
- L6 implements L5. Domain code must not import OS, UI, vendor SDK or device-specific adapters.
- L7 is accessed through explicit storage/repository ports with versioned schemas.
- L8 composes the system and depends inward; inner layers never depend on deployment or a specific host.
- L9 tests boundaries as well as outputs; tests do not make an unimplemented feature real.

**Allowed:** surface → application → domain/policy → port → adapter/platform.  
**Forbidden:** domain → GUI/OS/vendor adapter; hidden global side effects; unversioned cross-repository source coupling.

## Orthogonal axes

1. **Structural:** L0–L9 responsibility map.
2. **Vertical slice:** one end-to-end capability from input to decision, adapter, output/state and tests.
3. **Trust:** untrusted input → parse/normalize → validate → policy gate → explicitly authorized effect → auditable result.
4. **Lifecycle:** design → configure → build → verify → package → deploy/boot → operate → recover/rollback → retire.
5. **Observability:** bounded, redacted diagnostics with origin, timestamp and confidence; measurements prove only what was observed.

## Repository-specific allocation

### L0 — Operator experience
Native Windows views, device inventory, readiness summaries and local reports. UI does not mutate devices.

### L1 — Diagnostic contracts
Typed observation, evidence, result, status and report structures; stable command/configuration contracts.

### L2 — Read-only workflows
Enumerate → normalize → inspect → correlate → assess evidence completeness → report.

### L3 — Evidence domain
Device identity, signal normalization, evidence provenance, confidence, coverage and operator findings.

### L4 — Read-only safety policy
Deny writes, driver/service mutation, private backup-content access, network upload and bypass operations. Redact sensitive fields.

### L5 — Observation ports
Abstract PnP, USB, service, event, clock and local-report interfaces.

### L6 — Windows adapters
Win32 PnP/USB, service, event and filesystem readers; isolate OS-version differences and partial permissions.

### L7 — Local evidence state
Bounded event history, normalized observations, local reports and redaction settings.

### L8 — Windows delivery
CMake/C17 build, Windows packaging and supported-host checks.

### L9 — Diagnostics assurance
CTest, Windows CI, mocked signal tests, read-only regression checks, redaction checks and real-device coverage matrix.

## Vertical slices

1. **Device inventory:** UI request → scan workflow → observation ports → Win32 adapter → normalized evidence → redacted report.
2. **Readiness assessment:** Signals → completeness policy → separate confidence and coverage → explainable guidance.
3. **Event history:** OS event source → bounded collector → normalize/redact → local store → operator timeline.

## Cross-cutting planes

- **Security/trust:** least privilege, explicit consent, default-deny for unknown effects, secrets hygiene and validated inputs.
- **Resilience:** bounded memory/time/work, explicit degraded/fault states, cancellation and rollback where feasible.
- **Compatibility:** version public API/ABI, manifests, schemas and persisted formats.
- **Observability:** provenance, confidence, redaction and bounded retention.
- **Accessibility:** keyboard, focus, contrast and reduced-motion behavior on user-facing surfaces.
- **Performance:** measure the real target path before optimizing; host benchmarks do not qualify hardware.

## NUN Mesh federation

Repositories stay independently buildable and releasable. A link between repositories is a versioned contract, not an implicit runtime dependency. Each integration declares provider, consumer, contract/version, optionality, failure behavior, trust boundary and evidence. A manifest or “adapter-ready” label is not proof of integration.

## Incremental adoption

1. Map existing files and APIs to layers without moving code.
2. Find cycles, hidden globals, direct platform calls and unguarded effects.
3. Stabilize L1/L5 contracts before swapping implementations.
4. Isolate deterministic L3 logic from UI and platform details.
5. Gate sensitive effects at L4; implement concrete adapters at L6.
6. Version L7 data and L8 composition.
7. Add L9 checks for boundaries, error paths, resource limits and target behavior.
8. Migrate one vertical slice at a time; document compatibility and rollback.

## Definition of done

The owning layer is explicit; dependencies point inward; contracts are versioned; policy and failure behavior are tested; verification matches the claimed maturity; and documentation labels work as **implemented**, **partial**, **planned** or **not verified**.

## Repository guardrails

ToolNG remains read-only. No hidden repair, driver/service changes, activation-lock bypass, private backup parsing or automatic cloud upload. Signal absence is not a confirmed negative; insufficient permission reduces coverage.
