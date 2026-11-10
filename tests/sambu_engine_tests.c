#include "wdep_engine.h"

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

static void set_complete_healthy_evidence(WdepEvidence *evidence)
{
    assert(wdep_authorize(WDEP_OP_SCAN, WDEP_ORIGIN_INTERNAL));
    assert(wdep_authorize(WDEP_OP_READ_LOCAL, WDEP_ORIGIN_INTERNAL));
    assert(!wdep_authorize(WDEP_OP_EXPORT, WDEP_ORIGIN_INTERNAL));
    assert(wdep_authorize(WDEP_OP_EXPORT, WDEP_ORIGIN_USER));
    assert(!wdep_authorize(WDEP_OP_MUTATE_DEVICE, WDEP_ORIGIN_USER));
    assert(!wdep_authorize(WDEP_OP_UPLOAD_DATA, WDEP_ORIGIN_USER));
    assert(wdep_authorize(WDEP_OP_RECORD_LOCAL_PROFILE, WDEP_ORIGIN_INTERNAL));
    assert(!wdep_authorize(WDEP_OP_RECORD_LOCAL_PROFILE, WDEP_ORIGIN_USER));
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
    CHECK(wdep_authorize(WDEP_OP_SCAN, WDEP_ORIGIN_INTERNAL));
    CHECK(wdep_authorize(WDEP_OP_READ_LOCAL, WDEP_ORIGIN_INTERNAL));
    CHECK(!wdep_authorize(WDEP_OP_WRITE_PRIVATE_REPORT, WDEP_ORIGIN_INTERNAL));
    CHECK(wdep_authorize(WDEP_OP_WRITE_PRIVATE_REPORT, WDEP_ORIGIN_USER));
    CHECK(!wdep_authorize(WDEP_OP_EXPORT, WDEP_ORIGIN_INTERNAL));
    CHECK(wdep_authorize(WDEP_OP_EXPORT, WDEP_ORIGIN_USER));
    CHECK(!wdep_authorize(WDEP_OP_CLIPBOARD, WDEP_ORIGIN_INTERNAL));
    CHECK(wdep_authorize(WDEP_OP_CLIPBOARD, WDEP_ORIGIN_USER));
    CHECK(!wdep_authorize(WDEP_OP_MUTATE_DEVICE, WDEP_ORIGIN_USER));
    CHECK(!wdep_authorize(WDEP_OP_UPLOAD_DATA, WDEP_ORIGIN_USER));
    CHECK(!wdep_authorize(WDEP_OP_READ_LOCAL, (WdepOrigin)999));
    CHECK(!wdep_authorize(WDEP_OP_SCAN, (WdepOrigin)999));
    CHECK(!wdep_authorize((WdepOperation)999, WDEP_ORIGIN_USER));
    return 0;
}

static int test_assessment_and_coverage(void)
{
    WdepEvidence evidence;
    WdepAssessment result;

    memset(&evidence, 0, sizeof(evidence));
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_SCAN_UNAVAILABLE);
    CHECK(result.readiness_score == 0);
    CHECK(result.score_coverage_percent == 0u);
    CHECK(result.confidence_percent == 0u);
    CHECK(result.score_provisional == 1);
    CHECK(result.evidence_mask == 0u);

    evidence.scan_complete = 1;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_NO_DEVICE);
    CHECK(result.score_coverage_percent == 40u);
    CHECK(result.readiness_score == 0);
    CHECK((result.evidence_mask & WDEP_EVIDENCE_SCAN_COMPLETE) != 0u);

    /* A known positive signal can score 100 while remaining explicitly partial. */
    evidence.device_found = 1;
    evidence.service_running = 1;
    evidence.backup_found = 1;
    evidence.backup_marker_valid = 1;
    evidence.backup_folders_seen = 1u;
    evidence.backup_folders_valid = 1u;
    evidence.usb_watch_registered = 0;
    evidence.stability_window_elapsed = 0;
    result = wdep_assess(&evidence);
    CHECK(result.readiness_score == 100);
    CHECK(result.score_coverage_percent == 40u);
    CHECK(result.score_provisional == 1);
    CHECK(result.device_points == 40);

    set_complete_healthy_evidence(&evidence);
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_HEALTHY);
    CHECK(result.readiness_score == 100);
    CHECK(result.score_coverage_percent == 100u);
    CHECK(result.confidence_percent == 100u);
    CHECK(result.quality_flags == 0u);
    CHECK(strcmp(wdep_quality_key(result.quality_flags), "clean") == 0);
    CHECK(result.score_provisional == 0);
    CHECK(result.device_points == 40);
    CHECK(result.service_points == 25);
    CHECK(result.backup_points == 10);
    CHECK(result.marker_points == 15);
    CHECK(result.stability_points == 10);
    CHECK((result.evidence_mask & WDEP_EVIDENCE_DEVICE_VISIBLE) != 0u);
    CHECK((result.evidence_mask & WDEP_EVIDENCE_SERVICE_CHECKED) != 0u);
    CHECK((result.evidence_mask & WDEP_EVIDENCE_BACKUP_CHECKED) != 0u);
    CHECK((result.evidence_mask & WDEP_EVIDENCE_BACKUP_MARKERS) != 0u);

    evidence.backup_age_known = 1;
    evidence.backup_age_days = 31u;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_BACKUP_STALE);

    evidence.recent_usb_removals = 1u;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_USB_UNSTABLE);
    CHECK(wdep_assess(NULL).finding == WDEP_FINDING_SCAN_UNAVAILABLE);
    return 0;
}

