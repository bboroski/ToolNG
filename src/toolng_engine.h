#ifndef TOOLNG_ENGINE_H
#define TOOLNG_ENGINE_H

#include <stddef.h>
#include <stdint.h>

#define TOOLNG_ENGINE_VERSION "1.4.0"
#define TOOLNG_REPORT_SCHEMA "toolng.diagnostic.v1"
#define TOOLNG_LEDGER_CAPACITY 32u
#define TOOLNG_LEDGER_WINDOW_MS 600000ULL
#define TOOLNG_RELAXATION_WARMUP_SAMPLES 4u

typedef enum ToolNGOperation {
    TOOLNG_OP_READ_LOCAL = 1,
    TOOLNG_OP_SCAN = 2,
    TOOLNG_OP_WRITE_PRIVATE_REPORT = 3,
    TOOLNG_OP_EXPORT = 4,
    TOOLNG_OP_CLIPBOARD = 5,
    TOOLNG_OP_MUTATE_DEVICE = 6,
    TOOLNG_OP_UPLOAD_DATA = 7,
    TOOLNG_OP_RECORD_LOCAL_PROFILE = 8
} ToolNGOperation;

typedef enum ToolNGOrigin {
    TOOLNG_ORIGIN_INTERNAL = 0,
    TOOLNG_ORIGIN_USER = 1
} ToolNGOrigin;

typedef enum ToolNGFinding {
    TOOLNG_FINDING_NONE = 0,
    TOOLNG_FINDING_SCAN_UNAVAILABLE = 1,
    TOOLNG_FINDING_NO_DEVICE = 2,
    TOOLNG_FINDING_SERVICE_UNAVAILABLE = 3,
    TOOLNG_FINDING_USB_UNSTABLE = 4,
    TOOLNG_FINDING_BACKUP_STRUCTURE = 5,
    TOOLNG_FINDING_BACKUP_STALE = 6,
    TOOLNG_FINDING_BACKUP_MISSING = 7,
    TOOLNG_FINDING_HEALTHY = 8,
    TOOLNG_FINDING_SERVICE_UNKNOWN = 9,
    TOOLNG_FINDING_BACKUP_UNKNOWN = 10,
    TOOLNG_FINDING_STABILITY_UNCONFIRMED = 11
} ToolNGFinding;

enum {
    TOOLNG_EVIDENCE_SCAN_COMPLETE = 1u << 0,
    TOOLNG_EVIDENCE_DEVICE_VISIBLE = 1u << 1,
    TOOLNG_EVIDENCE_SERVICE_RUNNING = 1u << 2,
    TOOLNG_EVIDENCE_BACKUP_FOUND = 1u << 3,
    TOOLNG_EVIDENCE_BACKUP_MARKERS = 1u << 4,
    TOOLNG_EVIDENCE_USB_WATCH = 1u << 5,
    TOOLNG_EVIDENCE_USB_REMOVAL = 1u << 6,
    TOOLNG_EVIDENCE_STABILITY_WINDOW = 1u << 7,
    TOOLNG_EVIDENCE_SERVICE_CHECKED = 1u << 8,
    TOOLNG_EVIDENCE_BACKUP_CHECKED = 1u << 9
};

typedef struct ToolNGEvidence {
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
} ToolNGEvidence;

enum {
    TOOLNG_QUALITY_DEVICE_WITHOUT_SCAN = 1u << 0,
    TOOLNG_QUALITY_BACKUP_COUNTS_INCONSISTENT = 1u << 1,
    TOOLNG_QUALITY_BACKUP_SET_INCOMPLETE = 1u << 2,
    TOOLNG_QUALITY_STABILITY_WITHOUT_WATCH = 1u << 3
};

typedef struct ToolNGAssessment {
    ToolNGFinding finding;
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
} ToolNGAssessment;

typedef struct ToolNGLedgerEntry {
    uint32_t finding_code;
    uint32_t occurrences;
    uint64_t first_seen_ms;
    uint64_t last_seen_ms;
    uint64_t window_start_ms;
    uint32_t window_occurrences;
} ToolNGLedgerEntry;

typedef enum ToolNGPattern {
    TOOLNG_PATTERN_NONE = 0,
    TOOLNG_PATTERN_SINGLE = 1,
    TOOLNG_PATTERN_RECURRENT = 2,
    TOOLNG_PATTERN_PERSISTENT = 3
} ToolNGPattern;

