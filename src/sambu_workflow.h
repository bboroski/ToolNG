#ifndef TOOLZNG_WDEP_WORKFLOW_H
#define TOOLZNG_WDEP_WORKFLOW_H

#include "wdep_engine.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define WDEP_WORKFLOW_VERSION "1.1.0"
#define WDEP_WORKFLOW_SOURCE_CAP 3072u
#define WDEP_WORKFLOW_STEP_CAP 8u
#define WDEP_WORKFLOW_NAME_CAP 32u
#define WDEP_WORKFLOW_TEXT_CAP 128u

typedef enum WdepWorkflowAction {
    WDEP_WF_NONE = 0,
    WDEP_WF_SCAN_USB,
    WDEP_WF_READ_SERVICE,
    WDEP_WF_RECORD_PROFILE,
    WDEP_WF_AUDIT_BACKUP,
    WDEP_WF_ASSESS_READINESS
} WdepWorkflowAction;

typedef enum WdepWorkflowAutonomy {
    WDEP_WF_OBSERVE_ONLY = 0,
    WDEP_WF_LOCAL_RECORD = 1
} WdepWorkflowAutonomy;

typedef enum WdepWorkflowState {
    WDEP_WF_IDLE = 0,
    WDEP_WF_RUNNING,
    WDEP_WF_COMPLETED,
    WDEP_WF_FAILED
} WdepWorkflowState;

typedef enum WdepWorkflowError {
    WDEP_WF_OK = 0,
    WDEP_WF_ARGUMENT,
    WDEP_WF_SOURCE_LIMIT,
    WDEP_WF_LEXICAL,
    WDEP_WF_SYNTAX,
    WDEP_WF_UNKNOWN_ACTION,
    WDEP_WF_DUPLICATE_STEP,
    WDEP_WF_BUDGET,
    WDEP_WF_POLICY,
    WDEP_WF_HOST_FAILURE,
    WDEP_WF_SESSION_INVALID
} WdepWorkflowError;

typedef struct WdepSourceSpan {
    size_t offset;
    size_t length;
    uint32_t line;
    uint32_t column;
} WdepSourceSpan;

typedef struct WdepWorkflowStep {
    char name[WDEP_WORKFLOW_NAME_CAP];
    WdepWorkflowAction action;
    WdepSourceSpan source;
} WdepWorkflowStep;

typedef struct WdepWorkflow {
    char name[WDEP_WORKFLOW_NAME_CAP];
    char purpose[WDEP_WORKFLOW_TEXT_CAP];
    uint16_t max_steps;
    uint16_t step_count;
    WdepWorkflowAutonomy autonomy;
    WdepWorkflowStep steps[WDEP_WORKFLOW_STEP_CAP];
} WdepWorkflow;

typedef struct WdepWorkflowDiagnostic {
    WdepWorkflowError code;
    WdepSourceSpan source;
    char message[WDEP_WORKFLOW_TEXT_CAP];
} WdepWorkflowDiagnostic;

typedef struct WdepWorkflowPolicy {
    uint16_t max_steps;
    bool allow_observation;
    bool allow_local_record;
} WdepWorkflowPolicy;

typedef struct WdepWorkflowEvidence {
    uint16_t sequence;
    WdepWorkflowAction action;
    bool succeeded;
    WdepSourceSpan source;
    char step_name[WDEP_WORKFLOW_NAME_CAP];
    char finding[WDEP_WORKFLOW_TEXT_CAP];
} WdepWorkflowEvidence;

typedef struct WdepWorkflowRun {
    bool preflight_approved;
    bool completed;
    uint16_t attempted;
    uint16_t passed;
    uint16_t failed;
    WdepWorkflowEvidence evidence[WDEP_WORKFLOW_STEP_CAP];
    WdepWorkflowDiagnostic diagnostic;
} WdepWorkflowRun;

typedef bool (*WdepWorkflowHandler)(WdepWorkflowAction action,
                                     void *context,
                                     char *finding,
                                     size_t finding_capacity);

typedef struct WdepWorkflowHost {
    WdepWorkflowHandler handler;
    void *context;
} WdepWorkflowHost;

typedef struct WdepWorkflowSession {
    WdepWorkflow program;
    WdepWorkflowPolicy policy;
    WdepWorkflowHost host;
    WdepWorkflowRun run;
    uint16_t cursor;
    WdepWorkflowState state;
} WdepWorkflowSession;

bool wdep_workflow_compile(const char *source, size_t length,
                            WdepWorkflow *program,
                            WdepWorkflowDiagnostic *diagnostic);
bool wdep_workflow_preflight(const WdepWorkflow *program,
                             const WdepWorkflowPolicy *policy,
                             WdepWorkflowDiagnostic *diagnostic);
bool wdep_workflow_begin(WdepWorkflowSession *session,
                          const WdepWorkflow *program,
                          const WdepWorkflowPolicy *policy,
                          const WdepWorkflowHost *host,
                          WdepWorkflowDiagnostic *diagnostic);
/* Executes no more than one allowlisted action per call. */
WdepWorkflowState wdep_workflow_tick(WdepWorkflowSession *session);
const char *wdep_workflow_action_name(WdepWorkflowAction action);
const char *wdep_workflow_error_name(WdepWorkflowError error);
const char *wdep_workflow_state_name(WdepWorkflowState state);

#endif
