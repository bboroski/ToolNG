# Systems Engineering Pattern Catalog

**Scope:** ToolNG C17 core, Win32 adapters, desktop UI, diagnostic workflows, reports and release pipeline  
**Document type:** engineering policy and implementation map  
**Status:** design requirements; implementation claims remain governed by source code and tests

## 1. Purpose

This catalog translates high-level system-composition ideas into concrete software engineering patterns for ToolNG. It does not introduce a dependency on SAMBU's Python runtime and does not weaken ToolNG's local-first, read-only diagnostic boundary.

The target is a diagnostic application whose components can evolve independently while preserving reliable evidence, predictable resource use, safe state transitions and clear user control.

## 2. Core engineering laws

| ID | Law | ToolNG interpretation | Acceptance evidence |
|---|---|---|---|
| TNG-01 | Layered composition | Separate portable C17 policy/core, Windows adapters, workflow dispatch, UI composition and reporting | Portable core builds without Win32 headers or implicit platform calls |
| TNG-02 | Deterministic processing | Normalize observations before scoring, finding selection and report serialization | Identical evidence snapshots produce identical assessment keys and report fields |
| TNG-03 | Explicit lifecycle | Centralize page enter/leave, watcher registration, timer ownership and session teardown | Every successful acquisition has one release path; duplicate registration is prevented |
| TNG-04 | Bounded event flow | Bound USB event history, workflow steps, report history and diagnostic payloads | Capacity, overflow policy and dropped-event accounting are tested |
| TNG-05 | Evidence provenance | Preserve source, completion state, timestamp domain and limitations of each observation | A finding is traceable to the evidence fields that caused it |
| TNG-06 | Unknown-safe behavior | Keep Observed, Not found, Unknown and Failed semantically distinct | Missing access or incomplete scans never become negative or healthy findings |
| TNG-07 | Fail-closed authorization | Require an allowed operation, origin and policy before every local side effect | Denied operations are rejected before mutation; no implicit network/device powers |
| TNG-08 | Cooperative execution | Run no more than the declared bounded amount of workflow work per dispatch | UI remains responsive; cancellation and failure are observable |
| TNG-09 | Reversible evolution | Introduce modules and new adapters behind versioned contracts | Existing report IDs, workflow behavior and CLI/build contracts remain compatible |
| TNG-10 | Test the test system | Use mutation, boundary, state-transition and fault-injection tests | The suite detects deliberately injected boundary and recovery defects |
| TNG-11 | Local data minimization | Store only required diagnostic signals and bounded metadata | No raw backup payload, unnecessary identifiers or network transmission |
| TNG-12 | Honest presentation | Keep UI labels, readiness score and docs aligned with actual evidence | No result appears complete when evidence coverage is partial |

## 3. Diagnostic data path

The diagnostic pipeline is a sequence of independently testable stages:

1. **Discovery** — collect Windows-visible PnP, service, backup-marker and watcher observations.
2. **Normalization** — convert platform-specific responses into portable evidence fields.
3. **Validation** — check completeness, counter relationships, allowed value ranges and source availability.
4. **Assessment** — calculate readiness, evidence coverage, quality flags and prioritized finding.
5. **Presentation model** — convert assessment into stable UI/report values without changing its meaning.
6. **User-directed output** — display locally; copy or export only after explicit user action.

Each signal must retain its epistemic state:

- **Observed** — the check completed and confirmed the signal.
- **Not found** — the check completed and did not find the signal.
- **Unknown** — the check could not establish the state.
- **Failed** — the operation itself failed and has a diagnostic cause.

Do not collapse these states into one Boolean. A missing service query is not proof that a service is stopped. A partial backup scan is not proof that a backup is absent. A watcher that is not active cannot establish connection stability.

## 4. Page lifecycle and transition gates

A page transition is a controlled state-machine operation, not an arbitrary assignment to a global page index.

Recommended transition sequence:

**validate target → run current-page leave guard → release page-owned resources → commit route change → initialize target page → run target-page enter guard → render.**

The controller should own:

- current page and allowed transitions;
- page-specific timer subscriptions;
- device-notification registrations;
- scroll and selection state;
- focus restoration and keyboard navigation;
- cancellation of work that is no longer relevant to the visible page.

Rules:

1. Reject invalid target pages before changing application state.
2. Do not register the same watcher or timer twice.
3. Release page-scoped resources on every exit path, including errors and window shutdown.
4. If target-page initialization fails, show an explicit unavailable state; do not fabricate readiness.
5. Keep user-visible actions separate from automatic lifecycle hooks.
6. Never let a view directly bypass diagnostic-core policy.
7. Preserve only state whose lifetime is explicitly defined.

Use small C modules and narrow interfaces for the navigation controller, view composition and report presentation. Keep Win32 handles and callbacks out of portable policy structures.

## 5. Bounded event processing and resource budgets

Event-driven monitoring must define capacity and overload behavior. The existing bounded USB event history and workflow limits are useful foundations; extensions must preserve explicit caps.

For every event queue or journal, specify:

- capacity in entries and bytes;
- monotonic clock source and timestamp meaning;
- ordering and deduplication policy;
- overflow behavior (drop-oldest, reject-newest or aggregation);
- dropped/merged event counters;
- reset and shutdown semantics.

