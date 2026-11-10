#include "toolng_engine.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

static void set_complete_healthy_evidence(ToolNGEvidence *evidence)
{
    assert(toolng_authorize(TOOLNG_OP_SCAN, TOOLNG_ORIGIN_INTERNAL));
    assert(toolng_authorize(TOOLNG_OP_READ_LOCAL, TOOLNG_ORIGIN_INTERNAL));
    assert(!toolng_authorize(TOOLNG_OP_EXPORT, TOOLNG_ORIGIN_INTERNAL));
    assert(toolng_authorize(TOOLNG_OP_EXPORT, TOOLNG_ORIGIN_USER));
    assert(!toolng_authorize(TOOLNG_OP_MUTATE_DEVICE, TOOLNG_ORIGIN_USER));
    assert(!toolng_authorize(TOOLNG_OP_UPLOAD_DATA, TOOLNG_ORIGIN_USER));
    assert(toolng_authorize(TOOLNG_OP_RECORD_LOCAL_PROFILE, TOOLNG_ORIGIN_INTERNAL));
    assert(!toolng_authorize(TOOLNG_OP_RECORD_LOCAL_PROFILE, TOOLNG_ORIGIN_USER));
    memset(evidence, 0, sizeof(*evidence));
    evidence->scan_complete = 1;
    evidence->device_found = 1;
    evidence->service_query_complete = 1;
    evidence->service_running = 1;
    evidence->backup_scan_complete = 1;
    evidence->backup_found = 1;
    evidence->backup_marker_valid = 1;
    evidence->backup_folders_seen = 1;
    evidence->backup_folders_valid = 1;
    evidence->usb_watch_registered = 1;
    evidence->stability_window_elapsed = 1;
}

static int test_policy(void)
{
    CHECK(toolng_authorize(TOOLNG_OP_SCAN, TOOLNG_ORIGIN_INTERNAL));
    CHECK(toolng_authorize(TOOLNG_OP_READ_LOCAL, TOOLNG_ORIGIN_INTERNAL));
    CHECK(!toolng_authorize(TOOLNG_OP_WRITE_PRIVATE_REPORT, TOOLNG_ORIGIN_INTERNAL));
    CHECK(toolng_authorize(TOOLNG_OP_WRITE_PRIVATE_REPORT, TOOLNG_ORIGIN_USER));
    CHECK(!toolng_authorize(TOOLNG_OP_EXPORT, TOOLNG_ORIGIN_INTERNAL));
    CHECK(toolng_authorize(TOOLNG_OP_EXPORT, TOOLNG_ORIGIN_USER));
    CHECK(!toolng_authorize(TOOLNG_OP_CLIPBOARD, TOOLNG_ORIGIN_INTERNAL));
    CHECK(toolng_authorize(TOOLNG_OP_CLIPBOARD, TOOLNG_ORIGIN_USER));
    CHECK(!toolng_authorize(TOOLNG_OP_MUTATE_DEVICE, TOOLNG_ORIGIN_USER));
    CHECK(!toolng_authorize(TOOLNG_OP_UPLOAD_DATA, TOOLNG_ORIGIN_USER));
    CHECK(!toolng_authorize(TOOLNG_OP_READ_LOCAL, (ToolNGOrigin)999));
    CHECK(!toolng_authorize(TOOLNG_OP_SCAN, (ToolNGOrigin)999));
    CHECK(!toolng_authorize((ToolNGOperation)999, TOOLNG_ORIGIN_USER));
    return 0;
}