static int test_unknown_states_are_not_negative_findings(void)
{
    WdepEvidence evidence;
    WdepAssessment result;

    set_complete_healthy_evidence(&evidence);
    evidence.service_query_complete = 0;
    evidence.service_running = 0;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_SERVICE_UNKNOWN);
    CHECK(result.service_points == 0);
    CHECK(result.score_coverage_percent == 75u);

    set_complete_healthy_evidence(&evidence);
    evidence.backup_scan_complete = 0;
    evidence.backup_found = 0;
    evidence.backup_marker_valid = 0;
    evidence.backup_folders_seen = 0;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_BACKUP_UNKNOWN);
    CHECK(result.backup_points == 0);
    CHECK(result.marker_points == 0);
    CHECK(result.score_coverage_percent == 75u);

    set_complete_healthy_evidence(&evidence);
    evidence.usb_watch_registered = 0;
    evidence.stability_window_elapsed = 0;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_STABILITY_UNCONFIRMED);
    CHECK(result.stability_points == 0);
    CHECK(result.score_coverage_percent == 90u);
    return 0;
}

static int test_finding_priority(void)
{
    WdepEvidence evidence;
    WdepAssessment result;

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
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_SCAN_UNAVAILABLE);

    evidence.scan_complete = 1;
    evidence.device_found = 0;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_NO_DEVICE);

    evidence.device_found = 1;
    evidence.service_query_complete = 0;
    evidence.service_running = 0;
    evidence.recent_usb_removals = 1u;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_SERVICE_UNKNOWN);

    evidence.service_query_complete = 1;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_SERVICE_UNAVAILABLE);

    evidence.service_running = 1;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_USB_UNSTABLE);

    evidence.recent_usb_removals = 0u;
    evidence.backup_folders_seen = 2u;
    evidence.backup_folders_valid = 0u;
    evidence.backup_scan_complete = 0;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_BACKUP_UNKNOWN);

    evidence.backup_scan_complete = 1;
    evidence.backup_folders_seen = 2;
    evidence.backup_marker_valid = 0;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_BACKUP_STRUCTURE);

    evidence.backup_marker_valid = 1;
    evidence.backup_folders_valid = 2u;
    evidence.backup_age_known = 1;
    evidence.backup_age_days = 31u;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_BACKUP_STALE);

    evidence.backup_age_known = 0;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_HEALTHY);

    evidence.backup_found = 0;
    evidence.backup_marker_valid = 0;
    evidence.backup_folders_seen = 0u;
    evidence.backup_folders_valid = 0u;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_BACKUP_MISSING);

    evidence.backup_found = 1;
    evidence.backup_marker_valid = 1;
    evidence.backup_folders_seen = 1u;
    evidence.backup_folders_valid = 1u;
    evidence.usb_watch_registered = 0;
    evidence.stability_window_elapsed = 0;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_STABILITY_UNCONFIRMED);
    return 0;
}

