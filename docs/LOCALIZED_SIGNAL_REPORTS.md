# Localized Diagnostic Labels and Recurrence Evidence

ToolNG may present diagnostic findings in multiple UI languages while keeping its native evidence model stable. Localization changes presentation; it must not change finding identity, readiness scoring, or authorization.

## Stable event model

- Store stable finding codes and monotonic timestamps in the bounded recurrence ledger.
- Count a new episode only when the finding transitions away and later returns; a repeated UI refresh is not a new episode.
- Keep `single`, `recurrent`, and `persistent` as presentation labels derived from the existing observation window and thresholds.
- Use translation-resource keys to render labels. Do not use translated display text as a machine key.
- Keep unknown, unavailable, not checked, failed, and passed as distinct states.

## Privacy and scope

ToolNG remains a local Windows-side Apple diagnostics utility. Do not store arbitrary user text in the recurrence ledger, upload diagnostic text, or infer device state from a language/script label. Reports should contain the minimum evidence needed to explain the selected finding and its coverage.

The [SAMBU multilingual signal contract](https://github.com/bboroski/SAMBU/blob/main/docs/MULTILINGUAL_SIGNAL_PIPELINE.md) defines separate representation, language-hint, translation, and semantic-review states. ToolNG can reuse the principle of explicit statuses without embedding the SAMBU Python runtime or turning the device diagnostic workflow into a general text analyzer.

A recurrence count is an observation pattern, not a probability of hardware failure and not permission to mutate the device.