static int test_assessment_and_coverage(void)
{
    ToolNGEvidence evidence;
    ToolNGAssessment result;

    memset(&evidence, 0, sizeof(evidence));
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_SCAN_UNAVAILABLE);
    CHECK(result.readiness_score == 0);
    CHECK(result.score_coverage_percent == 0u);
    CHECK(result.confidence_percent == 0u);
    CHECK(result.score_provisional == 1);
    CHECK(result.evidence_mask == 0u);

    evidence.scan_complete = 1;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_NO_DEVICE);
    CHECK(result.score_coverage_percent == 40u);
    CHECK(result.readiness_score == 0);
    CHECK((result.evidence_mask & TOOLNG_EVIDENCE_SCAN_COMPLETE) != 0u);

    /* A known positive signal can score 100 while remaining explicitly partial. */
    evidence.device_found = 1;
    evidence.service_running = 1;
    evidence.backup_found = 1;
    evidence.backup_marker_valid = 1;
    evidence.backup_folders_seen = 1u;
    evidence.backup_folders_valid = 1u;
    evidence.usb_watch_registered = 0;
    evidence.stability_window_elapsed = 0;
    result = toolng_assess(&evidence);
    CHECK(result.readiness_score == 100);
    CHECK(result.score_coverage_percent == 40u);
    CHECK(result.score_provisional == 1);
    CHECK(result.device_points == 40);

    set_complete_healthy_evidence(&evidence);
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_HEALTHY);
    CHECK(result.readiness_score == 100);
    CHECK(result.score_coverage_percent == 100u);
    CHECK(result.confidence_percent == 100u);
    CHECK(result.quality_flags == 0u);
    CHECK(strcmp(toolng_quality_key(result.quality_flags), "clean") == 0);
    CHECK(result.score_provisional == 0);
    CHECK(result.device_points == 40);
    CHECK(result.service_points == 25);
    CHECK(result.backup_points == 10);
    CHECK(result.marker_points == 15);
    CHECK(result.stability_points == 10);
    CHECK((result.evidence_mask & TOOLNG_EVIDENCE_DEVICE_VISIBLE) != 0u);
    CHECK((result.evidence_mask & TOOLNG_EVIDENCE_SERVICE_CHECKED) != 0u);
    CHECK((result.evidence_mask & TOOLNG_EVIDENCE_BACKUP_CHECKED) != 0u);
    CHECK((result.evidence_mask & TOOLNG_EVIDENCE_BACKUP_MARKERS) != 0u);

    evidence.backup_age_known = 1;
    evidence.backup_age_days = 31u;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_BACKUP_STALE);

    evidence.recent_usb_removals = 1u;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_USB_UNSTABLE);
    CHECK(toolng_assess(NULL).finding == TOOLNG_FINDING_SCAN_UNAVAILABLE);
    return 0;
}

static int test_unknown_states_are_not_negative_findings(void)
{
    ToolNGEvidence evidence;
    ToolNGAssessment result;

    set_complete_healthy_evidence(&evidence);
    evidence.service_query_complete = 0;
    evidence.service_running = 0;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_SERVICE_UNKNOWN);
    CHECK(result.service_points == 0);
    CHECK(result.score_coverage_percent == 75u);

    set_complete_healthy_evidence(&evidence);
    evidence.backup_scan_complete = 0;
    evidence.backup_found = 0;
    evidence.backup_marker_valid = 0;
    evidence.backup_folders_seen = 0;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_BACKUP_UNKNOWN);
    CHECK(result.backup_points == 0);
    CHECK(result.marker_points == 0);
    CHECK(result.score_coverage_percent == 75u);

    set_complete_healthy_evidence(&evidence);
    evidence.usb_watch_registered = 0;
    evidence.stability_window_elapsed = 0;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_STABILITY_UNCONFIRMED);
    CHECK(result.stability_points == 0);
    CHECK(result.score_coverage_percent == 90u);
    return 0;
}

