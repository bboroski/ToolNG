# Engineering upgrade baseline — ToolNG

## Scope
Windows read-only device evidence probe and local diagnostics.

## Required engineering changes
1. **Contracts and typing:** validate public inputs at module/API boundaries; version externally consumed schemas and ABI; reject malformed or unsupported versions explicitly.
2. **Runtime and memory:** profile before optimizing; document ownership, lifetime, bounds and failure behavior; use bounded reuse/pooling only where allocation measurements justify it.
3. **Interfaces and events:** isolate adapters behind stable interfaces; keep asynchronous work bounded, observable and non-blocking where the platform supports it; define overload and shutdown behavior.
4. **Performance:** establish repeatable baselines for latency, throughput and peak memory; optimize measured hot paths; add SIMD only with scalar fallback and equivalence tests.
5. **Reliability:** add boundary, malformed-input, fault-injection and stress tests; preserve deterministic behavior and explicit degraded/unknown states.
6. **Configuration and migration:** version persisted configuration; validate before applying; make migrations idempotent, transactional where possible, and rollback-capable.
7. **CI/CD:** require build, tests, static checks and migration validation before merge; auto-merge may be enabled only after repository protections and required checks are verified.

## Repository-specific constraints
Keep all device operations read-only. Add schema validation for local reports, bounded non-blocking telemetry/event queues, redaction tests and explicit unknown/partial states. Never turn a recommendation into a device mutation.

## Verification gates
CMake configure/build; one.py check; Windows CI for supported SDK/toolchain; tests with absent, partial and malformed device evidence.

## Delivery policy
Implement changes in small independently reviewable PRs. Keep `main` unchanged until checks pass and review is complete. Record benchmark environment and results; do not claim hardware/production readiness from host-only tests.

## Status
This PR establishes the repository-specific acceptance baseline. It does not claim that the runtime optimizations above have already been implemented; each implementation must land with code, tests and measured evidence.