For every workflow, define maximum steps, payload sizes, retry count, deadline and allowed host actions. A one-step cooperative dispatcher is not a general-purpose asynchronous runtime. Do not add a busy loop to the UI thread; time-consuming platform discovery should have a clear cancellation and completion contract before it is moved to a worker.

Resource limits must be visible in tests. An application that is usually fast but has unbounded growth is not considered reliable.

## 6. Modular architecture target

Preserve the portable-core boundary already represented by `ToolNG.PolicyCore`. The Windows shell can evolve independently through these modules:

| Module boundary | Responsibility | Must not own |
|---|---|---|
| `toolng_engine` | Authorization, evidence validation, assessment, stable IDs, bounded ledgers | HWND, GDI, SetupAPI calls |
| `toolng_workflow` | Workflow lexer/parser, allowlist, preflight, bounded one-step dispatch | Arbitrary shell or dynamic plugin execution |
| `platform_apple` | SetupAPI, SCM, device notifications and OS observations | Readiness policy or report copy |
| `ui_navigation` | Page state, enter/leave guards, focus and navigation | Device discovery and evidence mutation |
| `ui_views` | Rendering and interaction layout | Policy decisions or persistent state |
| `report_model` | Stable report composition and privacy projection | Discovery and hidden export side effects |
| `local_store` | Bounded local journal, settings and report index | Network transport or backup payload parsing |

The current desktop host still contains substantial UI and platform orchestration in `src/main.c`. This table is a decomposition target, not a claim that all modules already exist. Extract one boundary at a time, preserving observable behavior and running the portable CTest suite after every step.

## 7. Workflow contract

The workflow language remains a closed, allowlisted instruction format.

- Parse into a bounded AST with source locations.
- Validate names, manifest, action allowlist and autonomy policy before dispatch.
- Keep authorization checks in the portable core and host boundary.
- Execute at most one declared action per tick.
- Record ordered step evidence and the first failure.
- Never accept arbitrary command strings, dynamic native library paths or unrestricted callbacks from workflow source.
- A successful preflight means only that the plan passed policy checks; it does not mean the device action succeeded.

New actions require a documented purpose, capability, risk assessment, source diagnostic, positive test, denial test and resource limit.

## 8. Evidence-based adaptive improvement

The improvement loop is:

**baseline → bounded observation → hypothesis → isolated change → tests → benchmark → review → release decision.**

It may adapt thresholds or recommend priorities only when the evidence and policy authorize that change. It must not silently rewrite readiness weights, hide findings, change access permissions, delete local evidence or upload information.

To improve the verification process itself:

- boundary tests cover zero, one, maximum and over-capacity inputs;
- state-machine tests cover every valid and invalid page transition;
- fault injection covers API failures, inaccessible paths and partial enumeration;
- property-based tests check evidence invariants and score bounds;
- mutation tests verify that the suite catches inverted predicates and skipped gates;
- snapshot tests protect stable report identifiers and schema behavior;
- benchmark fixtures compare the same synthetic evidence and event workloads.

A random sample or one successful run is not proof of correctness. Real-device smoke tests remain distinct from portable unit tests and must identify the tested Windows/device configuration.

## 9. Security and privacy invariants

- No passcode, pairing or Activation Lock bypass.
- No jailbreak, firmware modification or silent device mutation.
- No arbitrary command execution from workflow input.
- No backup payload inspection for structural checks.
- No network upload or telemetry by default.
- Hardware IDs and full local paths remain private in exported views unless a future explicitly consented feature defines a separate policy.
- Hashes detect some forms of post-write change; they do not prove source observations were truthful.
- An authorization denial must occur before a side effect, not after it.

## 10. Prioritized implementation roadmap

**P0 — lifecycle and evidence correctness**
- Add a central page-transition controller with enter/leave guards.
- Add table-driven tests for every page transition and resource-release path.
- Add explicit event-overflow counters and test the selected ring-buffer policy.
- Preserve stable diagnostic codes and report schema.

**P1 — host decomposition**
- Extract navigation and view composition from `src/main.c`.
- Extract report-model construction and privacy projection.
- Isolate SetupAPI, Service Control Manager and device-notification adapters behind narrow interfaces.
- Keep the portable core independently buildable on non-Windows CI.

**P2 — stronger diagnostics**
- Add structured trace/event identifiers connecting observation, validation, finding and report.
- Add fault-injection tests for every OS adapter failure class.
- Add property-based and mutation tests for scoring, completeness and permission logic.

**P3 — optional integrations and release engineering**
- Keep libimobiledevice pairing integration optional and provider-gated.
- Add signed, reproducible release artifacts and installer verification.
- Add hardware smoke-test records with explicit device/OS coverage.
- Add accessibility and UI Automation checks as UI modules are extracted.

## 11. Change acceptance checklist

- [ ] The change maps to a named law above and a concrete user problem.
- [ ] Platform APIs remain outside the portable core.
- [ ] Every queue, loop, payload and retry has a finite limit.
- [ ] All state transitions have explicit success and failure behavior.
- [ ] Authorization is checked before each side effect.
- [ ] Unknown and incomplete evidence remain unknown and incomplete.
- [ ] A positive test and at least one relevant denial/failure test exist.
- [ ] Report IDs, privacy projection and workflow compatibility are preserved.
- [ ] Documentation distinguishes current implementation from roadmap.
- [ ] Windows hardware claims are supported by an actual hardware test record.