static int test_finding_priority(void)
{
    ToolNGEvidence evidence;
    ToolNGAssessment result;

    memset(&evidence, 0, sizeof(evidence));
    evidence.device_found = 1;
    evidence.service_running = 1;
    evidence.backup_found = 1;
    evidence.backup_marker_valid = 1;
    evidence.backup_folders_seen = 2u;
    evidence.backup_folders_valid = 2u;
    evidence.usb_watch_registered = 1;
    evidence.stability_window_elapsed = 1;
    set_complete_healthy_evidence(&evidence);

    evidence.scan_complete = 0;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_SCAN_UNAVAILABLE);

    evidence.scan_complete = 1;
    evidence.device_found = 0;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_NO_DEVICE);

    evidence.device_found = 1;
    evidence.service_query_complete = 0;
    evidence.service_running = 0;
    evidence.recent_usb_removals = 1u;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_SERVICE_UNKNOWN);

    evidence.service_query_complete = 1;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_SERVICE_UNAVAILABLE);

    evidence.service_running = 1;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_USB_UNSTABLE);

    evidence.recent_usb_removals = 0u;
    evidence.backup_folders_seen = 2u;
    evidence.backup_folders_valid = 0u;
    evidence.backup_scan_complete = 0;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_BACKUP_UNKNOWN);

    evidence.backup_scan_complete = 1;
    evidence.backup_folders_seen = 2;
    evidence.backup_marker_valid = 0;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_BACKUP_STRUCTURE);

    evidence.backup_marker_valid = 1;
    evidence.backup_folders_valid = 2u;
    evidence.backup_age_known = 1;
    evidence.backup_age_days = 31u;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_BACKUP_STALE);

    evidence.backup_age_known = 0;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_HEALTHY);

    evidence.backup_found = 0;
    evidence.backup_marker_valid = 0;
    evidence.backup_folders_seen = 0u;
    evidence.backup_folders_valid = 0u;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_BACKUP_MISSING);

    evidence.backup_found = 1;
    evidence.backup_marker_valid = 1;
    evidence.backup_folders_seen = 1u;
    evidence.backup_folders_valid = 1u;
    evidence.usb_watch_registered = 0;
    evidence.stability_window_elapsed = 0;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_STABILITY_UNCONFIRMED);
    return 0;
}

static int test_ledger(void)
{
    ToolNGLedger ledger;
    uint32_t i;

    toolng_ledger_init(&ledger);
    toolng_ledger_record(&ledger, 7u, 100u);
    toolng_ledger_record(&ledger, 7u, 200u);
    CHECK(toolng_ledger_unique_count(&ledger) == 1u);
    CHECK(toolng_ledger_occurrences(&ledger, 7u) == 2u);
    CHECK(ledger.entries[0].first_seen_ms == 100u);
    CHECK(ledger.entries[0].last_seen_ms == 200u);

    for (i = 1u; i <= TOOLNG_LEDGER_CAPACITY + 8u; ++i)
        toolng_ledger_record(&ledger, 100u + i, 300u + i);
    CHECK(toolng_ledger_unique_count(&ledger) == TOOLNG_LEDGER_CAPACITY);
    CHECK(toolng_ledger_occurrences(&ledger, 108u) == 0u);
    CHECK(toolng_ledger_occurrences(&ledger, 7u) == 0u);
    CHECK(toolng_ledger_unique_count(NULL) == 0u);
    CHECK(toolng_ledger_occurrences(NULL, 7u) == 0u);
    toolng_ledger_record(NULL, 1u, 1u);
    return 0;
}

