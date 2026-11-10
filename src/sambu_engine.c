#include "sambu_engine.h"

#include <math.h>
#include <string.h>

typedef struct SambuLayer {
    const char *name;
    uint32_t requires;
    uint32_t provides;
} SambuLayer;

enum {
    LAYER_CORE = 1u << 0,
    LAYER_POLICY = 1u << 1,
    LAYER_EVIDENCE = 1u << 2,
    LAYER_PLATFORM = 1u << 3,
    LAYER_MEMORY = 1u << 4,
    LAYER_PRESENTATION = 1u << 5,
    LAYER_EXPORT = 1u << 6
};

static const SambuLayer g_layers[] = {
    { "core", 0u, LAYER_CORE },
    { "policy", LAYER_CORE, LAYER_POLICY },
    { "evidence", LAYER_POLICY, LAYER_EVIDENCE },
    { "platform-adapter", LAYER_CORE | LAYER_POLICY, LAYER_PLATFORM },
    { "memory", LAYER_EVIDENCE, LAYER_MEMORY },
    { "presentation", LAYER_EVIDENCE | LAYER_PLATFORM | LAYER_MEMORY,
      LAYER_PRESENTATION },
    { "export", LAYER_EVIDENCE | LAYER_MEMORY, LAYER_EXPORT }
};

int sambu_authorize(SambuOperation operation, SambuOrigin origin)
{
    /* Reject unknown callers before evaluating any capability. */
    if (origin != SAMBU_ORIGIN_INTERNAL && origin != SAMBU_ORIGIN_USER)
        return 0;

    switch (operation) {
        case SAMBU_OP_READ_LOCAL:
        case SAMBU_OP_SCAN:
            return 1;

        case SAMBU_OP_RECORD_LOCAL_PROFILE:
            return origin == SAMBU_ORIGIN_INTERNAL;

        case SAMBU_OP_WRITE_PRIVATE_REPORT:
        case SAMBU_OP_EXPORT:
        case SAMBU_OP_CLIPBOARD:
            return origin == SAMBU_ORIGIN_USER;

        case SAMBU_OP_MUTATE_DEVICE:
        case SAMBU_OP_UPLOAD_DATA:
        default:
            return 0;
    }
}

