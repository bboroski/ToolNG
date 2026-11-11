# Diagnostic guidance

ToolNG now maps each deterministic finding to an operator-facing severity, summary key and next-action key. The mapping is local and read-only: it does not start services, modify devices, create backups or export reports. Unknown findings fail to a neutral review instruction. A healthy finding explicitly requires no operator action; unknown evidence remains a prompt to collect evidence, never a negative diagnosis.

The API is `toolng_guidance_for_finding()` in `src/toolng_guidance.h`. Keys are stable localization inputs rather than user-facing prose.