static int test_recurrence_pattern(void)
{
    ToolNGLedger ledger;
    toolng_ledger_init(&ledger);

    toolng_ledger_record(&ledger, 7u, 100u);
    CHECK(toolng_ledger_recent_occurrences(&ledger, 7u, 100u) == 1u);
    CHECK(toolng_ledger_pattern(&ledger, 7u, 100u) == TOOLNG_PATTERN_SINGLE);

    toolng_ledger_record(&ledger, 7u, 200u);
    CHECK(toolng_ledger_recent_occurrences(&ledger, 7u, 200u) == 2u);
    CHECK(toolng_ledger_pattern(&ledger, 7u, 200u) == TOOLNG_PATTERN_RECURRENT);

    toolng_ledger_record(&ledger, 7u, 300u);
    toolng_ledger_record(&ledger, 7u, 400u);
    CHECK(toolng_ledger_recent_occurrences(&ledger, 7u, 400u) == 4u);
    CHECK(toolng_ledger_pattern(&ledger, 7u, 400u) == TOOLNG_PATTERN_PERSISTENT);
    CHECK(strcmp(toolng_pattern_key(TOOLNG_PATTERN_RECURRENT), "recurrent") == 0);
    CHECK(toolng_ledger_recent_occurrences(&ledger, 7u,
        400u + TOOLNG_LEDGER_WINDOW_MS + 1u) == 0u);
    CHECK(toolng_ledger_pattern(&ledger, 7u,
        400u + TOOLNG_LEDGER_WINDOW_MS + 1u) == TOOLNG_PATTERN_NONE);

    /* A new episode after the window or a monotonic-clock reset starts anew. */
    toolng_ledger_record(&ledger, 7u, 700500u);
    CHECK(toolng_ledger_recent_occurrences(&ledger, 7u, 700500u) == 1u);
    CHECK(toolng_ledger_pattern(&ledger, 7u, 700500u) == TOOLNG_PATTERN_SINGLE);
    toolng_ledger_record(&ledger, 7u, 50u);
    CHECK(toolng_ledger_recent_occurrences(&ledger, 7u, 50u) == 1u);
    return 0;
}

static int test_quality_gate(void)
{
    ToolNGEvidence evidence;
    ToolNGAssessment result;

    memset(&evidence, 0, sizeof(evidence));
    evidence.device_found = 1;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_SCAN_UNAVAILABLE);
    CHECK(result.quality_flags == TOOLNG_QUALITY_DEVICE_WITHOUT_SCAN);
    CHECK(result.readiness_score == 0);
    CHECK(result.confidence_percent == 0u);
    CHECK(result.score_coverage_percent == 0u);
    CHECK((result.evidence_mask & TOOLNG_EVIDENCE_DEVICE_VISIBLE) == 0u);
    CHECK(strcmp(toolng_quality_key(result.quality_flags),
                 "device_without_completed_scan") == 0);

    memset(&evidence, 0, sizeof(evidence));
    evidence.scan_complete = 1;
    evidence.device_found = 1;
    evidence.service_query_complete = 1;
    evidence.service_running = 1;
    evidence.backup_scan_complete = 1;
    evidence.backup_found = 1;
    evidence.backup_marker_valid = 1;
    evidence.backup_folders_seen = 3u;
    evidence.backup_folders_valid = 1u;
    evidence.usb_watch_registered = 1;
    evidence.stability_window_elapsed = 1;
    result = toolng_assess(&evidence);
    CHECK(result.finding == TOOLNG_FINDING_BACKUP_STRUCTURE);
    CHECK(result.quality_flags == TOOLNG_QUALITY_BACKUP_SET_INCOMPLETE);
    CHECK(result.confidence_percent == 100u);
    CHECK(result.score_coverage_percent == 100u);
    CHECK((result.evidence_mask & TOOLNG_EVIDENCE_BACKUP_MARKERS) == 0u);
    CHECK(result.readiness_score == 85);
    CHECK(strcmp(toolng_quality_key(result.quality_flags),
                 "backup_set_incomplete") == 0);

    evidence.backup_folders_valid = 4u;
    result = toolng_assess(&evidence);
    CHECK(result.quality_flags == TOOLNG_QUALITY_BACKUP_COUNTS_INCONSISTENT);
    CHECK(result.confidence_percent == 100u);
    CHECK(result.score_coverage_percent == 100u);

    evidence.backup_folders_seen = 1u;
    evidence.backup_folders_valid = 1u;
    evidence.usb_watch_registered = 0;
    result = toolng_assess(&evidence);
    CHECK(result.quality_flags == TOOLNG_QUALITY_STABILITY_WITHOUT_WATCH);
    CHECK((result.evidence_mask & TOOLNG_EVIDENCE_STABILITY_WINDOW) == 0u);
    CHECK(result.readiness_score == 100);
    CHECK(result.score_coverage_percent == 90u);
    CHECK(result.confidence_percent == 90u);

    evidence.usb_watch_registered = 1;
    result = toolng_assess(&evidence);
    CHECK(result.quality_flags == 0u);
    CHECK(result.finding == TOOLNG_FINDING_HEALTHY);
    CHECK(result.readiness_score == 100);
    CHECK(result.confidence_percent == 100u);
    return 0;
}