SambuAssessment sambu_assess(const SambuEvidence *evidence)
{
    SambuAssessment result;
    const SambuEvidence empty = {0};
    const SambuEvidence *e = evidence ? evidence : &empty;
    int possible = 0;
    int earned = 0;
    int device_verified;
    int backup_counts_consistent;
    int backup_set_complete;
    int stable_window_verified;

    memset(&result, 0, sizeof(result));
    result.finding = SAMBU_FINDING_SCAN_UNAVAILABLE;

    device_verified = e->scan_complete && e->device_found;
    backup_counts_consistent =
        e->backup_folders_valid <= e->backup_folders_seen &&
        ((e->backup_folders_valid > 0u) == (e->backup_marker_valid != 0)) &&
        (e->backup_found ||
         (e->backup_folders_seen == 0u && e->backup_folders_valid == 0u));
    backup_set_complete = e->backup_scan_complete &&
        e->backup_found && e->backup_folders_seen > 0u &&
        backup_counts_consistent &&
        e->backup_folders_valid == e->backup_folders_seen;
    stable_window_verified =
        e->scan_complete && e->device_found &&
        e->usb_watch_registered && e->stability_window_elapsed;

    /* Integrity warnings describe contradictory or incomplete evidence.
       They do not overwrite the measured-coverage percentage. */
    if (e->device_found && !e->scan_complete)
        result.quality_flags |= SAMBU_QUALITY_DEVICE_WITHOUT_SCAN;
    if (e->backup_scan_complete && !backup_counts_consistent)
        result.quality_flags |= SAMBU_QUALITY_BACKUP_COUNTS_INCONSISTENT;
    if (e->backup_scan_complete && e->backup_found &&
        e->backup_folders_seen > 0u && backup_counts_consistent &&
        e->backup_folders_valid < e->backup_folders_seen)
        result.quality_flags |= SAMBU_QUALITY_BACKUP_SET_INCOMPLETE;
    if (e->stability_window_elapsed && !e->usb_watch_registered)
        result.quality_flags |= SAMBU_QUALITY_STABILITY_WITHOUT_WATCH;

    /* Score only checks that completed successfully. */
    if (e->scan_complete) {
        result.evidence_mask |= SAMBU_EVIDENCE_SCAN_COMPLETE;
        possible += 40;
        if (device_verified) {
            result.evidence_mask |= SAMBU_EVIDENCE_DEVICE_VISIBLE;
            result.device_points = 40;
            earned += result.device_points;
        }
    }

    if (e->service_query_complete) {
        result.evidence_mask |= SAMBU_EVIDENCE_SERVICE_CHECKED;
        possible += 25;
        if (e->service_running) {
            result.evidence_mask |= SAMBU_EVIDENCE_SERVICE_RUNNING;
            result.service_points = 25;
            earned += result.service_points;
        }
    }

    if (e->backup_scan_complete) {
        result.evidence_mask |= SAMBU_EVIDENCE_BACKUP_CHECKED;
        possible += 25;
        if (e->backup_found) {
            result.evidence_mask |= SAMBU_EVIDENCE_BACKUP_FOUND;
            result.backup_points = 10;
            earned += result.backup_points;
        }
        if (backup_set_complete) {
            result.evidence_mask |= SAMBU_EVIDENCE_BACKUP_MARKERS;
            result.marker_points = 15;
            earned += result.marker_points;
        }
    }

    if (e->usb_watch_registered)
        result.evidence_mask |= SAMBU_EVIDENCE_USB_WATCH;
    if (e->recent_usb_removals > 0u)
        result.evidence_mask |= SAMBU_EVIDENCE_USB_REMOVAL;
    if (stable_window_verified)
        result.evidence_mask |= SAMBU_EVIDENCE_STABILITY_WINDOW;

    if (stable_window_verified) {
        possible += 10;
        if (e->recent_usb_removals == 0u) {
            result.stability_points = 10;
            earned += result.stability_points;
        }
    }

    if (possible > 100) possible = 100;
    if (earned > possible) earned = possible;
    result.score_coverage_percent = (unsigned)possible;
    result.confidence_percent = result.score_coverage_percent;
    result.score_provisional = possible < 100;
    result.readiness_score = possible > 0 ? (earned * 100) / possible : 0;

    /* Unknown checks remain distinct from verified negative observations. */
    if (!e->scan_complete) {
        result.finding = SAMBU_FINDING_SCAN_UNAVAILABLE;
    } else if (!device_verified) {
        result.finding = SAMBU_FINDING_NO_DEVICE;
    } else if (!e->service_query_complete) {
        result.finding = SAMBU_FINDING_SERVICE_UNKNOWN;
    } else if (!e->service_running) {
        result.finding = SAMBU_FINDING_SERVICE_UNAVAILABLE;
    } else if (e->recent_usb_removals > 0u) {
        result.finding = SAMBU_FINDING_USB_UNSTABLE;
    } else if (!e->backup_scan_complete) {
        result.finding = SAMBU_FINDING_BACKUP_UNKNOWN;
    } else if (e->backup_found && e->backup_folders_seen > 0u &&
               !backup_set_complete) {
        result.finding = SAMBU_FINDING_BACKUP_STRUCTURE;
    } else if (backup_set_complete && e->backup_age_known &&
               e->backup_age_days > 30u) {
        result.finding = SAMBU_FINDING_BACKUP_STALE;
    } else if (!e->backup_found || !backup_set_complete) {
        result.finding = SAMBU_FINDING_BACKUP_MISSING;
    } else if (!stable_window_verified) {
        result.finding = SAMBU_FINDING_STABILITY_UNCONFIRMED;
    } else {
        result.finding = SAMBU_FINDING_HEALTHY;
    }

    return result;
}

