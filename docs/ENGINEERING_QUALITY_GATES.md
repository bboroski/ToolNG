# ToolNG Engineering Quality and Visual Evidence Gates

**Baseline:** ToolNG 0.6.1, portable C17 engine/workflow and Windows-only Win32 desktop UI.  
**Purpose:** translate the SAMBU/ToolNG improvement brief into release gates that match the actual product boundary.

ToolNG is a local Windows-side Apple-device connection and backup-marker diagnostic utility. It is not a creative-text processor, an NLP service, an iOS repair tool or an automatic device-management system. The Python creative ontology belongs in SAMBU; ToolNG adopts only the engineering principles that are relevant to a native diagnostic application.

## 1. Hot Ice: change only what improves the system

A change should identify:

- the observed defect or user friction;
- the smallest responsible module;
- the invariant that must remain true;
- the test that fails before the change and passes after it;
- the expected operational or usability improvement;
- the rollback path.

Do not report dependency reductions, lower latency, lower memory use or faster startup without a before/after measurement on the same workload and build configuration. Avoid broad rewrites when a bounded change can be tested and reverted.

## 2. Curtain Dynamics: progressive disclosure without hiding evidence

The UI may reveal detail in stages:

1. **Overview:** current scan state, evidence coverage and the highest-priority finding.
2. **Inspection:** individual Windows-visible device, service, backup-marker and USB-observer results.
3. **Advanced detail:** stable finding codes, measurement limits, report schema and diagnostic context.

Every summary must link conceptually to the evidence behind it. “Unknown” must remain distinct from “failed”; missing permissions or incomplete observation windows must not be presented as negative measurements. A score must never imply certainty beyond its measured evidence coverage.

The current application is a native Win32/GDI desktop program. Do not promise Framer Motion, GPU-accelerated transitions or a cross-platform GUI unless the implementation actually changes.

## 3. Screenshot and visual-documentation standard

For every material GUI change, add a screenshot or explicitly mark the documentation as a static design preview.

Recommended capture standard:

| Field | Requirement |
|---|---|
| Capture | Screenshot from the running application for implementation claims |
| Resolution | Prefer 1920×1080 or higher; retain a readable native-scale crop |
| Context | Show enough window chrome to identify the application and page |
| Data | Synthetic or sanitized device identifiers and backup paths |
| Annotations | Optional vector overlays; never cover the source UI state |
| Theme | Match the shipped black/graphite and yellow visual system |
| Accessibility | Verify text contrast, focus visibility, keyboard operation and scaling |
| Naming | Stable descriptive names under `assets/screenshots/` or `docs/gui/` |

Current SVG previews in the README are illustrative layouts, not captured runtime evidence. Keep that distinction visible until real application captures replace them. Do not invent screenshot assets or label a mockup as a running application.

Suggested evidence set:

- `overview.png` — dashboard and Duck Readiness;
- `usb-devices.png` — device and service observations;
- `reports.png` — local report history and preview;
- `settings.png` — supported configuration;
- `error-states.png` — unavailable, incomplete and recovery states.

A GIF is optional and should be added only when an interaction cannot be understood from static captures.

## 4. Portable engine and platform boundary

The C17 core remains buildable without Win32. It owns deterministic policy, evidence assessment, bounded finding memory, architecture validation and stable identifiers. Windows APIs remain in the platform/UI boundary.

Required checks:

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Windows x64 release verification:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

A passing portable engine test does not prove the Win32 UI, Apple driver installation, pairing state or a physical cable. Hardware checks require a documented supported device, Windows version, driver state and test procedure.

## 5. Resilience matrix

The engine and application should retain distinct outcomes for at least these conditions:

| Condition | Expected interpretation |
|---|---|
| Device enumeration unavailable | Scan unavailable; no claim that the device is absent |
| No Apple PnP interface found after a completed scan | Device not visible to Windows |
| Service query denied or incomplete | Service status unknown |
| Service confirmed stopped or absent | Service unavailable |
| Backup directory cannot be enumerated | Backup scan incomplete |
| Backup markers are absent or malformed | Backup structure not verified |
| USB watch not registered or warm-up incomplete | Stability unconfirmed |
| Complete measurements meet the policy | Windows-side checks passed, not “iPhone repaired” |

For each new state, test both its finding priority and score-coverage effect. Preserve the report contract's stable machine identifiers and add fields rather than silently changing existing field meanings.

