# ToolNG evidence pipeline and safe automation

ToolNG already separates policy, evidence, platform adapters, memory, presentation and export. This document fixes the cross-layer order and the markers that must survive each transition.

## Pipeline stages

1. **Discover:** query supported Windows PnP/USB and service surfaces.
2. **Collect:** record whether each query completed and which observations were returned.
3. **Reconcile:** compare observed values, backup structure and marker counts; contradictory counts become quality flags.
4. **Wait and recheck:** only report stability after a registered watch and its configured observation window have elapsed.
5. **Assess:** compute readiness and measured coverage separately. Missing evidence stays unknown; an unavailable scan is not a clean scan.
6. **Report:** produce a local report with redaction and its checksum sidecar.
7. **Operator decision:** recommendations are not commands; export or clipboard actions remain user-authorized.

## Automatic behavior boundary

Permitted automation is bounded observation, retry, debounce, evidence reconciliation and report preparation. Device mutation, firmware changes, backup repair, data upload and system reset remain unavailable through the policy core. A GUI animation may show progress or a verified state transition, but it must never imply that a device operation happened when only a recommendation was generated.

## Required markers

Each report should preserve the evidence mask, quality flags, coverage percentage, provisional-state flag, finding code, timestamp source and report-schema version. The checksum detects later accidental changes; it is not a digital signature.

## Verification gates

Host tests validate policy invariants. Windows PnP, service query, USB hotplug and backup marker scenarios still require their dedicated Windows integration tests.
