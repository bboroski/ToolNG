#ifndef TOOLZNG_WDEP_ENGINE_H
#define TOOLZNG_WDEP_ENGINE_H

#include <stddef.h>
#include <stdint.h>

#define WDEP_ENGINE_VERSION "1.4.0"
#define WDEP_REPORT_SCHEMA "wdep.diagnostic.v1"
#define WDEP_LEDGER_CAPACITY 32u
#define WDEP_LEDGER_WINDOW_MS 600000ULL
#define WDEP_RELAXATION_WARMUP_SAMPLES 4u

typedef enum WdepOperation {
    WDEP_OP_READ_LOCAL = 1,
    WDEP_OP_SCAN = 2,
    WDEP_OP_WRITE_PRIVATE_REPORT = 3,
    WDEP_OP_EXPORT = 4,
    WDEP_OP_CLIPBOARD = 5,
    WDEP_OP_MUTATE_DEVICE = 6,
    WDEP_OP_UPLOAD_DATA = 7,
    WDEP_OP_RECORD_LOCAL_PROFILE = 8
} WdepOperation;

typedef enum WdepOrigin {
    WDEP_ORIGIN_INTERNAL = 0,
    WDEP_ORIGIN_USER = 1
} WdepOrigin;

typedef enum WdepFinding {
    WDEP_FINDING_NONE = 0,
    WDEP_FINDING_SCAN_UNAVAILABLE = 1,
    WDEP_FINDING_NO_DEVICE = 2,
    WDEP_FINDING_SERVICE_UNAVAILABLE = 3,
    WDEP_FINDING_USB_UNSTABLE = 4,
    WDEP_FINDING_BACKUP_STRUCTURE = 5,
    WDEP_FINDING_BACKUP_STALE = 6,
    WDEP_FINDING_BACKUP_MISSING = 7,
    WDEP_FINDING_HEALTHY = 8,
    WDEP_FINDING_SERVICE_UNKNOWN = 9,
    WDEP_FINDING_BACKUP_UNKNOWN = 10,
    WDEP_FINDING_STABILITY_UNCONFIRMED = 11
} WdepFinding;

enum {
    WDEP_EVIDENCE_SCAN_COMPLETE = 1u << 0,
    WDEP_EVIDENCE_DEVICE_VISIBLE = 1u << 1,
    WDEP_EVIDENCE_SERVICE_RUNNING = 1u << 2,
    WDEP_EVIDENCE_BACKUP_FOUND = 1u << 3,
    WDEP_EVIDENCE_BACKUP_MARKERS = 1u << 4,
    WDEP_EVIDENCE_USB_WATCH = 1u << 5,
    WDEP_EVIDENCE_USB_REMOVAL = 1u << 6,
    WDEP_EVIDENCE_STABILITY_WINDOW = 1u << 7,
    WDEP_EVIDENCE_SERVICE_CHECKED = 1u << 8,
    WDEP_EVIDENCE_BACKUP_CHECKED = 1u << 9
};

typedef struct WdepEvidence {
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
} WdepEvidence;

enum {
    WDEP_QUALITY_DEVICE_WITHOUT_SCAN = 1u << 0,
    WDEP_QUALITY_BACKUP_COUNTS_INCONSISTENT = 1u << 1,
    WDEP_QUALITY_BACKUP_SET_INCOMPLETE = 1u << 2,
    WDEP_QUALITY_STABILITY_WITHOUT_WATCH = 1u << 3
};

typedef struct WdepAssessment {
    WdepFinding finding;
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
} WdepAssessment;

typedef struct WdepLedgerEntry {
    uint32_t finding_code;
    uint32_t occurrences;
    uint64_t first_seen_ms;
    uint64_t last_seen_ms;
    uint64_t window_start_ms;
    uint32_t window_occurrences;
} WdepLedgerEntry;

typedef enum WdepPattern {
    WDEP_PATTERN_NONE = 0,
    WDEP_PATTERN_SINGLE = 1,
    WDEP_PATTERN_RECURRENT = 2,
    WDEP_PATTERN_PERSISTENT = 3
} WdepPattern;