static int test_ledger(void)
{
    WdepLedger ledger;
    uint32_t i;

    wdep_ledger_init(&ledger);
    wdep_ledger_record(&ledger, 7u, 100u);
    wdep_ledger_record(&ledger, 7u, 200u);
    CHECK(wdep_ledger_unique_count(&ledger) == 1u);
    CHECK(wdep_ledger_occurrences(&ledger, 7u) == 2u);
    CHECK(ledger.entries[0].first_seen_ms == 100u);
    CHECK(ledger.entries[0].last_seen_ms == 200u);

    for (i = 1u; i <= WDEP_LEDGER_CAPACITY + 8u; ++i)
        wdep_ledger_record(&ledger, 100u + i, 300u + i);
    CHECK(wdep_ledger_unique_count(&ledger) == WDEP_LEDGER_CAPACITY);
    CHECK(wdep_ledger_occurrences(&ledger, 108u) == 0u);
    CHECK(wdep_ledger_occurrences(&ledger, 7u) == 0u);
    CHECK(wdep_ledger_unique_count(NULL) == 0u);
    CHECK(wdep_ledger_occurrences(NULL, 7u) == 0u);
    wdep_ledger_record(NULL, 1u, 1u);
    return 0;
}

static int test_recurrence_pattern(void)
{
    WdepLedger ledger;
    wdep_ledger_init(&ledger);

    wdep_ledger_record(&ledger, 7u, 100u);
    CHECK(wdep_ledger_recent_occurrences(&ledger, 7u, 100u) == 1u);
    CHECK(wdep_ledger_pattern(&ledger, 7u, 100u) == WDEP_PATTERN_SINGLE);

    wdep_ledger_record(&ledger, 7u, 200u);
    CHECK(wdep_ledger_recent_occurrences(&ledger, 7u, 200u) == 2u);
    CHECK(wdep_ledger_pattern(&ledger, 7u, 200u) == WDEP_PATTERN_RECURRENT);

    wdep_ledger_record(&ledger, 7u, 300u);
    wdep_ledger_record(&ledger, 7u, 400u);
    CHECK(wdep_ledger_recent_occurrences(&ledger, 7u, 400u) == 4u);
    CHECK(wdep_ledger_pattern(&ledger, 7u, 400u) == WDEP_PATTERN_PERSISTENT);
    CHECK(strcmp(wdep_pattern_key(WDEP_PATTERN_RECURRENT), "recurrent") == 0);
    CHECK(wdep_ledger_recent_occurrences(&ledger, 7u,
        400u + WDEP_LEDGER_WINDOW_MS + 1u) == 0u);
    CHECK(wdep_ledger_pattern(&ledger, 7u,
        400u + WDEP_LEDGER_WINDOW_MS + 1u) == WDEP_PATTERN_NONE);

    /* A new episode after the window or a monotonic-clock reset starts anew. */
    wdep_ledger_record(&ledger, 7u, 700500u);
    CHECK(wdep_ledger_recent_occurrences(&ledger, 7u, 700500u) == 1u);
    CHECK(wdep_ledger_pattern(&ledger, 7u, 700500u) == WDEP_PATTERN_SINGLE);
    wdep_ledger_record(&ledger, 7u, 50u);
    CHECK(wdep_ledger_recent_occurrences(&ledger, 7u, 50u) == 1u);
    return 0;
}