typedef struct ToolNGLedger {
    ToolNGLedgerEntry entries[TOOLNG_LEDGER_CAPACITY];
    size_t count;
    size_t next;
} ToolNGLedger;

/* Bounded exponential relaxation for noisy diagnostic time series. */
typedef struct ToolNGRelaxationState {
    double mean;
    double variance;
    uint64_t last_update_ms;
    uint32_t samples;
    int initialized;
} ToolNGRelaxationState;

typedef struct ToolNGRelaxationResult {
    double mean;
    double variance;
    double residual;
    double z_score;
    double alpha;
    uint32_t samples;
    int warm;
} ToolNGRelaxationResult;

#define TOOLNG_TRACE_CAPACITY 64u

typedef enum ToolNGTraceStage {
    TOOLNG_TRACE_SOURCE = 1,
    TOOLNG_TRACE_POLICY = 2,
    TOOLNG_TRACE_EVIDENCE = 3,
    TOOLNG_TRACE_ASSESSMENT = 4,
    TOOLNG_TRACE_RELAXATION = 5,
    TOOLNG_TRACE_REPORT = 6
} ToolNGTraceStage;

typedef enum ToolNGTraceOutcome {
    TOOLNG_TRACE_OBSERVED = 1,
    TOOLNG_TRACE_ACCEPTED = 2,
    TOOLNG_TRACE_REJECTED = 3,
    TOOLNG_TRACE_DEGRADED = 4
} ToolNGTraceOutcome;

/* Bounded provenance only: no raw device payloads or personal data are stored. */
typedef struct ToolNGTraceEvent {
    uint64_t sequence;
    uint64_t monotonic_ms;
    uint32_t event_code;
    uint32_t evidence_mask;
    uint32_t quality_flags;
    ToolNGTraceStage stage;
    ToolNGTraceOutcome outcome;
} ToolNGTraceEvent;

typedef struct ToolNGTrace {
    ToolNGTraceEvent events[TOOLNG_TRACE_CAPACITY];
    size_t head;
    size_t count;
    uint64_t next_sequence;
    uint32_t dropped;
} ToolNGTrace;

void toolng_trace_init(ToolNGTrace *trace);
int toolng_trace_record(ToolNGTrace *trace, ToolNGTraceStage stage,
                       ToolNGTraceOutcome outcome, uint32_t event_code,
                       uint32_t evidence_mask, uint32_t quality_flags,
                       uint64_t monotonic_ms);
/* Copies the oldest available events first; no more than capacity entries. */
size_t toolng_trace_snapshot(const ToolNGTrace *trace, ToolNGTraceEvent *out,
                            size_t capacity);

/* S — fail-closed action policy. Device mutation and network upload are never granted. */
int toolng_authorize(ToolNGOperation operation, ToolNGOrigin origin);

/* A/V — deterministic assessment, known-evidence normalization and score breakdown. */
ToolNGAssessment toolng_assess(const ToolNGEvidence *evidence);

/* M — bounded failure/finding memory; stores codes and counters, never raw device data. */
void toolng_ledger_init(ToolNGLedger *ledger);
void toolng_ledger_record(ToolNGLedger *ledger, uint32_t finding_code,
                         uint64_t monotonic_ms);
uint32_t toolng_ledger_occurrences(const ToolNGLedger *ledger,
                                  uint32_t finding_code);
uint32_t toolng_ledger_recent_occurrences(const ToolNGLedger *ledger,
                                         uint32_t finding_code,
                                         uint64_t now_ms);
ToolNGPattern toolng_ledger_pattern(const ToolNGLedger *ledger,
                                  uint32_t finding_code,
                                  uint64_t now_ms);
const char *toolng_pattern_key(ToolNGPattern pattern);
const char *toolng_quality_key(uint32_t quality_flags);
uint32_t toolng_ledger_unique_count(const ToolNGLedger *ledger);

/* Time-aware exponential relaxation. A backward clock jump restarts the window.
   Invalid inputs are rejected without committing partial state. */
int toolng_relaxation_update(ToolNGRelaxationState *state, double observation,
                            uint64_t monotonic_ms, double tau_ms,
                            ToolNGRelaxationResult *result);

/* B — validate the dependency graph before the application starts. */
int toolng_architecture_validate(void);

/* U — stable identifiers for reports and integrations. */
const char *toolng_finding_key(ToolNGFinding finding);
const char *toolng_engine_version(void);
const char *toolng_report_schema(void);

#endif