void sambu_ledger_init(SambuLedger *ledger)
{
    if (ledger) memset(ledger, 0, sizeof(*ledger));
}

void sambu_ledger_record(SambuLedger *ledger, uint32_t finding_code,
                         uint64_t monotonic_ms)
{
    size_t i;
    SambuLedgerEntry *entry;

    if (!ledger || finding_code == 0u) return;

    for (i = 0; i < ledger->count; ++i) {
        entry = &ledger->entries[i];
        if (entry->finding_code == finding_code) {
            if (entry->occurrences < UINT32_MAX) ++entry->occurrences;
            if (monotonic_ms < entry->last_seen_ms ||
                monotonic_ms < entry->window_start_ms ||
                monotonic_ms - entry->window_start_ms > SAMBU_LEDGER_WINDOW_MS) {
                entry->window_start_ms = monotonic_ms;
                entry->window_occurrences = 1u;
            } else if (entry->window_occurrences < UINT32_MAX) {
                ++entry->window_occurrences;
            }
            entry->last_seen_ms = monotonic_ms;
            return;
        }
    }

    if (ledger->count < SAMBU_LEDGER_CAPACITY) {
        entry = &ledger->entries[ledger->count++];
        ledger->next = ledger->count % SAMBU_LEDGER_CAPACITY;
    } else {
        entry = &ledger->entries[ledger->next];
        ledger->next = (ledger->next + 1u) % SAMBU_LEDGER_CAPACITY;
    }

    entry->finding_code = finding_code;
    entry->occurrences = 1u;
    entry->first_seen_ms = monotonic_ms;
    entry->last_seen_ms = monotonic_ms;
    entry->window_start_ms = monotonic_ms;
    entry->window_occurrences = 1u;
}

uint32_t sambu_ledger_occurrences(const SambuLedger *ledger,
                                  uint32_t finding_code)
{
    size_t i;
    if (!ledger || finding_code == 0u) return 0u;
    for (i = 0; i < ledger->count; ++i) {
        if (ledger->entries[i].finding_code == finding_code)
            return ledger->entries[i].occurrences;
    }
    return 0u;
}

uint32_t sambu_ledger_recent_occurrences(const SambuLedger *ledger,
                                         uint32_t finding_code,
                                         uint64_t now_ms)
{
    size_t i;
    if (!ledger || finding_code == 0u) return 0u;

    for (i = 0; i < ledger->count; ++i) {
        const SambuLedgerEntry *entry = &ledger->entries[i];
        if (entry->finding_code != finding_code) continue;
        if (now_ms < entry->last_seen_ms || now_ms < entry->window_start_ms)
            return 0u;
        if (now_ms - entry->last_seen_ms > SAMBU_LEDGER_WINDOW_MS ||
            now_ms - entry->window_start_ms > SAMBU_LEDGER_WINDOW_MS)
            return 0u;
        return entry->window_occurrences;
    }
    return 0u;
}

SambuPattern sambu_ledger_pattern(const SambuLedger *ledger,
                                  uint32_t finding_code,
                                  uint64_t now_ms)
{
    const uint32_t recent = sambu_ledger_recent_occurrences(
        ledger, finding_code, now_ms);
    if (recent == 0u) return SAMBU_PATTERN_NONE;
    if (recent == 1u) return SAMBU_PATTERN_SINGLE;
    if (recent < 4u) return SAMBU_PATTERN_RECURRENT;
    return SAMBU_PATTERN_PERSISTENT;
}

const char *sambu_pattern_key(SambuPattern pattern)
{
    switch (pattern) {
        case SAMBU_PATTERN_SINGLE: return "single";
        case SAMBU_PATTERN_RECURRENT: return "recurrent";
        case SAMBU_PATTERN_PERSISTENT: return "persistent";
        case SAMBU_PATTERN_NONE:
        default: return "none";
    }
}

