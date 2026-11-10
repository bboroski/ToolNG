# Closed-Domain Boundary Profile — ToolNG

**Role:** read-only Windows device diagnostics and local evidence reporting.

## Transfer into ToolNG

Treat the bounded domain as the diagnostic session and its explicitly selected evidence sources—not as a physical spacetime model.

| CDM-1 concept | ToolNG feature contract |
|---|---|
| Impermeable boundary | Read-only collection boundary; no device repair, reset, flashing, or configuration writes |
| Fixed boundary data | Snapshot the selected device identifiers, collector versions, and policy before collection |
| Exchange channels | Report whether local collection, filesystem output, and network transfer are enabled; default to no network egress |
| Metric deformation | Track bounded changes in observed device/service state over a declared collection window |
| Energy/invariant ledger | Count requested, completed, unavailable, failed, and redacted observations |
| Boundary stress | Report collector timeout, API denial, access failure, or resource-limit pressure without inventing a device fault |

## Required invariants

1. A report scope is explicit: selected sources, collection window, permissions, and output destination.
2. Collection remains read-only. A diagnostic result is not authorization to mutate a device.
3. Preserve `observed`, `not_found`, `unknown`, `failed`, and `passed` as distinct states.
4. Local report integrity metadata detects accidental modification only; it is not a digital signature or proof of source authenticity.
5. Missing observations reduce coverage; they must not be converted into a healthy/closed state.
6. Resource limits and cancellation must terminate collection without leaking handles or partially publishing a success report.

## Acceptance tests to add

- Deny an unlisted source and any attempted write operation.
- Verify that a missing collector produces `unknown` or `failed`, not `passed`.
- Verify that report export preserves source, timestamp, redaction, and coverage metadata.
- Verify that collection cancellation closes owned handles and never emits a complete-report status.
- Verify that network egress remains disabled unless a separately reviewed feature explicitly enables it.

## Scope

CDM-1 is an optional research vocabulary and configuration reference. It does not introduce metric tensors, mass calculations, fictional sensors, or new hardware-control powers into ToolNG.
