#ifndef TOOLZNG_SAMBU_WORKFLOW_H
#define TOOLZNG_SAMBU_WORKFLOW_H

#include "sambu_engine.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SAMBU_WORKFLOW_VERSION "1.1.0"
#define SAMBU_WORKFLOW_SOURCE_CAP 3072u
#define SAMBU_WORKFLOW_STEP_CAP 8u
#define SAMBU_WORKFLOW_NAME_CAP 32u
#define SAMBU_WORKFLOW_TEXT_CAP 128u

typedef enum SambuWorkflowAction {
    SAMBU_WF_NONE = 0,
    SAMBU_WF_SCAN_USB,
    SAMBU_WF_READ_SERVICE,
    SAMBU_WF_RECORD_PROFILE,
    SAMBU_WF_AUDIT_BACKUP,
    SAMBU_WF_ASSESS_READINESS
} SambuWorkflowAction;

typedef enum SambuWorkflowAutonomy {
    SAMBU_WF_OBSERVE_ONLY = 0,
    SAMBU_WF_LOCAL_RECORD = 1
} SambuWorkflowAutonomy;

typedef enum SambuWorkflowState {
    SAMBU_WF_IDLE = 0,
    SAMBU_WF_RUNNING,
    SAMBU_WF_COMPLETED,
    SAMBU_WF_FAILED
} SambuWorkflowState;

typedef enum SambuWorkflowError {
    SAMBU_WF_OK = 0,
    SAMBU_WF_ARGUMENT,
    SAMBU_WF_SOURCE_LIMIT,
    SAMBU_WF_LEXICAL,
    SAMBU_WF_SYNTAX,
    SAMBU_WF_UNKNOWN_ACTION,
    SAMBU_WF_DUPLICATE_STEP,
    SAMBU_WF_BUDGET,
    SAMBU_WF_POLICY,
    SAMBU_WF_HOST_FAILURE,
    SAMBU_WF_SESSION_INVALID
} SambuWorkflowError;

typedef struct SambuSourceSpan {
    size_t offset;
    size_t length;
    uint32_t line;
    uint32_t column;
} SambuSourceSpan;

typedef struct SambuWorkflowStep {
    char name[SAMBU_WORKFLOW_NAME_CAP];
    SambuWorkflowAction action;
    SambuSourceSpan source;
} SambuWorkflowStep;

typedef struct SambuWorkflow {
    char name[SAMBU_WORKFLOW_NAME_CAP];
    char purpose[SAMBU_WORKFLOW_TEXT_CAP];
    uint16_t max_steps;
    uint16_t step_count;
    SambuWorkflowAutonomy autonomy;
    SambuWorkflowStep steps[SAMBU_WORKFLOW_STEP_CAP];
} SambuWorkflow;

typedef struct SambuWorkflowDiagnostic {
    SambuWorkflowError code;
    SambuSourceSpan source;
    char message[SAMBU_WORKFLOW_TEXT_CAP];
} SambuWorkflowDiagnostic;

typedef struct SambuWorkflowPolicy {
    uint16_t max_steps;
    bool allow_observation;
    bool allow_local_record;
} SambuWorkflowPolicy;

typedef struct SambuWorkflowEvidence {
    uint16_t sequence;
    SambuWorkflowAction action;
    bool succeeded;
    SambuSourceSpan source;
    char step_name[SAMBU_WORKFLOW_NAME_CAP];
    char finding[SAMBU_WORKFLOW_TEXT_CAP];
} SambuWorkflowEvidence;

typedef struct SambuWorkflowRun {
    bool preflight_approved;
    bool completed;
    uint16_t attempted;
    uint16_t passed;
    uint16_t failed;
    SambuWorkflowEvidence evidence[SAMBU_WORKFLOW_STEP_CAP];
    SambuWorkflowDiagnostic diagnostic;
} SambuWorkflowRun;

typedef bool (*SambuWorkflowHandler)(SambuWorkflowAction action,
                                     void *context,
                                     char *finding,
                                     size_t finding_capacity);

typedef struct SambuWorkflowHost {
    SambuWorkflowHandler handler;
    void *context;
} SambuWorkflowHost;

typedef struct SambuWorkflowSession {
    SambuWorkflow program;
    SambuWorkflowPolicy policy;
    SambuWorkflowHost host;
    SambuWorkflowRun run;
    uint16_t cursor;
    SambuWorkflowState state;
} SambuWorkflowSession;

bool sambu_workflow_compile(const char *source, size_t length,
                            SambuWorkflow *program,
                            SambuWorkflowDiagnostic *diagnostic);
bool sambu_workflow_preflight(const SambuWorkflow *program,
                             const SambuWorkflowPolicy *policy,
                             SambuWorkflowDiagnostic *diagnostic);
bool sambu_workflow_begin(SambuWorkflowSession *session,
                          const SambuWorkflow *program,
                          const SambuWorkflowPolicy *policy,
                          const SambuWorkflowHost *host,
                          SambuWorkflowDiagnostic *diagnostic);
/* Executes no more than one allowlisted action per call. */
SambuWorkflowState sambu_workflow_tick(SambuWorkflowSession *session);
const char *sambu_workflow_action_name(SambuWorkflowAction action);
const char *sambu_workflow_error_name(SambuWorkflowError error);
const char *sambu_workflow_state_name(SambuWorkflowState state);

#endif
