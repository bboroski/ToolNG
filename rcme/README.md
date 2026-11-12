# RCME native module

Repository-specific native C11 adapter for the Recursive Cross-Domain Matrix Engine.

## Scope
- Caller-owned matrix views with explicit element capacity; no heap allocation, global mutable state, or implicit I/O.
- Checked matrix multiplication and transpose, including output/input overlap rejection.
- One bounded composition: normalize(M × A × transpose(M) + B).
- Frobenius normalization, residual measurement, finite-value, dimension and capacity checks.
- Repository-specific diagonal weighting for read-only device diagnostics, evidence quality and telemetry health.

## Build and test
\`\`\`sh
make -C rcme test
\`\`\`

Each matrix view declares rows, columns, stride, accessible element capacity and a caller-owned data pointer. Capacity is measured in double elements from the pointer. Output and scratch views must not overlap input or each other. Destination buffers are not read before writing; numeric overflow may leave partial output, as documented in the C API.

This is an isolated additive slice. It does not modify existing application APIs, infer physical laws, execute hardware operations, free caller memory, or claim quantum speedup. It is a bounded numeric reference engine, not a physical simulator or hardware controller.
