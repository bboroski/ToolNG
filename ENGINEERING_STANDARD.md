# Engineering Standard

**Standard ID:** PERSONA-ENG-STD  
**Version:** 1.0.0  
**Applies to:** this repository and its independently released components  
**Status:** normative baseline; repository-specific profile is authoritative for local scope

## 1. Purpose and precedence
This standard defines minimum engineering controls across the bboroski repository ecosystem. It does not define a shared application architecture, runtime, language, build system, release cadence, or product roadmap. The `.engineering/profile.json` file is the repository-specific overlay. Narrower local safety boundaries, existing contracts, platform constraints, licenses, and documented build/test procedures take precedence.

## 2. Architecture preservation
1. Preserve this repository's product identity, domain, language choices, module boundaries, public APIs, data formats, and native build system.
2. Do not introduce a common runtime, framework, dependency, folder layout, or cross-repository import merely to make repositories look alike.
3. Integrate repositories only through explicit, documented, versioned contracts and adapters. No implicit shared-memory, filesystem, process, or hardware access is assumed.
4. Prefer additive, backward-compatible changes. Breaking changes require rationale, migration guidance, compatibility impact, and review.
5. Keep experimental, partial, target-dependent, and production-qualified capabilities distinct.

## 3. Change control
1. Implement focused changes on a topic branch and propose them through a Pull Request.
2. Keep each PR independently reviewable; split unrelated work.
3. Describe intent, affected components/contracts, risk, compatibility, tests, documentation, and rollback.
4. Inspect the full diff. Never accept conflict sides wholesale without understanding file semantics.
5. Do not bypass failing checks, required review, repository policies, or protected-branch controls.
6. Never claim merge, release, or deployment before the corresponding system confirms it.

## 4. Verification and evidence
1. Run the repository's native build, tests, lint, static-analysis, and documentation checks relevant to the change.
2. Add positive, negative, boundary, and regression tests for behavior changes.
3. Test the current PR head.
4. Treat host tests, emulators, simulations, and static analysis only as the evidence they provide; they do not imply hardware qualification.
5. Record exact commands and outcomes. Mark unavailable checks as not run and disclose remaining risk.
6. Do not add fake tests, synthetic success output, invented benchmarks, or unsupported claims.

## 5. Security, privacy, and supply chain
1. Apply least privilege to workflows, tokens, credentials, and application capabilities.
2. Do not commit secrets, private data, or machine-specific paths.
3. Review dependency additions, versions, licenses, provenance, and vulnerabilities.
4. New workflows default to read-only permissions; document any required write access.
5. Treat external inputs, downloaded artifacts, manifests, and inter-repository data as untrusted until validated.
6. Destructive operations require explicit target identification, confirmation, bounded scope, and recovery plan.
7. Do not infer consent to upload or transmit local diagnostic data.

## 6. Documentation and decisions
1. Update docs when behavior, interfaces, commands, configuration, support claims, or operations change.
2. Keep examples executable or label them illustrative.
3. Record significant architecture decisions in the repository's established ADR/design location.
4. Keep translated README variants scope-equivalent where supported.
5. State limitations and non-goals alongside capability claims.

## 7. Release and operations
A merged change is not automatically a release. Release readiness requires applicable native checks, reviewed dependencies, compatibility disclosure, known-issue disclosure, and appropriate rollback/recovery. Maintain independent versions and release decisions for independent products.

## 8. Required evidence gates
Every material change addresses profile validation, native project checks, architecture/compatibility, security/privacy, documentation, human review, and target qualification when applicable. “Not applicable” requires a short reason.

## 9. Status vocabulary
- **PROPOSED** — suggested, not implemented.
- **PLANNED** — accepted for future work, not implemented.
- **IMPLEMENTED** — code/configuration exists; verification may be pending.
- **TESTED** — specified tests were run and passed; scope is stated.
- **VERIFIED** — stated acceptance criteria have evidence.
- **MERGED** — present in the target branch.
- **RELEASED** — a versioned release was published.
- **BLOCKED** — progress is stopped by a stated dependency, failure, or decision.

These statuses are not interchangeable. A design, open PR, metadata check, or host test is not a production release or hardware qualification.

## 10. Ternary decision model
- **-1 / reject:** regression, unsafe boundary, failed invariant, or insufficient evidence.
- **0 / hold:** unknown or incomplete; gather evidence.
- **+1 / accept:** acceptance criteria pass and required review is complete.
A score never overrides a failed mandatory gate or approval requirement.

## 11. Definition of Done
A change is complete only when implemented, relevant native checks are run and recorded, security and compatibility are considered, documentation is accurate, the complete diff is reviewed, and required review/merge/release steps are confirmed. Missing target evidence remains an explicit limitation.

## 12. Enforcement boundary
The `Engineering Standard / profile-validation` workflow validates the machine-readable profile. Existing workflows remain responsible for native builds and tests. Administrators must set profile-validation and applicable native checks as required status checks in branch protection/rulesets before GitHub can block merges on them. This change does not rewrite branch protection or bypass existing rules.
