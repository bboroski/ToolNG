#ifndef TOOLZNG_SAMBU_ENGINE_H
#define TOOLZNG_SAMBU_ENGINE_H

#include <stddef.h>
#include <stdint.h>

#define SAMBU_ENGINE_VERSION "1.0.0"
#define SAMBU_REPORT_SCHEMA "toolzng.diagnostic.v1"
#define SAMBU_LEDGER_CAPACITY 32u

typedef enum SambuOperation {
    SAMBU_OP_READ_LOCAL = 1,
    SAMBU_OP_SCAN = 2,
    SAMBU_OP_WRITE_PRIVATE_REPORT = 3,
    SAMBU_OP_EXPORT = 4,
    SAMBU_OP_CLIPBOARD = 5,
    SAMBU_OP_MUTATE_DEVICE = 6,
    SAMBU_OP_UPLOAD_DATA = 7
} SambuOperation;

typedef enum SambuOrigin {
    SAMBU_ORIGIN_INTERNAL = 0,
    SAMBU_ORIGIN_USER = 1
} SambuOrigin;

typedef enum SambuFinding {
    SAMBU_FINDING_NONE = 0,
    SAMBU_FINDING_SCAN_UNAVAILABLE = 1,
    SAMBU_FINDING_NO_DEVICE = 2,
    SAMBU_FINDING_SERVICE_UNAVAILABLE = 3,
    SAMBU_FINDING_USB_UNSTABLE = 4,
    SAMBU_FINDING_BACKUP_STRUCTURE = 5,
    SAMBU_FINDING_BACKUP_STALE = 6,
    SAMBU_FINDING_BACKUP_MISSING = 7,
    SAMBU_FINDING_HEALTHY = 8
} SambuFinding;

enum {
    SAMBU_EVIDENCE_SCAN_COMPLETE = 1u << 0,
    SAMBU_EVIDENCE_DEVICE_VISIBLE = 1u << 1,
    SAMBU_EVIDENCE_SERVICE_RUNNING = 1u << 2,
    SAMBU_EVIDENCE_BACKUP_FOUND = 1u << 3,
    SAMBU_EVIDENCE_BACKUP_MARKERS = 1u << 4,
    SAMBU_EVIDENCE_USB_WATCH = 1u << 5,
    SAMBU_EVIDENCE_USB_REMOVAL = 1u << 6,
    SAMBU_EVIDENCE_STABILITY_WINDOW = 1u << 7
};

typedef struct SambuEvidence {
    int scan_complete;
    int device_found;
    int service_running;
    int backup_found;
    int backup_marker_valid;
    int backup_folders_seen;
    int backup_age_known;
    uint32_t backup_age_days;
    uint32_t recent_usb_removals;
    int usb_watch_registered;
    int stability_window_elapsed;
} SambuEvidence;

typedef struct SambuAssessment {
    SambuFinding finding;
    int readiness_score;
    unsigned confidence_percent;
    uint32_t evidence_mask;
} SambuAssessment;

typedef struct SambuLedgerEntry {
    uint32_t finding_code;
    uint32_t occurrences;
    uint64_t first_seen_ms;
    uint64_t last_seen_ms;
} SambuLedgerEntry;

typedef struct SambuLedger {
    SambuLedgerEntry entries[SAMBU_LEDGER_CAPACITY];
    size_t count;
    size_t next;
} SambuLedger;

/* S — fail-closed action policy. Device mutation and network upload are never granted. */
int sambu_authorize(SambuOperation operation, SambuOrigin origin);

/* A/V — deterministic evidence assessment and explainable readiness score. */
SambuAssessment sambu_assess(const SambuEvidence *evidence);

/* M — bounded failure/finding memory; stores codes and counters, never raw device data. */
void sambu_ledger_init(SambuLedger *ledger);
void sambu_ledger_record(SambuLedger *ledger, uint32_t finding_code,
                         uint64_t monotonic_ms);
uint32_t sambu_ledger_occurrences(const SambuLedger *ledger,
                                  uint32_t finding_code);
uint32_t sambu_ledger_unique_count(const SambuLedger *ledger);

/* B — validate the dependency graph before the application starts. */
int sambu_architecture_validate(void);

/* U — stable identifiers for reports and integrations. */
const char *sambu_finding_key(SambuFinding finding);
const char *sambu_engine_version(void);
const char *sambu_report_schema(void);

#endif
