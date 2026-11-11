# NUN OS integration contract — ToolNG

## Ownership
ToolNG owns developer diagnostics and evidence tooling: log collection, static-analysis adapters, profiling reports and debug-session orchestration. It is not a kernel subsystem and must not become a runtime boot dependency.

## Integration direction
1. Consume serial logs, QEMU metadata, compiler diagnostics, map files and debug symbols from a pinned NUN build.
2. Normalize kernel panic, exception vector, fault address and interrupt timing into a stable report schema.
3. Add optional GDB remote-stub support only after NUN publishes a stable debug transport contract.
4. Keep Windows diagnostics usable without a running NUN target; isolate host-specific code from portable parsers.
5. Preserve source hashes and tool versions in every evidence bundle.

## Acceptance gates
- Log parsers have fixtures for clean boot, memory-test failure, exception and timer interrupt output.
- Missing or malformed evidence is reported as inconclusive.
- Profiling reports identify sampling overhead and do not invent kernel performance numbers.
- Static-analysis findings are actionable and retain file/line references.

## Current status
ToolNG remains an optional diagnostic utility. A successful ToolNG report cannot substitute for a QEMU boot workflow or hardware qualification.
