# Reaper Cycle Case Study — SAMBU diagnostic patterns

This document uses the supplied analysis of the Reapers from *Mass Effect* as an engineering-design exercise. It does **not** add fictional entities, terminology, game assets, or gameplay behavior to the ToolzNG interface. ToolzNG remains a Windows-side Apple diagnostics utility.

## 1. What the source contributes

The source describes a threat model built around repeated galactic cycles, an early-warning Vanguard, infrastructure dependencies, progressive indoctrination, and distinct classes of deployed units. The useful software concepts are:

| Source concept | General engineering pattern | ToolzNG/SAMBU application |
|---|---|---|
| Repeated 50,000-year harvest cycles | Recurrence over a bounded observation window | SAMBU counts repeated diagnostic finding transitions in a 10-minute window. The fictional 50,000-year interval is not copied as a product setting. |
| Sovereign as a hidden Vanguard | Early warning before a larger failure sequence | USB Pulse observes arrivals/removals before the user runs another full diagnostic pass. |
| Citadel and Mass Relay dependency | Critical infrastructure graph | `sambu_architecture_validate()` checks that core, policy, evidence, platform, memory, presentation, and export layers have a valid dependency order. |
| Multi-stage indoctrination | Escalation from isolated signal to persistent pattern | The ledger labels observations `single`, `recurrent`, or `persistent`; these are operational labels, not claims about hidden causes. |
| Different Reaper/ground-unit classes | Typed roles with distinct responsibilities | Evidence, policy, memory, presentation, and export remain separate modules with bounded contracts. |

The practical result is not a Mass Effect feature. It is a more careful way to reason about repeated signals, early warning, and dependency failure.

## 2. Recurrence model

SAMBU stores up to 32 distinct finding codes. Each entry retains:

- lifetime observation count;
- first and last monotonic timestamps;
- the start of the active 10-minute recurrence window;
- the number of finding transitions in that window.

A finding is recorded only when the selected diagnostic state changes. A timer repaint or a repeated refresh of the same unchanged state is not a new episode. This avoids the false-positive behavior of counting UI updates as independent failures.

| Episodes in the active window | Pattern | Interpretation |
|---:|---|---|
| 0 | `none` | No current observation in the window |
| 1 | `single` | One observed episode |
| 2–3 | `recurrent` | The same finding returned |
| 4+ | `persistent` | Repeated returns warrant checking the underlying cause |

The window is 10 minutes because ToolzNG already uses a 10-minute USB Pulse observation window. It is an operational parameter, not a lore-derived time conversion.

## 3. Example event sequence

A diagnostic session can produce this sequence:

1. `apple_device_not_visible` — one episode is recorded.
2. `windows_side_checks_passed` — the current finding changes; the healthy state is also recorded.
3. `usb_connection_unstable` — one USB-instability episode is recorded.
4. `windows_side_checks_passed` — the device returns to a state that passes the current checks.
5. `usb_connection_unstable` — the same finding returns; the recurrence count becomes two and the pattern becomes `recurrent`.
6. Further returns within the active window move the pattern to `persistent` at four episodes.

DuckFix adds a short note when an unhealthy finding has repeated. The private TXT report records the current 10-minute count and pattern key so the result can be reviewed later.

## 4. Safety and interpretation

- Recurrence is not a probability of hardware failure and does not identify a cable, port, driver, or device as the root cause.
- The ledger stores finding codes and timestamps only; it does not retain Hardware IDs, backup paths, personal device content, or the supplied lore text.
- The recurrence note recommends checking the cause; it does not perform repairs, mutate device state, or upload data.
- A missing or incomplete scan remains an unknown result, not proof that the device is absent.
- The Reapers' fictional indoctrination process is only a source of inspiration for staged modelling. ToolzNG does not infer compromise, intent, or psychological state.

## 5. Verification

The portable SAMBU tests cover first observation, repeated observations, the persistent threshold, expiry after the 10-minute window, and monotonic-clock reset. The Windows CI workflow runs these tests through CTest after building the native application.
