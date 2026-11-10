#ifndef WDEC_WORKFLOW_H
#define WDEC_WORKFLOW_H

#include "wdec_engine.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define WDEC_WORKFLOW_VERSION "1.1.0"
#define WDEC_WORKFLOW_SOURCE_CAP 3072u
#define WDEC_WORKFLOW_STEP_CAP 8u
#define WDEC_WORKFLOW_NAME_CAP 32u
#define WDEC_WORKFLOW_TEXT_CAP 128u

typedef enum WdecWorkflowAction {
    WDEC_WF_NONE = 0,
    WDEC_WF_SCAN_USB,
    WDEC_WF_READ_SERVICE,
    WDEC_WF_RECORD_PROFILE,
    WDEC_WF_AUDIT_BACKUP,
    WDEC_WF_ASSESS_READINESS
} WdecWorkflowAction;

typedef enum WdecWorkflowAutonomy {
    WDEC_WF_OBSERVE_ONLY = 0,
    WDEC_WF_LOCAL_RECORD = 1
} WdecWorkflowAutonomy;

typedef enum WdecWorkflowState {
    WDEC_WF_IDLE = 0,
    WDEC_WF_RUNNING,
    WDEC_WF_COMPLETED,
    WDEC_WF_FAILED
} WdecWorkflowState;

typedef enum WdecWorkflowError {
    WDEC_WF_OK = 0,
    WDEC_WF_ARGUMENT,
    WDEC_WF_SOURCE_LIMIT,
    WDEC_WF_LEXICAL,
    WDEC_WF_SYNTAX,
    WDEC_WF_UNKNOWN_ACTION,
    WDEC_WF_DUPLICATE_STEP,
    WDEC_WF_BUDGET,
    WDEC_WF_POLICY,
    WDEC_WF_HOST_FAILURE
} WdecWorkflowError;

typedef struct WdecSourceSpan {
    size_t offset;
    size_t length;
    uint32_t line;
    uint32_t column;
} WdecSourceSpan;

typedef struct WdecWorkflowStep {
    char name[WDEC_WORKFLOW_NAME_CAP];
    WdecWorkflowAction action;
    WdecSourceSpan source;
} WdecWorkflowStep;

typedef struct WdecWorkflow {
    char name[WDEC_WORKFLOW_NAME_CAP];
    char purpose[WDEC_WORKFLOW_TEXT_CAP];
    uint16_t max_steps;
    uint16_t step_count;
    WdecWorkflowAutonomy autonomy;
    WdecWorkflowStep steps[WDEC_WORKFLOW_STEP_CAP];
} WdecWorkflow;

typedef struct WdecWorkflowDiagnostic {
    WdecWorkflowError code;
    WdecSourceSpan source;
    char message[WDEC_WORKFLOW_TEXT_CAP];
} WdecWorkflowDiagnostic;

typedef struct WdecWorkflowPolicy {
    uint16_t max_steps;
    bool allow_observation;
    bool allow_local_record;
} WdecWorkflowPolicy;

typedef struct WdecWorkflowEvidence {
    uint16_t sequence;
    WdecWorkflowAction action;
    bool succeeded;
    WdecSourceSpan source;
    char step_name[WDEC_WORKFLOW_NAME_CAP];
    char finding[WDEC_WORKFLOW_TEXT_CAP];
} WdecWorkflowEvidence;

typedef struct WdecWorkflowRun {
    bool preflight_approved;
    bool completed;
    uint16_t attempted;
    uint16_t passed;
    uint16_t failed;
    WdecWorkflowEvidence evidence[WDEC_WORKFLOW_STEP_CAP];
    WdecWorkflowDiagnostic diagnostic;
} WdecWorkflowRun;

typedef bool (*WdecWorkflowHandler)(WdecWorkflowAction action,
                                     void *context,
                                     char *finding,
                                     size_t finding_capacity);

typedef struct WdecWorkflowHost {
    WdecWorkflowHandler handler;
    void *context;
} WdecWorkflowHost;

typedef struct WdecWorkflowSession {
    WdecWorkflow program;
    WdecWorkflowPolicy policy;
    WdecWorkflowHost host;
    WdecWorkflowRun run;
    uint16_t cursor;
    WdecWorkflowState state;
} WdecWorkflowSession;

bool wdec_workflow_compile(const char *source, size_t length,
                            WdecWorkflow *program,
                            WdecWorkflowDiagnostic *diagnostic);
bool wdec_workflow_preflight(const WdecWorkflow *program,
                             const WdecWorkflowPolicy *policy,
                             WdecWorkflowDiagnostic *diagnostic);
bool wdec_workflow_begin(WdecWorkflowSession *session,
                          const WdecWorkflow *program,
                          const WdecWorkflowPolicy *policy,
                          const WdecWorkflowHost *host,
                          WdecWorkflowDiagnostic *diagnostic);
/* Executes no more than one allowlisted action per call. */
WdecWorkflowState wdec_workflow_tick(WdecWorkflowSession *session);
const char *wdec_workflow_action_name(WdecWorkflowAction action);
const char *wdec_workflow_error_name(WdecWorkflowError error);
const char *wdec_workflow_state_name(WdecWorkflowState state);

#endif