const char *sambu_quality_key(uint32_t quality_flags)
{
    if (quality_flags == 0u) return "clean";
    if ((quality_flags & (quality_flags - 1u)) != 0u)
        return "multiple_integrity_warnings";

    switch (quality_flags) {
        case SAMBU_QUALITY_DEVICE_WITHOUT_SCAN:
            return "device_without_completed_scan";
        case SAMBU_QUALITY_BACKUP_COUNTS_INCONSISTENT:
            return "backup_counts_inconsistent";
        case SAMBU_QUALITY_BACKUP_SET_INCOMPLETE:
            return "backup_set_incomplete";
        case SAMBU_QUALITY_STABILITY_WITHOUT_WATCH:
            return "stability_without_usb_watch";
        default:
            return "unknown_integrity_warning";
    }
}

uint32_t sambu_ledger_unique_count(const SambuLedger *ledger)
{
    if (!ledger) return 0u;
    return (uint32_t)ledger->count;
}

int sambu_relaxation_update(SambuRelaxationState *state, double observation,
                            uint64_t monotonic_ms, double tau_ms,
                            SambuRelaxationResult *result)
{
    double alpha;
    double residual;
    double next_mean;
    double next_variance;
    double z_score;

    if (!state || !result || !isfinite(observation) ||
        !isfinite(tau_ms) || tau_ms <= 0.0)
        return 0;

    if (!state->initialized || monotonic_ms < state->last_update_ms) {
        state->mean = observation;
        state->variance = 0.0;
        state->last_update_ms = monotonic_ms;
        state->samples = 1u;
        state->initialized = 1;
        result->mean = state->mean;
        result->variance = 0.0;
        result->residual = 0.0;
        result->z_score = 0.0;
        result->alpha = 1.0;
        result->samples = 1u;
        result->warm = 0;
        return 1;
    }

    if (!isfinite(state->mean) || !isfinite(state->variance) ||
        state->variance < 0.0)
        return 0;

    residual = observation - state->mean;
    if (!isfinite(residual))
        return 0;

    if (monotonic_ms == state->last_update_ms) {
        alpha = 0.0;
    } else {
        const double elapsed = (double)(monotonic_ms - state->last_update_ms);
        const double ratio = elapsed / tau_ms;
        alpha = ratio >= 50.0 ? 1.0 : 1.0 - exp(-ratio);
    }

    next_mean = state->mean + alpha * residual;
    next_variance = (1.0 - alpha) *
        (state->variance + alpha * residual * residual);
    if (!isfinite(next_mean) || !isfinite(next_variance))
        return 0;
    if (next_variance < 0.0)
        next_variance = 0.0;

    z_score = next_variance > 1e-24
        ? residual / sqrt(next_variance) : 0.0;
    if (!isfinite(z_score))
        return 0;

    /* Commit only after every derived value has passed validation. */
    state->mean = next_mean;
    state->variance = next_variance;
    state->last_update_ms = monotonic_ms;
    if (state->samples < UINT32_MAX) ++state->samples;

    result->mean = state->mean;
    result->variance = state->variance;
    result->residual = residual;
    result->z_score = z_score;
    result->alpha = alpha;
    result->samples = state->samples;
    result->warm = state->samples >= SAMBU_RELAXATION_WARMUP_SAMPLES;
    return 1;
}

int sambu_architecture_validate(void)
{
    const size_t layer_count = sizeof(g_layers) / sizeof(g_layers[0]);
    const uint32_t all_layers = (1u << (sizeof(g_layers) / sizeof(g_layers[0]))) - 1u;
    uint32_t available = 0u;
    uint32_t completed = 0u;
    size_t pass;

    if (layer_count == 0u || layer_count > 16u) return 0;

    for (pass = 0; pass < layer_count; ++pass) {
        size_t i;
        int progressed = 0;

        for (i = 0; i < layer_count; ++i) {
            const uint32_t node_bit = 1u << i;
            const SambuLayer *layer = &g_layers[i];

            if ((completed & node_bit) != 0u) continue;
            if ((layer->requires & available) != layer->requires) continue;
            if (layer->name == NULL || layer->name[0] == '\0' ||
                layer->provides == 0u || (layer->provides & available) != 0u)
                return 0;

            available |= layer->provides;
            completed |= node_bit;
            progressed = 1;
        }

        if (completed == all_layers) return 1;
        if (!progressed) return 0;
    }

    return completed == all_layers;
}

