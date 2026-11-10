# Bounded Diagnostic Provenance Trace

The portable SAMBU core now provides a fixed-capacity event ring for recording
how a diagnostic result moved through the tool's processing layers.

## Contract

The API is declared in src/toolng_engine.h and implemented in
src/toolng_engine.c:

- stages: source, policy, evidence, assessment, relaxation and report;
- outcomes: observed, accepted, rejected and degraded;
- each event stores a monotonically increasing sequence number, caller-supplied
  monotonic timestamp, event code, evidence mask and quality flags;
- storage is fixed at 64 entries; when full, the oldest event is overwritten
  and a saturating dropped-event counter is incremented;
- snapshots are copied oldest-first and never allocate memory;
- unknown stages/outcomes, corrupt ring bounds and sequence overflow are
  rejected without committing an event.

The trace is intentionally metadata-only. It does not store device serials,
raw USB descriptors, file paths, backup contents, user identifiers or other
payload data. Timestamp values are supplied by the caller; the core does not
invent a clock or silently reorder events after a clock reset.

## Example

~~~c
SambuTrace trace;
SambuTraceEvent events[SAMBU_TRACE_CAPACITY];

sambu_trace_init(&trace);
sambu_trace_record(&trace, SAMBU_TRACE_EVIDENCE, SAMBU_TRACE_OBSERVED,
                   1001u, evidence_mask, quality_flags, monotonic_ms);
size_t count = sambu_trace_snapshot(&trace, events, SAMBU_TRACE_CAPACITY);
~~~

## Engineering rationale

The trace separates the raw observation environment from the stages that
interpret it. This makes it possible to answer which layer accepted, rejected
or degraded a finding without conflating an unknown measurement with a
verified negative result. The fixed ring makes memory use predictable for a
native desktop diagnostic process.

The trace is not an immutable audit log: it is a bounded in-memory diagnostic
window. Export and persistent retention remain separate, explicit user actions
and must follow the existing privacy policy.
