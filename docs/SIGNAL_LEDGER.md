# ToolNG bounded event history

A fixed-capacity, allocation-free diagnostic history preserves sequence, monotonic time, source/event codes, severity, evidence flags and a reference to an earlier record. Overflow drops the oldest item and increments a saturating dropped counter. Reads are chronological snapshots and do not mutate runtime state.

The ledger stores metadata only: it does not capture private payloads, query clocks, write devices, or claim causation. Confidence is a review annotation, not probability of truth. Corroboration requires an observed signal.

CI compiles the contract test with strict warnings.
