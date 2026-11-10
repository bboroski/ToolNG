#include "sambu_engine.h"

#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

static int test_policy(void)
{
    CHECK(sambu_authorize(SAMBU_OP_SCAN, SAMBU_ORIGIN_INTERNAL));
    CHECK(sambu_authorize(SAMBU_OP_READ_LOCAL, SAMBU_ORIGIN_INTERNAL));
    CHECK(!sambu_authorize(SAMBU_OP_EXPORT, SAMBU_ORIGIN_INTERNAL));
    CHECK(sambu_authorize(SAMBU_OP_EXPORT, SAMBU_ORIGIN_USER));
    CHECK(!sambu_authorize(SAMBU_OP_MUTATE_DEVICE, SAMBU_ORIGIN_USER));
    CHECK(!sambu_authorize(SAMBU_OP_UPLOAD_DATA, SAMBU_ORIGIN_USER));
    return 0;
}

static int test_assessment(void)
{
    SambuEvidence evidence;
    SambuAssessment result;

    memset(&evidence, 0, sizeof(evidence));
    result = sambu_assess(&evidence);
    CHECK(result.finding == SAMBU_FINDING_SCAN_UNAVAILABLE);
    CHECK(result.readiness_score == 0);
    CHECK(result.confidence_percent == 40u);

    evidence.scan_complete = 1;
    evidence.device_found = 1;
    evidence.service_running = 1;
    evidence.backup_found = 1;
    evidence.backup_marker_valid = 1;
    evidence.usb_watch_registered = 1;
    evidence.stability_window_elapsed = 1;
    result = sambu_assess(&evidence);
    CHECK(result.finding == SAMBU_FINDING_HEALTHY);
    CHECK(result.readiness_score == 100);
    CHECK(result.confidence_percent == 100u);

    evidence.backup_age_known = 1;
    evidence.backup_age_days = 31u;
    result = sambu_assess(&evidence);
    CHECK(result.finding == SAMBU_FINDING_BACKUP_STALE);

    evidence.recent_usb_removals = 1u;
    result = sambu_assess(&evidence);
    CHECK(result.finding == SAMBU_FINDING_USB_UNSTABLE);
    return 0;
}

static int test_ledger(void)
{
    SambuLedger ledger;
    uint32_t i;

    sambu_ledger_init(&ledger);
    sambu_ledger_record(&ledger, 7u, 100u);
    sambu_ledger_record(&ledger, 7u, 200u);
    CHECK(sambu_ledger_unique_count(&ledger) == 1u);
    CHECK(sambu_ledger_occurrences(&ledger, 7u) == 2u);

    for (i = 1u; i <= SAMBU_LEDGER_CAPACITY + 8u; ++i)
        sambu_ledger_record(&ledger, 100u + i, 300u + i);
    CHECK(sambu_ledger_unique_count(&ledger) == SAMBU_LEDGER_CAPACITY);
    CHECK(sambu_ledger_occurrences(&ledger, 108u) == 0u);
    return 0;
}

int main(void)
{
    CHECK(test_policy() == 0);
    CHECK(test_assessment() == 0);
    CHECK(test_ledger() == 0);
    CHECK(sambu_architecture_validate());
    CHECK(strcmp(sambu_engine_version(), "1.0.0") == 0);
    CHECK(strcmp(sambu_report_schema(), "toolzng.diagnostic.v1") == 0);
    puts("SAMBU engine tests passed.");
    return 0;
}
