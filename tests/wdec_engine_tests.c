#include "wdec_engine.h"

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

static void set_complete_healthy_evidence(WdecEvidence *evidence)
{
    assert(wdec_authorize(WDEC_OP_SCAN, WDEC_ORIGIN_INTERNAL));
    assert(wdec_authorize(WDEC_OP_READ_LOCAL, WDEC_ORIGIN_INTERNAL));
    assert(!wdec_authorize(WDEC_OP_EXPORT, WDEC_ORIGIN_INTERNAL));
    assert(wdec_authorize(WDEC_OP_EXPORT, WDEC_ORIGIN_USER));
    assert(!wdec_authorize(WDEC_OP_MUTATE_DEVICE, WDEC_ORIGIN_USER));
    assert(!wdec_authorize(WDEC_OP_UPLOAD_DATA, WDEC_ORIGIN_USER));
    assert(wdec_authorize(WDEC_OP_RECORD_LOCAL_PROFILE, WDEC_ORIGIN_INTERNAL));
    assert(!wdec_authorize(WDEC_OP_RECORD_LOCAL_PROFILE, WDEC_ORIGIN_USER));
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
    CHECK(wdec_authorize(WDEC_OP_SCAN, WDEC_ORIGIN_INTERNAL));
    CHECK(wdec_authorize(WDEC_OP_READ_LOCAL, WDEC_ORIGIN_INTERNAL));
    CHECK(!wdec_authorize(WDEC_OP_WRITE_PRIVATE_REPORT, WDEC_ORIGIN_INTERNAL));
    CHECK(wdec_authorize(WDEC_OP_WRITE_PRIVATE_REPORT, WDEC_ORIGIN_USER));
    CHECK(!wdec_authorize(WDEC_OP_EXPORT, WDEC_ORIGIN_INTERNAL));
    CHECK(wdec_authorize(WDEC_OP_EXPORT, WDEC_ORIGIN_USER));
    CHECK(!wdec_authorize(WDEC_OP_CLIPBOARD, WDEC_ORIGIN_INTERNAL));
    CHECK(wdec_authorize(WDEC_OP_CLIPBOARD, WDEC_ORIGIN_USER));
    CHECK(!wdec_authorize(WDEC_OP_MUTATE_DEVICE, WDEC_ORIGIN_USER));
    CHECK(!wdec_authorize(WDEC_OP_UPLOAD_DATA, WDEC_ORIGIN_USER));
    CHECK(!wdec_authorize(WDEC_OP_READ_LOCAL, (WdecOrigin)999));
    CHECK(!wdec_authorize(WDEC_OP_SCAN, (WdecOrigin)999));
    CHECK(!wdec_authorize((WdecOperation)999, WDEC_ORIGIN_USER));
    return 0;
}

static int test_assessment_and_coverage(void)
{
    WdecEvidence evidence;
    WdecAssessment result;

    memset(&evidence, 0, sizeof(evidence));
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_SCAN_UNAVAILABLE);
    CHECK(result.readiness_score == 0);
    CHECK(result.score_coverage_percent == 0u);
    CHECK(result.confidence_percent == 0u);
    CHECK(result.score_provisional == 1);
    CHECK(result.evidence_mask == 0u);

    evidence.scan_complete = 1;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_NO_DEVICE);
    CHECK(result.score_coverage_percent == 40u);
    CHECK(result.readiness_score == 0);
    CHECK((result.evidence_mask & WDEC_EVIDENCE_SCAN_COMPLETE) != 0u);

    /* A known positive signal can score 100 while remaining explicitly partial. */
    evidence.device_found = 1;
    evidence.service_running = 1;
    evidence.backup_found = 1;
    evidence.backup_marker_valid = 1;
    evidence.backup_folders_seen = 1u;
    evidence.backup_folders_valid = 1u;
    evidence.usb_watch_registered = 0;
    evidence.stability_window_elapsed = 0;
    result = wdec_assess(&evidence);
    CHECK(result.readiness_score == 100);
    CHECK(result.score_coverage_percent == 40u);
    CHECK(result.score_provisional == 1);
    CHECK(result.device_points == 40);

    set_complete_healthy_evidence(&evidence);
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_HEALTHY);
    CHECK(result.readiness_score == 100);
    CHECK(result.score_coverage_percent == 100u);
    CHECK(result.confidence_percent == 100u);
    CHECK(result.quality_flags == 0u);
    CHECK(strcmp(wdec_quality_key(result.quality_flags), "clean") == 0);
    CHECK(result.score_provisional == 0);
    CHECK(result.device_points == 40);
    CHECK(result.service_points == 25);
    CHECK(result.backup_points == 10);
    CHECK(result.marker_points == 15);
    CHECK(result.stability_points == 10);
    CHECK((result.evidence_mask & WDEC_EVIDENCE_DEVICE_VISIBLE) != 0u);
    CHECK((result.evidence_mask & WDEC_EVIDENCE_SERVICE_CHECKED) != 0u);
    CHECK((result.evidence_mask & WDEC_EVIDENCE_BACKUP_CHECKED) != 0u);
    CHECK((result.evidence_mask & WDEC_EVIDENCE_BACKUP_MARKERS) != 0u);

    evidence.backup_age_known = 1;
    evidence.backup_age_days = 31u;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_BACKUP_STALE);

    evidence.recent_usb_removals = 1u;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_USB_UNSTABLE);
    CHECK(wdec_assess(NULL).finding == WDEC_FINDING_SCAN_UNAVAILABLE);
    return 0;
}

