# NUN integration — host-side diagnostic evidence

## Assigned responsibility

ToolNG is a read-only Windows device-evidence probe. Its only safe NUN-facing role is optional host-side collection of local device or environment evidence for developers and test operators. It is not a NUN kernel driver, GDB stub, performance profiler or privileged system service.

## Current verified state

- The application inspects available Windows PnP/USB devices and related service metadata.
- Local evidence reports distinguish confirmed signals from unknown/incomplete coverage.
- Device mutation, driver changes, backup-content access and cloud upload are outside its stated scope.

## Integration contract

1. Keep all probes read-only and local by default.
2. Redact sensitive identifiers before evidence is exported or attached to an issue.
3. Label Windows host observations separately from QEMU guest observations and physical NUN hardware qualification.
4. Do not imply that a host USB inventory validates a NUN xHCI driver.
5. Do not add privileged kernel hooks or automatic repair actions as part of this OS integration.
6. Any future NUN debugger/profiler must be a separately reviewed project/interface, not a rename of the current device-evidence probe.

## Dependency order

ToolNG is not in the Phase A kernel boot path. It may be used by developers to annotate host test environments, while the QEMU serial log and CI run remain authoritative for the initial [NUN](https://github.com/bboroski/NUN) boot gate.

## ABI status

No ToolNG public API or NUN kernel ABI is changed here. Record any future NUN-facing contract change in `CHANGELOG.NUN`.
