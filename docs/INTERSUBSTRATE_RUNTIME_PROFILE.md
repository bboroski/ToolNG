# Inter-substrate runtime profile — ToolNG

**Contract:** [SAMBU runtime API v1](https://github.com/bboroski/SAMBU/blob/feat/substrate-runtime-contract-v1/docs/INTERSUBSTRATE_RUNTIME_API_V1.md)  
**Status:** repository-specific integration profile; not a blanket implementation claim.

## Ownership
Windows-local diagnostic application (C17/Win32).

## Existing evidence
Read-only Windows PnP/USB and service observation, local reports/logs, backup-marker inspection, evidence-quality scoring, and redaction.

## Integration slice
Use the v1 envelope for local diagnostic events and redacted report export. Resource descriptors represent observed device references and evidence artifacts, never handles or private backup contents. Correlation IDs connect a scan, device-change event, and report. Add bounded event retention, explicit unknown/not-checked/failed states, and counters for scan duration, queue rejection, and evidence coverage.

## Explicit non-goals
No device mutation, driver installation, cloud upload, E2E encryption, service mesh, token authority, or remote orchestration is introduced by this profile. Windows process isolation and access control remain OS/adapter responsibilities.

## Acceptance tests
Test redaction before serialization; malformed/oversized packet rejection; stable event IDs and sequence; bounded history; device unplug during scan; no network side effects; evidence-state distinctions.

## Capability maturity
- Existing features remain at the maturity stated by this repository's source, tests, and target evidence.
- The v1 event envelope, resource descriptor, canonical JSON codec, and bounded in-process reference queue are implemented in the SAMBU host reference branch only.
- Any C ABI, firmware transport, OS service, or remote adapter in this repository requires its own implementation, tests, and evidence before being marked implemented.