static int test_unknown_states_are_not_negative_findings(void)
{
    WdecEvidence evidence;
    WdecAssessment result;

    set_complete_healthy_evidence(&evidence);
    evidence.service_query_complete = 0;
    evidence.service_running = 0;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_SERVICE_UNKNOWN);
    CHECK(result.service_points == 0);
    CHECK(result.score_coverage_percent == 75u);

    set_complete_healthy_evidence(&evidence);
    evidence.backup_scan_complete = 0;
    evidence.backup_found = 0;
    evidence.backup_marker_valid = 0;
    evidence.backup_folders_seen = 0;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_BACKUP_UNKNOWN);
    CHECK(result.backup_points == 0);
    CHECK(result.marker_points == 0);
    CHECK(result.score_coverage_percent == 75u);

    set_complete_healthy_evidence(&evidence);
    evidence.usb_watch_registered = 0;
    evidence.stability_window_elapsed = 0;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_STABILITY_UNCONFIRMED);
    CHECK(result.stability_points == 0);
    CHECK(result.score_coverage_percent == 90u);
    return 0;
}

static int test_finding_priority(void)
{
    WdecEvidence evidence;
    WdecAssessment result;

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
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_SCAN_UNAVAILABLE);

    evidence.scan_complete = 1;
    evidence.device_found = 0;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_NO_DEVICE);

    evidence.device_found = 1;
    evidence.service_query_complete = 0;
    evidence.service_running = 0;
    evidence.recent_usb_removals = 1u;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_SERVICE_UNKNOWN);

    evidence.service_query_complete = 1;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_SERVICE_UNAVAILABLE);

    evidence.service_running = 1;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_USB_UNSTABLE);

    evidence.recent_usb_removals = 0u;
    evidence.backup_folders_seen = 2u;
    evidence.backup_folders_valid = 0u;
    evidence.backup_scan_complete = 0;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_BACKUP_UNKNOWN);

    evidence.backup_scan_complete = 1;
    evidence.backup_folders_seen = 2;
    evidence.backup_marker_valid = 0;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_BACKUP_STRUCTURE);

    evidence.backup_marker_valid = 1;
    evidence.backup_folders_valid = 2u;
    evidence.backup_age_known = 1;
    evidence.backup_age_days = 31u;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_BACKUP_STALE);

    evidence.backup_age_known = 0;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_HEALTHY);

    evidence.backup_found = 0;
    evidence.backup_marker_valid = 0;
    evidence.backup_folders_seen = 0u;
    evidence.backup_folders_valid = 0u;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_BACKUP_MISSING);

    evidence.backup_found = 1;
    evidence.backup_marker_valid = 1;
    evidence.backup_folders_seen = 1u;
    evidence.backup_folders_valid = 1u;
    evidence.usb_watch_registered = 0;
    evidence.stability_window_elapsed = 0;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_STABILITY_UNCONFIRMED);
    return 0;
}

static int test_ledger(void)
{
    WdecLedger ledger;
    uint32_t i;

    wdec_ledger_init(&ledger);
    wdec_ledger_record(&ledger, 7u, 100u);
    wdec_ledger_record(&ledger, 7u, 200u);
    CHECK(wdec_ledger_unique_count(&ledger) == 1u);
    CHECK(wdec_ledger_occurrences(&ledger, 7u) == 2u);
    CHECK(ledger.entries[0].first_seen_ms == 100u);
    CHECK(ledger.entries[0].last_seen_ms == 200u);

    for (i = 1u; i <= WDEC_LEDGER_CAPACITY + 8u; ++i)
        wdec_ledger_record(&ledger, 100u + i, 300u + i);
    CHECK(wdec_ledger_unique_count(&ledger) == WDEC_LEDGER_CAPACITY);
    CHECK(wdec_ledger_occurrences(&ledger, 108u) == 0u);
    CHECK(wdec_ledger_occurrences(&ledger, 7u) == 0u);
    CHECK(wdec_ledger_unique_count(NULL) == 0u);
    CHECK(wdec_ledger_occurrences(NULL, 7u) == 0u);
    wdec_ledger_record(NULL, 1u, 1u);
    return 0;
}

