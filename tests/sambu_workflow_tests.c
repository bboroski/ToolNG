#include "sambu_workflow.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>

static const char valid_source[] =
    "workflow apple_audit {\n"
    "  manifest { purpose \"Unit test\"; max_steps 3; autonomy local_record; }\n"
    "  step usb = observe.apple_usb;\n"
    "  step profile = record.link_profile;\n"
    "  step ready = compute.readiness;\n"
    "}\n";

typedef struct HostState {
    unsigned calls;
    unsigned fail_at;
} HostState;

static bool test_host(WdecWorkflowAction action, void *context,
                      char *finding, size_t finding_capacity)
{
    HostState *state = (HostState *)context;
    ++state->calls;
    (void)snprintf(finding, finding_capacity, "host call %u action %s",
                   state->calls, wdec_workflow_action_name(action));
    return state->fail_at == 0u || state->calls != state->fail_at;
}

static void test_compile_and_incremental_run(void)
{
    WdecWorkflow program;
    WdecWorkflowDiagnostic diagnostic;
    WdecWorkflowPolicy policy = { 5u, true, true };
    WdecWorkflowHost host;
    WdecWorkflowSession session;
    HostState state = { 0u, 0u };

    assert(wdec_workflow_compile(valid_source, sizeof(valid_source) - 1u,
                                  &program, &diagnostic));
    assert(program.step_count == 3u);
    assert(program.steps[0].source.line == 3u);
    assert(program.steps[0].source.column == 3u);
    assert(program.steps[0].source.length > 4u);
    assert(program.autonomy == WDEC_WF_LOCAL_RECORD);
    host.handler = test_host;
    host.context = &state;
    assert(wdec_workflow_begin(&session, &program, &policy, &host, &diagnostic));
    assert(session.run.preflight_approved);
    assert(wdec_workflow_tick(&session) == WDEC_WF_RUNNING);
    assert(state.calls == 1u);
    assert(session.run.attempted == 1u && session.run.passed == 1u);
    assert(wdec_workflow_tick(&session) == WDEC_WF_RUNNING);
    assert(state.calls == 2u);
    assert(wdec_workflow_tick(&session) == WDEC_WF_COMPLETED);
    assert(session.run.completed && session.run.passed == 3u);
    assert(state.calls == 3u);
    assert(session.run.evidence[1].source.line == 4u);
}

static void test_unknown_action_and_duplicate(void)
{
    static const char unknown[] =
        "workflow bad { manifest { purpose \"x\"; max_steps 1; autonomy observe_only; } "
        "step run = system.shell; }";
    static const char duplicate[] =
        "workflow dup { manifest { purpose \"x\"; max_steps 2; autonomy observe_only; } "
        "step same = observe.apple_usb; step same = observe.apple_service; }";
    WdecWorkflow program;
    WdecWorkflowDiagnostic diagnostic;
    assert(!wdec_workflow_compile(unknown, sizeof(unknown) - 1u, &program, &diagnostic));
    assert(diagnostic.code == WDEC_WF_UNKNOWN_ACTION);
    assert(diagnostic.source.line == 1u);
    assert(!wdec_workflow_compile(duplicate, sizeof(duplicate) - 1u, &program, &diagnostic));
    assert(diagnostic.code == WDEC_WF_DUPLICATE_STEP);
}

static void test_policy_and_budget(void)
{
    WdecWorkflow program;
    WdecWorkflowDiagnostic diagnostic;
    WdecWorkflowPolicy blocked = { 8u, true, false };
    assert(wdec_workflow_compile(valid_source, sizeof(valid_source) - 1u,
                                  &program, &diagnostic));
    assert(!wdec_workflow_preflight(&program, &blocked, &diagnostic));
    assert(diagnostic.code == WDEC_WF_POLICY);
    blocked.allow_local_record = true;
    blocked.max_steps = 2u;
    assert(!wdec_workflow_preflight(&program, &blocked, &diagnostic));
    assert(diagnostic.code == WDEC_WF_BUDGET);
}

static void test_missing_manifest_field_and_source_limit(void)
{
    static const char missing[] =
        "workflow bad { manifest { purpose \"x\"; max_steps 1; } "
        "step usb = observe.apple_usb; }";
    char large[WDEC_WORKFLOW_SOURCE_CAP + 1u];
    WdecWorkflow program;
    WdecWorkflowDiagnostic diagnostic;
    assert(!wdec_workflow_compile(missing, sizeof(missing) - 1u, &program, &diagnostic));
    assert(diagnostic.code == WDEC_WF_SYNTAX);
    memset(large, ' ', sizeof(large));
    assert(!wdec_workflow_compile(large, sizeof(large), &program, &diagnostic));
    assert(diagnostic.code == WDEC_WF_SOURCE_LIMIT);
}


static void test_observe_only_cannot_record(void)
{
    static const char source[] =
        "workflow restricted { manifest { purpose \"observe only\"; max_steps 1; autonomy observe_only; } "
        "step profile = record.link_profile; }";
    WdecWorkflow program;
    WdecWorkflowDiagnostic diagnostic;
    WdecWorkflowPolicy policy = { 8u, true, true };
    assert(wdec_workflow_compile(source, sizeof(source) - 1u, &program, &diagnostic));
    assert(!wdec_workflow_preflight(&program, &policy, &diagnostic));
    assert(diagnostic.code == WDEC_WF_POLICY);
}

static void test_rejects_unbounded_numeric_manifest(void)
{
    static const char source[] =
        "workflow bad { manifest { purpose \"x\"; max_steps 999999999999999999999999; "
        "autonomy observe_only; } step usb = observe.apple_usb; }";
    WdecWorkflow program;
    WdecWorkflowDiagnostic diagnostic;
    assert(!wdec_workflow_compile(source, sizeof(source) - 1u, &program, &diagnostic));
    assert(diagnostic.code == WDEC_WF_BUDGET);
}

static void test_fail_fast(void)
{
    WdecWorkflow program;
    WdecWorkflowDiagnostic diagnostic;
    WdecWorkflowPolicy policy = { 5u, true, true };
    WdecWorkflowHost host;
    WdecWorkflowSession session;
    HostState state = { 0u, 2u };
    assert(wdec_workflow_compile(valid_source, sizeof(valid_source) - 1u,
                                  &program, &diagnostic));
    host.handler = test_host;
    host.context = &state;
    assert(wdec_workflow_begin(&session, &program, &policy, &host, &diagnostic));
    assert(wdec_workflow_tick(&session) == WDEC_WF_RUNNING);
    assert(wdec_workflow_tick(&session) == WDEC_WF_FAILED);
    assert(session.run.attempted == 2u);
    assert(session.run.passed == 1u && session.run.failed == 1u);
    assert(session.run.evidence[2].sequence == 0u);
    assert(state.calls == 2u);
}

int main(void)
{
    test_compile_and_incremental_run();
    test_unknown_action_and_duplicate();
    test_policy_and_budget();
    test_missing_manifest_field_and_source_limit();
    test_observe_only_cannot_record();
    test_rejects_unbounded_numeric_manifest();
    test_fail_fast();
    puts("WDEC workflow tests passed.");
    return 0;
}
