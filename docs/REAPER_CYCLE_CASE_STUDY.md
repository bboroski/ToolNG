# Cyclic Fault and Signal-Risk Case Study — ToolNG

This document translates generic recurring-fault and signal-integrity patterns into diagnostic workflows. It does not add fictional entities, external media terminology or non-existent device behavior to ToolNG. ToolNG remains a Windows-side Apple diagnostics utility.

## 1. Engineering mapping

| Abstract pattern | Technical interpretation | ToolNG/SAMBU application |
|---|---|---|
| Repeated cycles | Recurrence within a bounded observation window | SAMBU counts diagnostic finding transitions in a 10-minute window. |
| Early-warning event | Observe a precursor before a larger failure sequence | USB Pulse observes arrivals/removals before a full diagnostic pass. |
| Central dependency hub | Critical infrastructure graph | `sambu_architecture_validate()` checks dependency order across core, policy, evidence, platform, memory, presentation and export layers. |
| Progressive signal corruption | Escalation from isolated signal to persistent pattern | The ledger labels observations `single`, `recurrent` or `persistent`; these are operational labels, not inferred causes. |
| Heterogeneous worker roles | Typed modules with distinct responsibilities | Evidence, policy, memory, presentation and export remain separate modules with bounded contracts. |

## 2. Recurrence model

SAMBU stores up to 32 distinct finding codes. Each entry retains lifetime observation count, first and last monotonic timestamps, the active 10-minute window start and the number of state transitions in that window.

A finding is recorded only when the selected diagnostic state changes. A timer repaint or refresh of an unchanged state is not a new episode.

| Episodes in active window | Pattern | Interpretation |
|---:|---|---|
| 0 | `none` | No observation in the current window |
| 1 | `single` | One observed episode |
| 2–3 | `recurrent` | The same finding returned |
| 4+ | `persistent` | Repeated returns warrant root-cause investigation |

The 10-minute window is an operational parameter already used by ToolNG USB Pulse, not a conversion from an external source.

## 3. Example event sequence

1. `apple_device_not_visible` — one episode is recorded.
2. `windows_side_checks_passed` — the finding changes; the healthy state is also recorded.
3. `usb_connection_unstable` — one instability episode is recorded.
4. `windows_side_checks_passed` — the device returns to a state that passes current checks.
5. `usb_connection_unstable` — the finding returns; recurrence count becomes two.
6. Further returns within the active window move the pattern to `persistent` at four episodes.

DuckFix adds a short note when an unhealthy finding repeats. The private TXT report records the current 10-minute count and pattern key for later review.

## 4. Safety and interpretation

- Recurrence is not a probability of hardware failure and does not identify a cable, port, driver or device as the root cause.
- The ledger stores finding codes and timestamps only; it does not retain Hardware IDs, backup paths or personal device content.
- A recurrence note recommends investigation; it does not repair, mutate device state or upload data.
- A missing or incomplete scan remains unknown, not proof that a device is absent.
- ToolNG does not infer compromise, intent or psychological state from diagnostic signals.

## 5. Verification

Portable SAMBU tests cover first observation, repeated observations, the persistent threshold, expiry after the 10-minute window and monotonic-clock reset. Windows CI runs these tests through CTest after building the native application.
