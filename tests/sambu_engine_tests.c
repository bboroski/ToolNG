#include "sambu_engine.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>

static void test_policy(void)
{
    assert(sambu_authorize(SAMBU_OP_SCAN, SAMBU_ORIGIN_INTERNAL));
    assert(sambu_authorize(SAMBU_OP_READ_LOCAL, SAMBU_ORIGIN_INTERNAL));
    assert(!sambu_authorize(SAMBU_OP_EXPORT, SAMBU_ORIGIN_INTERNAL));
    assert(sambu_authorize(SAMBU_OP_EXPORT, SAMBU_ORIGIN_USER));
    assert(!sambu_authorize(SAMBU_OP_MUTATE_DEVICE, SAMBU_ORIGIN_USER));
    assert(!sambu_authorize(SAMBU_OP_UPLOAD_DATA, SAMBU_ORIGIN_USER));
    assert(sambu_authorize(SAMBU_OP_RECORD_LOCAL_PROFILE, SAMBU_ORIGIN_INTERNAL));
    assert(!sambu_authorize(SAMBU_OP_RECORD_LOCAL_PROFILE, SAMBU_ORIGIN_USER));
}

static void test_assessment(void)
{
    SambuEvidence evidence;
    SambuAssessment result;

    memset(&evidence, 0, sizeof(evidence));
    result = sambu_assess(&evidence);
    assert(result.finding == SAMBU_FINDING_SCAN_UNAVAILABLE);
    assert(result.readiness_score == 0);
    assert(result.confidence_percent == 40u);

    evidence.scan_complete = 1;
    evidence.device_found = 1;
    evidence.service_running = 1;
    evidence.backup_found = 1;
    evidence.backup_marker_valid = 1;
    evidence.usb_watch_registered = 1;
    evidence.stability_window_elapsed = 1;
    result = sambu_assess(&evidence);
    assert(result.finding == SAMBU_FINDING_HEALTHY);
    assert(result.readiness_score == 100);
    assert(result.confidence_percent == 100u);

    evidence.backup_age_known = 1;
    evidence.backup_age_days = 31u;
    result = sambu_assess(&evidence);
    assert(result.finding == SAMBU_FINDING_BACKUP_STALE);

    evidence.recent_usb_removals = 1u;
    result = sambu_assess(&evidence);
    assert(result.finding == SAMBU_FINDING_USB_UNSTABLE);
}

static void test_ledger(void)
{
    SambuLedger ledger;
    uint32_t i;

    sambu_ledger_init(&ledger);
    sambu_ledger_record(&ledger, 7u, 100u);
    sambu_ledger_record(&ledger, 7u, 200u);
    assert(sambu_ledger_unique_count(&ledger) == 1u);
    assert(sambu_ledger_occurrences(&ledger, 7u) == 2u);

    for (i = 1u; i <= SAMBU_LEDGER_CAPACITY + 8u; ++i)
        sambu_ledger_record(&ledger, 100u + i, 300u + i);
    assert(sambu_ledger_unique_count(&ledger) == SAMBU_LEDGER_CAPACITY);
    assert(sambu_ledger_occurrences(&ledger, 108u) == 0u);
}

int main(void)
{
    test_policy();
    test_assessment();
    test_ledger();
    assert(sambu_architecture_validate());
    assert(strcmp(sambu_engine_version(), "1.0.0") == 0);
    assert(strcmp(sambu_report_schema(), "toolzng.diagnostic.v1") == 0);
    return 0;
}
