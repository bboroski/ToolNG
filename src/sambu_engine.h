#ifndef TOOLZNG_SAMBU_ENGINE_H
#define TOOLZNG_SAMBU_ENGINE_H

#include <stddef.h>
#include <stdint.h>

#define SAMBU_ENGINE_VERSION "1.2.0"
#define SAMBU_REPORT_SCHEMA "toolzng.diagnostic.v1"
#define SAMBU_LEDGER_CAPACITY 32u
#define SAMBU_LEDGER_WINDOW_MS 600000ULL

typedef enum SambuOperation {
    SAMBU_OP_READ_LOCAL = 1,
    SAMBU_OP_SCAN = 2,
    SAMBU_OP_WRITE_PRIVATE_REPORT = 3,
    SAMBU_OP_EXPORT = 4,
    SAMBU_OP_CLIPBOARD = 5,
    SAMBU_OP_MUTATE_DEVICE = 6,
    SAMBU_OP_UPLOAD_DATA = 7,
    SAMBU_OP_RECORD_LOCAL_PROFILE = 8
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
    SAMBU_FINDING_HEALTHY = 8,
    SAMBU_FINDING_SERVICE_UNKNOWN = 9,
    SAMBU_FINDING_BACKUP_UNKNOWN = 10,
    SAMBU_FINDING_STABILITY_UNCONFIRMED = 11
} SambuFinding;

enum {
    SAMBU_EVIDENCE_SCAN_COMPLETE = 1u << 0,
    SAMBU_EVIDENCE_DEVICE_VISIBLE = 1u << 1,
    SAMBU_EVIDENCE_SERVICE_RUNNING = 1u << 2,
    SAMBU_EVIDENCE_BACKUP_FOUND = 1u << 3,
    SAMBU_EVIDENCE_BACKUP_MARKERS = 1u << 4,
    SAMBU_EVIDENCE_USB_WATCH = 1u << 5,
    SAMBU_EVIDENCE_USB_REMOVAL = 1u << 6,
    SAMBU_EVIDENCE_STABILITY_WINDOW = 1u << 7,
    SAMBU_EVIDENCE_SERVICE_CHECKED = 1u << 8,
    SAMBU_EVIDENCE_BACKUP_CHECKED = 1u << 9
};

typedef struct SambuEvidence {
    int scan_complete;
    int device_found;
    int service_query_complete;
    int service_running;
    int backup_scan_complete;
    int backup_found;
    int backup_marker_valid; /* true when at least one discovered folder has valid markers */
    uint32_t backup_folders_seen;
    uint32_t backup_folders_valid;
    int backup_age_known;
    uint32_t backup_age_days;
    uint32_t recent_usb_removals;
    int usb_watch_registered;
    int stability_window_elapsed;
} SambuEvidence;

enum {
    SAMBU_QUALITY_DEVICE_WITHOUT_SCAN = 1u << 0,
    SAMBU_QUALITY_BACKUP_COUNTS_INCONSISTENT = 1u << 1,
    SAMBU_QUALITY_BACKUP_SET_INCOMPLETE = 1u << 2,
    SAMBU_QUALITY_STABILITY_WITHOUT_WATCH = 1u << 3
};

typedef struct SambuAssessment {
    SambuFinding finding;
    int readiness_score;
    /* Kept for report/API compatibility: this is measured coverage, not probability. */
    unsigned confidence_percent;
    unsigned score_coverage_percent;
    int score_provisional;
    int device_points;      /* 0..40 */
    int service_points;     /* 0..25 */
    int backup_points;      /* 0..10 */
    int marker_points;      /* 0..15 */
    int stability_points;   /* 0..10 */
    uint32_t evidence_mask;
    uint32_t quality_flags;
} SambuAssessment;

typedef struct SambuLedgerEntry {
    uint32_t finding_code;
    uint32_t occurrences;
    uint64_t first_seen_ms;
    uint64_t last_seen_ms;
    uint64_t window_start_ms;
    uint32_t window_occurrences;
} SambuLedgerEntry;

typedef enum SambuPattern {
    SAMBU_PATTERN_NONE = 0,
    SAMBU_PATTERN_SINGLE = 1,
    SAMBU_PATTERN_RECURRENT = 2,
    SAMBU_PATTERN_PERSISTENT = 3
} SambuPattern;

typedef struct SambuLedger {
    SambuLedgerEntry entries[SAMBU_LEDGER_CAPACITY];
    size_t count;
    size_t next;
} SambuLedger;

/* S — fail-closed action policy. Device mutation and network upload are never granted. */
int sambu_authorize(SambuOperation operation, SambuOrigin origin);

/* A/V — deterministic assessment, known-evidence normalization and score breakdown. */
SambuAssessment sambu_assess(const SambuEvidence *evidence);

/* M — bounded failure/finding memory; stores codes and counters, never raw device data. */
void sambu_ledger_init(SambuLedger *ledger);
void sambu_ledger_record(SambuLedger *ledger, uint32_t finding_code,
                         uint64_t monotonic_ms);
uint32_t sambu_ledger_occurrences(const SambuLedger *ledger,
                                  uint32_t finding_code);
uint32_t sambu_ledger_recent_occurrences(const SambuLedger *ledger,
                                         uint32_t finding_code,
                                         uint64_t now_ms);
SambuPattern sambu_ledger_pattern(const SambuLedger *ledger,
                                  uint32_t finding_code,
                                  uint64_t now_ms);
const char *sambu_pattern_key(SambuPattern pattern);
const char *sambu_quality_key(uint32_t quality_flags);
uint32_t sambu_ledger_unique_count(const SambuLedger *ledger);

/* B — validate the dependency graph before the application starts. */
int sambu_architecture_validate(void);

/* U — stable identifiers for reports and integrations. */
const char *sambu_finding_key(SambuFinding finding);
const char *sambu_engine_version(void);
const char *sambu_report_schema(void);

#endif