static int test_quality_gate(void)
{
    WdepEvidence evidence;
    WdepAssessment result;

    memset(&evidence, 0, sizeof(evidence));
    evidence.device_found = 1;
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_SCAN_UNAVAILABLE);
    CHECK(result.quality_flags == WDEP_QUALITY_DEVICE_WITHOUT_SCAN);
    CHECK(result.readiness_score == 0);
    CHECK(result.confidence_percent == 0u);
    CHECK(result.score_coverage_percent == 0u);
    CHECK((result.evidence_mask & WDEP_EVIDENCE_DEVICE_VISIBLE) == 0u);
    CHECK(strcmp(wdep_quality_key(result.quality_flags),
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
    result = wdep_assess(&evidence);
    CHECK(result.finding == WDEP_FINDING_BACKUP_STRUCTURE);
    CHECK(result.quality_flags == WDEP_QUALITY_BACKUP_SET_INCOMPLETE);
    CHECK(result.confidence_percent == 100u);
    CHECK(result.score_coverage_percent == 100u);
    CHECK((result.evidence_mask & WDEP_EVIDENCE_BACKUP_MARKERS) == 0u);
    CHECK(result.readiness_score == 85);
    CHECK(strcmp(wdep_quality_key(result.quality_flags),
                 "backup_set_incomplete") == 0);

    evidence.backup_folders_valid = 4u;
    result = wdep_assess(&evidence);
    CHECK(result.quality_flags == WDEP_QUALITY_BACKUP_COUNTS_INCONSISTENT);
    CHECK(result.confidence_percent == 100u);
    CHECK(result.score_coverage_percent == 100u);

    evidence.backup_folders_seen = 1u;
    evidence.backup_folders_valid = 1u;
    evidence.usb_watch_registered = 0;
    result = wdep_assess(&evidence);
    CHECK(result.quality_flags == WDEP_QUALITY_STABILITY_WITHOUT_WATCH);
    CHECK((result.evidence_mask & WDEP_EVIDENCE_STABILITY_WINDOW) == 0u);
    CHECK(result.readiness_score == 100);
    CHECK(result.score_coverage_percent == 90u);
    CHECK(result.confidence_percent == 90u);

    evidence.usb_watch_registered = 1;
    result = wdep_assess(&evidence);
    CHECK(result.quality_flags == 0u);
    CHECK(result.finding == WDEP_FINDING_HEALTHY);
    CHECK(result.readiness_score == 100);
    CHECK(result.confidence_percent == 100u);
    return 0;
}

static int test_bounded_provenance_trace(void)
{
    WdepTrace trace;
    WdepTraceEvent snapshot[WDEP_TRACE_CAPACITY];
    size_t count;
    uint32_t i;

    wdep_trace_init(&trace);
    CHECK(wdep_trace_snapshot(&trace, snapshot, WDEP_TRACE_CAPACITY) == 0u);
    CHECK(!wdep_trace_record(&trace, (WdepTraceStage)999,
                              WDEP_TRACE_OBSERVED, 1u, 0u, 0u, 1u));
    CHECK(!wdep_trace_record(&trace, WDEP_TRACE_SOURCE,
                              (WdepTraceOutcome)999, 1u, 0u, 0u, 1u));

    for (i = 1u; i <= WDEP_TRACE_CAPACITY + 6u; ++i) {
        CHECK(wdep_trace_record(&trace, WDEP_TRACE_EVIDENCE,
                                 WDEP_TRACE_OBSERVED, i, i, 0u,
                                 (uint64_t)i * 10u));
    }
    CHECK(trace.count == WDEP_TRACE_CAPACITY);
    CHECK(trace.dropped == 6u);
    count = wdep_trace_snapshot(&trace, snapshot, WDEP_TRACE_CAPACITY);
    CHECK(count == WDEP_TRACE_CAPACITY);
    CHECK(snapshot[0].sequence == 7u);
    CHECK(snapshot[0].event_code == 7u);
    CHECK(snapshot[WDEP_TRACE_CAPACITY - 1u].sequence ==
          (uint64_t)WDEP_TRACE_CAPACITY + 6u);
    CHECK(snapshot[WDEP_TRACE_CAPACITY - 1u].monotonic_ms ==
          ((uint64_t)WDEP_TRACE_CAPACITY + 6u) * 10u);
    CHECK(snapshot[0].stage == WDEP_TRACE_EVIDENCE);
    CHECK(snapshot[0].outcome == WDEP_TRACE_OBSERVED);
    CHECK(wdep_trace_snapshot(NULL, snapshot, WDEP_TRACE_CAPACITY) == 0u);
    CHECK(wdep_trace_snapshot(&trace, NULL, WDEP_TRACE_CAPACITY) == 0u);
    CHECK(wdep_trace_snapshot(&trace, snapshot, 0u) == 0u);
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
    CHECK(wdep_architecture_validate());
    CHECK(strcmp(wdep_engine_version(), "1.4.0") == 0);
    CHECK(strcmp(wdep_report_schema(), "toolzng.diagnostic.v1") == 0);
    CHECK(strcmp(wdep_finding_key(WDEP_FINDING_SERVICE_UNKNOWN),
                 "apple_service_status_unknown") == 0);
    CHECK(strcmp(wdep_finding_key(WDEP_FINDING_BACKUP_UNKNOWN),
                 "backup_scan_incomplete") == 0);
    puts("SAMBU engine tests passed.");
    return 0;
}
