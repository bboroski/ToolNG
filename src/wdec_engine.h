#ifndef WDEC_ENGINE_H
#define WDEC_ENGINE_H

#include <stddef.h>
#include <stdint.h>

#define WDEC_ENGINE_VERSION "1.3.0"
#define WDEC_REPORT_SCHEMA "wdec.diagnostic.v1"
#define WDEC_LEDGER_CAPACITY 32u
#define WDEC_LEDGER_WINDOW_MS 600000ULL
#define WDEC_RELAXATION_WARMUP_SAMPLES 4u

typedef enum WdecOperation {
    WDEC_OP_READ_LOCAL = 1,
    WDEC_OP_SCAN = 2,
    WDEC_OP_WRITE_PRIVATE_REPORT = 3,
    WDEC_OP_EXPORT = 4,
    WDEC_OP_CLIPBOARD = 5,
    WDEC_OP_MUTATE_DEVICE = 6,
    WDEC_OP_UPLOAD_DATA = 7,
    WDEC_OP_RECORD_LOCAL_PROFILE = 8
} WdecOperation;

typedef enum WdecOrigin {
    WDEC_ORIGIN_INTERNAL = 0,
    WDEC_ORIGIN_USER = 1
} WdecOrigin;

typedef enum WdecFinding {
    WDEC_FINDING_NONE = 0,
    WDEC_FINDING_SCAN_UNAVAILABLE = 1,
    WDEC_FINDING_NO_DEVICE = 2,
    WDEC_FINDING_SERVICE_UNAVAILABLE = 3,
    WDEC_FINDING_USB_UNSTABLE = 4,
    WDEC_FINDING_BACKUP_STRUCTURE = 5,
    WDEC_FINDING_BACKUP_STALE = 6,
    WDEC_FINDING_BACKUP_MISSING = 7,
    WDEC_FINDING_HEALTHY = 8,
    WDEC_FINDING_SERVICE_UNKNOWN = 9,
    WDEC_FINDING_BACKUP_UNKNOWN = 10,
    WDEC_FINDING_STABILITY_UNCONFIRMED = 11
} WdecFinding;

enum {
    WDEC_EVIDENCE_SCAN_COMPLETE = 1u << 0,
    WDEC_EVIDENCE_DEVICE_VISIBLE = 1u << 1,
    WDEC_EVIDENCE_SERVICE_RUNNING = 1u << 2,
    WDEC_EVIDENCE_BACKUP_FOUND = 1u << 3,
    WDEC_EVIDENCE_BACKUP_MARKERS = 1u << 4,
    WDEC_EVIDENCE_USB_WATCH = 1u << 5,
    WDEC_EVIDENCE_USB_REMOVAL = 1u << 6,
    WDEC_EVIDENCE_STABILITY_WINDOW = 1u << 7,
    WDEC_EVIDENCE_SERVICE_CHECKED = 1u << 8,
    WDEC_EVIDENCE_BACKUP_CHECKED = 1u << 9
};

typedef struct WdecEvidence {
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
} WdecEvidence;

enum {
    WDEC_QUALITY_DEVICE_WITHOUT_SCAN = 1u << 0,
    WDEC_QUALITY_BACKUP_COUNTS_INCONSISTENT = 1u << 1,
    WDEC_QUALITY_BACKUP_SET_INCOMPLETE = 1u << 2,
    WDEC_QUALITY_STABILITY_WITHOUT_WATCH = 1u << 3
};

typedef struct WdecAssessment {
    WdecFinding finding;
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
} WdecAssessment;

typedef struct WdecLedgerEntry {
    uint32_t finding_code;
    uint32_t occurrences;
    uint64_t first_seen_ms;
    uint64_t last_seen_ms;
    uint64_t window_start_ms;
    uint32_t window_occurrences;
} WdecLedgerEntry;

typedef enum WdecPattern {
    WDEC_PATTERN_NONE = 0,
    WDEC_PATTERN_SINGLE = 1,
    WDEC_PATTERN_RECURRENT = 2,
    WDEC_PATTERN_PERSISTENT = 3
} WdecPattern;

typedef struct WdecLedger {
    WdecLedgerEntry entries[WDEC_LEDGER_CAPACITY];
    size_t count;
    size_t next;
} WdecLedger;

/* Bounded exponential relaxation for noisy diagnostic time series. */
typedef struct WdecRelaxationState {
    double mean;
    double variance;
    uint64_t last_update_ms;
    uint32_t samples;
    int initialized;
} WdecRelaxationState;

typedef struct WdecRelaxationResult {
    double mean;
    double variance;
    double residual;
    double z_score;
    double alpha;
    uint32_t samples;
    int warm;
} WdecRelaxationResult;

/* S — fail-closed action policy. Device mutation and network upload are never granted. */
int wdec_authorize(WdecOperation operation, WdecOrigin origin);

/* A/V — deterministic assessment, known-evidence normalization and score breakdown. */
WdecAssessment wdec_assess(const WdecEvidence *evidence);

/* M — bounded failure/finding memory; stores codes and counters, never raw device data. */
void wdec_ledger_init(WdecLedger *ledger);
void wdec_ledger_record(WdecLedger *ledger, uint32_t finding_code,
                         uint64_t monotonic_ms);
uint32_t wdec_ledger_occurrences(const WdecLedger *ledger,
                                  uint32_t finding_code);
uint32_t wdec_ledger_recent_occurrences(const WdecLedger *ledger,
                                         uint32_t finding_code,
                                         uint64_t now_ms);
WdecPattern wdec_ledger_pattern(const WdecLedger *ledger,
                                  uint32_t finding_code,
                                  uint64_t now_ms);
const char *wdec_pattern_key(WdecPattern pattern);
const char *wdec_quality_key(uint32_t quality_flags);
uint32_t wdec_ledger_unique_count(const WdecLedger *ledger);

/* Time-aware exponential relaxation. A backward clock jump restarts the window.
   Invalid inputs are rejected without committing partial state. */
int wdec_relaxation_update(WdecRelaxationState *state, double observation,
                            uint64_t monotonic_ms, double tau_ms,
                            WdecRelaxationResult *result);

/* B — validate the dependency graph before the application starts. */
int wdec_architecture_validate(void);

/* U — stable identifiers for reports and integrations. */
const char *wdec_finding_key(WdecFinding finding);
const char *wdec_engine_version(void);
const char *wdec_report_schema(void);

#endif
