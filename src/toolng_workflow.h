#ifndef TOOLNG_WORKFLOW_H
#define TOOLNG_WORKFLOW_H

#include "toolng_engine.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TOOLNG_WORKFLOW_VERSION "1.1.0"
#define TOOLNG_WORKFLOW_SOURCE_CAP 3072u
#define TOOLNG_WORKFLOW_STEP_CAP 8u
#define TOOLNG_WORKFLOW_NAME_CAP 32u
#define TOOLNG_WORKFLOW_TEXT_CAP 128u

typedef enum ToolNGWorkflowAction {
    TOOLNG_WF_NONE = 0,
    TOOLNG_WF_SCAN_USB,
    TOOLNG_WF_READ_SERVICE,
    TOOLNG_WF_RECORD_PROFILE,
    TOOLNG_WF_AUDIT_BACKUP,
    TOOLNG_WF_ASSESS_READINESS
} ToolNGWorkflowAction;

typedef enum ToolNGWorkflowAutonomy {
    TOOLNG_WF_OBSERVE_ONLY = 0,
    TOOLNG_WF_LOCAL_RECORD = 1
} ToolNGWorkflowAutonomy;

typedef enum ToolNGWorkflowState {
    TOOLNG_WF_IDLE = 0,
    TOOLNG_WF_RUNNING,
    TOOLNG_WF_COMPLETED,
    TOOLNG_WF_FAILED
} ToolNGWorkflowState;

typedef enum ToolNGWorkflowError {
    TOOLNG_WF_OK = 0,
    TOOLNG_WF_ARGUMENT,
    TOOLNG_WF_SOURCE_LIMIT,
    TOOLNG_WF_LEXICAL,
    TOOLNG_WF_SYNTAX,
    TOOLNG_WF_UNKNOWN_ACTION,
    TOOLNG_WF_DUPLICATE_STEP,
    TOOLNG_WF_BUDGET,
    TOOLNG_WF_POLICY,
    TOOLNG_WF_HOST_FAILURE,
    TOOLNG_WF_SESSION_INVALID
} ToolNGWorkflowError;

typedef struct ToolNGSourceSpan {
    size_t offset;
    size_t length;
    uint32_t line;
    uint32_t column;
} ToolNGSourceSpan;

typedef struct ToolNGWorkflowStep {
    char name[TOOLNG_WORKFLOW_NAME_CAP];
    ToolNGWorkflowAction action;
    ToolNGSourceSpan source;
} ToolNGWorkflowStep;

typedef struct ToolNGWorkflow {
    char name[TOOLNG_WORKFLOW_NAME_CAP];
    char purpose[TOOLNG_WORKFLOW_TEXT_CAP];
    uint16_t max_steps;
    uint16_t step_count;
    ToolNGWorkflowAutonomy autonomy;
    ToolNGWorkflowStep steps[TOOLNG_WORKFLOW_STEP_CAP];
} ToolNGWorkflow;

typedef struct ToolNGWorkflowDiagnostic {
    ToolNGWorkflowError code;
    ToolNGSourceSpan source;
    char message[TOOLNG_WORKFLOW_TEXT_CAP];
} ToolNGWorkflowDiagnostic;

typedef struct ToolNGWorkflowPolicy {
    uint16_t max_steps;
    bool allow_observation;
    bool allow_local_record;
} ToolNGWorkflowPolicy;

typedef struct ToolNGWorkflowEvidence {
    uint16_t sequence;
    ToolNGWorkflowAction action;
    bool succeeded;
    ToolNGSourceSpan source;
    char step_name[TOOLNG_WORKFLOW_NAME_CAP];
    char finding[TOOLNG_WORKFLOW_TEXT_CAP];
} ToolNGWorkflowEvidence;

typedef struct ToolNGWorkflowRun {
    bool preflight_approved;
    bool completed;
    uint16_t attempted;
    uint16_t passed;
    uint16_t failed;
    ToolNGWorkflowEvidence evidence[TOOLNG_WORKFLOW_STEP_CAP];
    ToolNGWorkflowDiagnostic diagnostic;
} ToolNGWorkflowRun;

typedef bool (*ToolNGWorkflowHandler)(ToolNGWorkflowAction action,
                                     void *context,
                                     char *finding,
                                     size_t finding_capacity);

typedef struct ToolNGWorkflowHost {
    ToolNGWorkflowHandler handler;
    void *context;
} ToolNGWorkflowHost;

typedef struct ToolNGWorkflowSession {
    ToolNGWorkflow program;
    ToolNGWorkflowPolicy policy;
    ToolNGWorkflowHost host;
    ToolNGWorkflowRun run;
    uint16_t cursor;
    ToolNGWorkflowState state;
} ToolNGWorkflowSession;

bool toolng_workflow_compile(const char *source, size_t length,
                            ToolNGWorkflow *program,
                            ToolNGWorkflowDiagnostic *diagnostic);
bool toolng_workflow_preflight(const ToolNGWorkflow *program,
                             const ToolNGWorkflowPolicy *policy,
                             ToolNGWorkflowDiagnostic *diagnostic);
bool toolng_workflow_begin(ToolNGWorkflowSession *session,
                          const ToolNGWorkflow *program,
                          const ToolNGWorkflowPolicy *policy,
                          const ToolNGWorkflowHost *host,
                          ToolNGWorkflowDiagnostic *diagnostic);
/* Executes no more than one allowlisted action per call. */
ToolNGWorkflowState toolng_workflow_tick(ToolNGWorkflowSession *session);
const char *toolng_workflow_action_name(ToolNGWorkflowAction action);
const char *toolng_workflow_error_name(ToolNGWorkflowError error);
const char *toolng_workflow_state_name(ToolNGWorkflowState state);

#endif