## 6. Performance and resource budgets

Measure the real application before setting optimization claims. A useful benchmark record includes:

- commit SHA, compiler and version;
- build type and target architecture;
- Windows version and hardware class;
- input size and number of connected PnP interfaces;
- warm-up procedure and repetition count;
- median and p95 elapsed time;
- peak working set and report size;
- test result and any skipped measurements.

A 5% regression threshold may be used as a review trigger after a stable baseline exists; it is not an already-enforced project guarantee. Do not add custom allocators, memory pools, asynchronous layers or caches unless a profile identifies a bottleneck and correctness tests cover the new lifetime/concurrency behavior.

## 7. Privacy and trust boundaries

- Read-only scan and inspection remain the default.
- Report creation, clipboard access and export require an explicit user action.
- Device mutation and network upload remain denied by the core policy.
- The bounded ledger stores finding codes, counts and monotonic timestamps—not raw Hardware IDs, backup contents or personal files.
- A SHA-256 sidecar detects changes to a report after creation; it does not prove that the diagnostic observations are true.
- Local paths and identifiers must be reviewed before a user shares a report.
- Unknown measurements must not be coerced into negative observations.

## 8. Merge acceptance checklist

- [ ] C17 build completes with warnings enabled.
- [ ] CTest passes on the portable core.
- [ ] Windows x64 build and GUI smoke test pass when UI/platform code changes.
- [ ] New assessment states preserve unknown-versus-failed semantics.
- [ ] Report schema compatibility is checked.
- [ ] Fail-closed permission tests cover new operations.
- [ ] Resource bounds and error paths have tests.
- [ ] Real screenshots or clearly labelled previews are supplied for GUI changes.
- [ ] README and relevant docs reflect actual behavior and known limits.
- [ ] Benchmarks are reproducible and no unsupported speed/quality claim is introduced.
- [ ] User-visible data collection, export or device actions are documented.

## 9. Deterministic workflow tick and session integrity

A workflow session is caller-visible state, so preflight approval is not a
permanent trust token. Before every host callback, `toolng_workflow_tick`
revalidates the bounded step count, cursor, policy budget, attempted/passed/
failed counters, prior evidence sequence, current allowlisted action and
local-record permission. A malformed or modified session transitions to
`SAMBU_WF_FAILED` with `SAMBU_WF_SESSION_INVALID`; no host action is called.

The scheduling contract remains one allowlisted action per tick. A failed
action stops the workflow immediately, later actions remain unattempted, and
terminal sessions do not replay callbacks. Regression tests must cover both
ordinary incremental execution and post-preflight state tampering.

## 9. UI modularization sequence

The desktop host currently concentrates dashboard drawing, page state, Win32 event routing and report composition in \`src/main.c\`. Treat the single-responsibility refactor as staged work rather than claiming it is already complete.

1. **Freeze contracts:** identify shared application state and define a small UI/platform interface without moving diagnostic policy into view code.
2. **Extract read-only presentation:** move dashboard, devices, backups, reports and care rendering into focused view modules. Views receive immutable snapshots and do not call Windows device APIs directly.
3. **Extract report composition:** centralize report schema fields and privacy filtering; maintain compatibility tests.
4. **Extract event/navigation routing:** centralize hit-testing, shortcuts and page transitions; keep platform callbacks thin.
5. **Verify each step:** portable core tests, Windows x64 build/CTest, documentation checks, accessibility review and real sanitized screenshots.

Do not introduce a UI framework, GPU animation system, asynchronous pool or custom allocator merely to satisfy a design phrase. Adopt these only when a measured need and lifecycle tests justify them.

## 10. Roadmap derived from the improvement brief

**Foundation:** validate the existing portable core, document its evidence model, standardize GUI captures and keep README claims synchronized with code.

**Acceleration:** add targeted resilience and UI regression tests, introduce repeatable performance baselines, and improve diagnostics only where test evidence identifies a gap.

**Release hardening:** verify real Windows x64 builds, device/driver scenarios, report compatibility, privacy disclosures and upgrade/rollback behavior.

The time windows in the source proposal are planning suggestions, not delivery promises. Each phase closes only when its acceptance evidence exists.

**Release rule:** a feature is complete when implementation, tests, documentation, privacy boundaries and user-visible evidence agree.