static int test_recurrence_pattern(void)
{
    WdecLedger ledger;
    wdec_ledger_init(&ledger);

    wdec_ledger_record(&ledger, 7u, 100u);
    CHECK(wdec_ledger_recent_occurrences(&ledger, 7u, 100u) == 1u);
    CHECK(wdec_ledger_pattern(&ledger, 7u, 100u) == WDEC_PATTERN_SINGLE);

    wdec_ledger_record(&ledger, 7u, 200u);
    CHECK(wdec_ledger_recent_occurrences(&ledger, 7u, 200u) == 2u);
    CHECK(wdec_ledger_pattern(&ledger, 7u, 200u) == WDEC_PATTERN_RECURRENT);

    wdec_ledger_record(&ledger, 7u, 300u);
    wdec_ledger_record(&ledger, 7u, 400u);
    CHECK(wdec_ledger_recent_occurrences(&ledger, 7u, 400u) == 4u);
    CHECK(wdec_ledger_pattern(&ledger, 7u, 400u) == WDEC_PATTERN_PERSISTENT);
    CHECK(strcmp(wdec_pattern_key(WDEC_PATTERN_RECURRENT), "recurrent") == 0);
    CHECK(wdec_ledger_recent_occurrences(&ledger, 7u,
        400u + WDEC_LEDGER_WINDOW_MS + 1u) == 0u);
    CHECK(wdec_ledger_pattern(&ledger, 7u,
        400u + WDEC_LEDGER_WINDOW_MS + 1u) == WDEC_PATTERN_NONE);

    /* A new episode after the window or a monotonic-clock reset starts anew. */
    wdec_ledger_record(&ledger, 7u, 700500u);
    CHECK(wdec_ledger_recent_occurrences(&ledger, 7u, 700500u) == 1u);
    CHECK(wdec_ledger_pattern(&ledger, 7u, 700500u) == WDEC_PATTERN_SINGLE);
    wdec_ledger_record(&ledger, 7u, 50u);
    CHECK(wdec_ledger_recent_occurrences(&ledger, 7u, 50u) == 1u);
    return 0;
}

static int test_quality_gate(void)
{
    WdecEvidence evidence;
    WdecAssessment result;

    memset(&evidence, 0, sizeof(evidence));
    evidence.device_found = 1;
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_SCAN_UNAVAILABLE);
    CHECK(result.quality_flags == WDEC_QUALITY_DEVICE_WITHOUT_SCAN);
    CHECK(result.readiness_score == 0);
    CHECK(result.confidence_percent == 0u);
    CHECK(result.score_coverage_percent == 0u);
    CHECK((result.evidence_mask & WDEC_EVIDENCE_DEVICE_VISIBLE) == 0u);
    CHECK(strcmp(wdec_quality_key(result.quality_flags),
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
    result = wdec_assess(&evidence);
    CHECK(result.finding == WDEC_FINDING_BACKUP_STRUCTURE);
    CHECK(result.quality_flags == WDEC_QUALITY_BACKUP_SET_INCOMPLETE);
    CHECK(result.confidence_percent == 100u);
    CHECK(result.score_coverage_percent == 100u);
    CHECK((result.evidence_mask & WDEC_EVIDENCE_BACKUP_MARKERS) == 0u);
    CHECK(result.readiness_score == 85);
    CHECK(strcmp(wdec_quality_key(result.quality_flags),
                 "backup_set_incomplete") == 0);

    evidence.backup_folders_valid = 4u;
    result = wdec_assess(&evidence);
    CHECK(result.quality_flags == WDEC_QUALITY_BACKUP_COUNTS_INCONSISTENT);
    CHECK(result.confidence_percent == 100u);
    CHECK(result.score_coverage_percent == 100u);

    evidence.backup_folders_seen = 1u;
    evidence.backup_folders_valid = 1u;
    evidence.usb_watch_registered = 0;
    result = wdec_assess(&evidence);
    CHECK(result.quality_flags == WDEC_QUALITY_STABILITY_WITHOUT_WATCH);
    CHECK((result.evidence_mask & WDEC_EVIDENCE_STABILITY_WINDOW) == 0u);
    CHECK(result.readiness_score == 100);
    CHECK(result.score_coverage_percent == 90u);
    CHECK(result.confidence_percent == 90u);

    evidence.usb_watch_registered = 1;
    result = wdec_assess(&evidence);
    CHECK(result.quality_flags == 0u);
    CHECK(result.finding == WDEC_FINDING_HEALTHY);
    CHECK(result.readiness_score == 100);
    CHECK(result.confidence_percent == 100u);
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
    CHECK(wdec_architecture_validate());
    CHECK(strcmp(wdec_engine_version(), "1.3.0") == 0);
    CHECK(strcmp(wdec_report_schema(), "wdec.diagnostic.v1") == 0);
    CHECK(strcmp(wdec_finding_key(WDEC_FINDING_SERVICE_UNKNOWN),
                 "apple_service_status_unknown") == 0);
    CHECK(strcmp(wdec_finding_key(WDEC_FINDING_BACKUP_UNKNOWN),
                 "backup_scan_incomplete") == 0);
    puts("WDEC engine tests passed.");
    return 0;
}
