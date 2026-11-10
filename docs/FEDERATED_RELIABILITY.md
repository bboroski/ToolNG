# Federated reliability model

ToolNG is a local Windows diagnostic utility. Its trust boundary is observation and evidence collection, not automatic device repair.

## Ownership and admission

- Device discovery, service observation, backup-marker inspection and workflow interpretation remain separately testable operations.
- `toolng_workflow_compile` accepts only allowlisted actions; arbitrary shell execution is not an action.
- `toolng_workflow_preflight` enforces the selected policy and resource limit before a run begins.
- `toolng_workflow_tick` advances one action at a time, making progress and failures observable.

## Evidence and safety

- Each completed step retains its source span, action, outcome and finding in a bounded run record.
- Unknown actions, duplicate step names, malformed manifests, policy denial and tampered sessions fail closed.
- A missing signal is not a negative finding; an incomplete evidence set must remain visibly incomplete.
- Recommendations are not executed repairs. Local report handling and privacy filtering remain explicit operator-controlled steps.

## Verification gate

Build with CMake and run CTest on a supported host. The workflow test now includes the actual `toolng_workflow.h` interface; it previously included a nonexistent header name. Windows device behavior still requires Windows-host validation.