static int test_bounded_provenance_trace(void)
{
    ToolNGTrace trace;
    ToolNGTraceEvent snapshot[TOOLNG_TRACE_CAPACITY];
    size_t count;
    uint32_t i;

    toolng_trace_init(&trace);
    CHECK(toolng_trace_snapshot(&trace, snapshot, TOOLNG_TRACE_CAPACITY) == 0u);
    CHECK(!toolng_trace_record(&trace, (ToolNGTraceStage)999,
                              TOOLNG_TRACE_OBSERVED, 1u, 0u, 0u, 1u));
    CHECK(!toolng_trace_record(&trace, TOOLNG_TRACE_SOURCE,
                              (ToolNGTraceOutcome)999, 1u, 0u, 0u, 1u));

    for (i = 1u; i <= TOOLNG_TRACE_CAPACITY + 6u; ++i) {
        CHECK(toolng_trace_record(&trace, TOOLNG_TRACE_EVIDENCE,
                                 TOOLNG_TRACE_OBSERVED, i, i, 0u,
                                 (uint64_t)i * 10u));
    }
    CHECK(trace.count == TOOLNG_TRACE_CAPACITY);
    CHECK(trace.dropped == 6u);
    count = toolng_trace_snapshot(&trace, snapshot, TOOLNG_TRACE_CAPACITY);
    CHECK(count == TOOLNG_TRACE_CAPACITY);
    CHECK(snapshot[0].sequence == 7u);
    CHECK(snapshot[0].event_code == 7u);
    CHECK(snapshot[TOOLNG_TRACE_CAPACITY - 1u].sequence ==
          (uint64_t)TOOLNG_TRACE_CAPACITY + 6u);
    CHECK(snapshot[TOOLNG_TRACE_CAPACITY - 1u].monotonic_ms ==
          ((uint64_t)TOOLNG_TRACE_CAPACITY + 6u) * 10u);
    CHECK(snapshot[0].stage == TOOLNG_TRACE_EVIDENCE);
    CHECK(snapshot[0].outcome == TOOLNG_TRACE_OBSERVED);
    CHECK(toolng_trace_snapshot(NULL, snapshot, TOOLNG_TRACE_CAPACITY) == 0u);
    CHECK(toolng_trace_snapshot(&trace, NULL, TOOLNG_TRACE_CAPACITY) == 0u);
    CHECK(toolng_trace_snapshot(&trace, snapshot, 0u) == 0u);
    return 0;
}

int main(void)
{
    CHECK(test_policy() == 0);
    CHECK(test_assessment_and_coverage() == 0);
    CHECK(test_unknown_states_are_not_negative_findings() == 0);
    CHECK(test_finding_priority() == 0);
    CHECK(test_ledger() == 0);
    CHECK(test_recurrence_pattern() == 0);
    CHECK(test_quality_gate() == 0);
    CHECK(test_bounded_provenance_trace() == 0);
    CHECK(toolng_architecture_validate());
    CHECK(strcmp(toolng_engine_version(), "1.4.0") == 0);
    CHECK(strcmp(toolng_report_schema(), "toolng.diagnostic.v1") == 0);
    CHECK(strcmp(toolng_finding_key(TOOLNG_FINDING_SERVICE_UNKNOWN),
                 "apple_service_status_unknown") == 0);
    CHECK(strcmp(toolng_finding_key(TOOLNG_FINDING_BACKUP_UNKNOWN),
                 "backup_scan_incomplete") == 0);
    puts("ToolNG engine tests passed.");
    return 0;
}