typedef struct WdepLedger {
    WdepLedgerEntry entries[WDEP_LEDGER_CAPACITY];
    size_t count;
    size_t next;
} WdepLedger;

/* Bounded exponential relaxation for noisy diagnostic time series. */
typedef struct WdepRelaxationState {
    double mean;
    double variance;
    uint64_t last_update_ms;
    uint32_t samples;
    int initialized;
} WdepRelaxationState;

typedef struct WdepRelaxationResult {
    double mean;
    double variance;
    double residual;
    double z_score;
    double alpha;
    uint32_t samples;
    int warm;
} WdepRelaxationResult;

#define WDEP_TRACE_CAPACITY 64u

typedef enum WdepTraceStage {
    WDEP_TRACE_SOURCE = 1,
    WDEP_TRACE_POLICY = 2,
    WDEP_TRACE_EVIDENCE = 3,
    WDEP_TRACE_ASSESSMENT = 4,
    WDEP_TRACE_RELAXATION = 5,
    WDEP_TRACE_REPORT = 6
} WdepTraceStage;

typedef enum WdepTraceOutcome {
    WDEP_TRACE_OBSERVED = 1,
    WDEP_TRACE_ACCEPTED = 2,
    WDEP_TRACE_REJECTED = 3,
    WDEP_TRACE_DEGRADED = 4
} WdepTraceOutcome;

/* Bounded provenance only: no raw device payloads or personal data are stored. */
typedef struct WdepTraceEvent {
    uint64_t sequence;
    uint64_t monotonic_ms;
    uint32_t event_code;
    uint32_t evidence_mask;
    uint32_t quality_flags;
    WdepTraceStage stage;
    WdepTraceOutcome outcome;
} WdepTraceEvent;

typedef struct WdepTrace {
    WdepTraceEvent events[WDEP_TRACE_CAPACITY];
    size_t head;
    size_t count;
    uint64_t next_sequence;
    uint32_t dropped;
} WdepTrace;

void wdep_trace_init(WdepTrace *trace);
int wdep_trace_record(WdepTrace *trace, WdepTraceStage stage,
                       WdepTraceOutcome outcome, uint32_t event_code,
                       uint32_t evidence_mask, uint32_t quality_flags,
                       uint64_t monotonic_ms);
/* Copies the oldest available events first; no more than capacity entries. */
size_t wdep_trace_snapshot(const WdepTrace *trace, WdepTraceEvent *out,
                            size_t capacity);

/* S — fail-closed action policy. Device mutation and network upload are never granted. */
int wdep_authorize(WdepOperation operation, WdepOrigin origin);

/* A/V — deterministic assessment, known-evidence normalization and score breakdown. */
WdepAssessment wdep_assess(const WdepEvidence *evidence);

/* M — bounded failure/finding memory; stores codes and counters, never raw device data. */
void wdep_ledger_init(WdepLedger *ledger);
void wdep_ledger_record(WdepLedger *ledger, uint32_t finding_code,
                         uint64_t monotonic_ms);
uint32_t wdep_ledger_occurrences(const WdepLedger *ledger,
                                  uint32_t finding_code);
uint32_t wdep_ledger_recent_occurrences(const WdepLedger *ledger,
                                         uint32_t finding_code,
                                         uint64_t now_ms);
WdepPattern wdep_ledger_pattern(const WdepLedger *ledger,
                                  uint32_t finding_code,
                                  uint64_t now_ms);
const char *wdep_pattern_key(WdepPattern pattern);
const char *wdep_quality_key(uint32_t quality_flags);
uint32_t wdep_ledger_unique_count(const WdepLedger *ledger);

/* Time-aware exponential relaxation. A backward clock jump restarts the window.
   Invalid inputs are rejected without committing partial state. */
int wdep_relaxation_update(WdepRelaxationState *state, double observation,
                            uint64_t monotonic_ms, double tau_ms,
                            WdepRelaxationResult *result);

/* B — validate the dependency graph before the application starts. */
int wdep_architecture_validate(void);

/* U — stable identifiers for reports and integrations. */
const char *wdep_finding_key(WdepFinding finding);
const char *wdep_engine_version(void);
const char *wdep_report_schema(void);

#endif
