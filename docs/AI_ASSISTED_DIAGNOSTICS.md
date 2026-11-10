# AI-Assisted Diagnostic Summaries

**Scope:** local Windows device evidence and operator-facing explanations.  
**Status:** design boundary; ToolNG does not require or call a cloud AI service.

## Allowed role

Gemini or another model may help draft explanations for an already-produced, privacy-filtered ToolNG report. It may group findings, explain terminology and suggest questions for an operator to investigate. It must not scan a device, infer an unobserved hardware state, repair a connection or alter the report's evidence.

## Evidence contract

Summaries must preserve the ToolNG result categories:

- **observed / passed:** a check completed and its stated condition was observed;
- **not found:** a completed check did not find the expected item;
- **unknown:** the check could not establish the state;
- **failed:** the check itself encountered an error.

Never rewrite unknown as failed, healthy, missing or passed. Keep evidence coverage separate from a readiness score. A high score over incomplete coverage remains provisional.

Every sentence that states a device condition must link to an existing report finding or evidence field. Model-generated recommendations must be phrased as suggestions, not as completed actions.

## Privacy boundary

ToolNG reports remain local. Do not send raw hardware IDs, serial numbers, exact backup paths, user names, credentials, private backup contents or unredacted diagnostic bundles to an external model. If an operator deliberately uses an external service, provide only a reviewed, minimized and redacted summary.

## Non-negotiable restrictions

- No model-supplied command execution, PowerShell, registry writes or device mutations.
- No firmware update, restore, reset, cache deletion or service restart initiated by a generated summary.
- No claim that backup markers prove backup completeness, integrity or restore-ability.
- No claim that a local digest is a cryptographic identity or signature.
- No automatic upload of local diagnostic reports.

## Verification cases

A future summary adapter must be tested with: absent device; unavailable scan; unknown service state; incomplete backup scan; contradictory evidence; a healthy fully-covered sample; malicious instructions embedded in report text; and redacted identifiers. The structured report remains authoritative when model text disagrees with it.

See [Safety and technical scope](SAFETY_AND_SCOPE.md) for implemented behavior and exclusions.