const char *sambu_finding_key(SambuFinding finding)
{
    switch (finding) {
        case SAMBU_FINDING_SCAN_UNAVAILABLE: return "scan_unavailable";
        case SAMBU_FINDING_NO_DEVICE: return "apple_device_not_visible";
        case SAMBU_FINDING_SERVICE_UNAVAILABLE: return "apple_service_unavailable";
        case SAMBU_FINDING_USB_UNSTABLE: return "usb_connection_unstable";
        case SAMBU_FINDING_BACKUP_STRUCTURE: return "backup_markers_missing";
        case SAMBU_FINDING_BACKUP_STALE: return "backup_marker_stale";
        case SAMBU_FINDING_BACKUP_MISSING: return "backup_not_verified";
        case SAMBU_FINDING_HEALTHY: return "windows_side_checks_passed";
        case SAMBU_FINDING_SERVICE_UNKNOWN: return "apple_service_status_unknown";
        case SAMBU_FINDING_BACKUP_UNKNOWN: return "backup_scan_incomplete";
        case SAMBU_FINDING_STABILITY_UNCONFIRMED: return "usb_stability_unconfirmed";
        case SAMBU_FINDING_NONE:
        default:
            return "none";
    }
}

const char *sambu_engine_version(void)
{
    return SAMBU_ENGINE_VERSION;
}

const char *sambu_report_schema(void)
{
    return SAMBU_REPORT_SCHEMA;
}


void sambu_trace_init(SambuTrace *trace)
{
    if (trace != NULL) memset(trace, 0, sizeof(*trace));
}

static int sambu_trace_stage_valid(SambuTraceStage stage)
{
    return stage >= SAMBU_TRACE_SOURCE && stage <= SAMBU_TRACE_REPORT;
}

static int sambu_trace_outcome_valid(SambuTraceOutcome outcome)
{
    return outcome >= SAMBU_TRACE_OBSERVED && outcome <= SAMBU_TRACE_DEGRADED;
}

int sambu_trace_record(SambuTrace *trace, SambuTraceStage stage,
                       SambuTraceOutcome outcome, uint32_t event_code,
                       uint32_t evidence_mask, uint32_t quality_flags,
                       uint64_t monotonic_ms)
{
    SambuTraceEvent *event;
    if (trace == NULL || !sambu_trace_stage_valid(stage) ||
        !sambu_trace_outcome_valid(outcome) ||
        trace->next_sequence == UINT64_MAX)
        return 0;

    event = &trace->events[trace->head];
    event->sequence = ++trace->next_sequence;
    event->monotonic_ms = monotonic_ms;
    event->event_code = event_code;
    event->evidence_mask = evidence_mask;
    event->quality_flags = quality_flags;
    event->stage = stage;
    event->outcome = outcome;

    trace->head = (trace->head + 1u) % SAMBU_TRACE_CAPACITY;
    if (trace->count < SAMBU_TRACE_CAPACITY) {
        ++trace->count;
    } else if (trace->dropped < UINT32_MAX) {
        ++trace->dropped;
    }
    return 1;
}

size_t sambu_trace_snapshot(const SambuTrace *trace, SambuTraceEvent *out,
                            size_t capacity)
{
    size_t count;
    size_t oldest;
    size_t i;
    if (trace == NULL || out == NULL || capacity == 0u) return 0u;

    count = trace->count < capacity ? trace->count : capacity;
    oldest = (trace->head + SAMBU_TRACE_CAPACITY - trace->count) %
             SAMBU_TRACE_CAPACITY;
    for (i = 0u; i < count; ++i)
        out[i] = trace->events[(oldest + i) % SAMBU_TRACE_CAPACITY];
    return count;
}
