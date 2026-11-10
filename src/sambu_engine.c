#include "sambu_engine.h"

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
    int score = 0;

    memset(&result, 0, sizeof(result));
    result.finding = SAMBU_FINDING_SCAN_UNAVAILABLE;
    result.confidence_percent = e->scan_complete
        ? (e->usb_watch_registered ? 100u : 85u) : 40u;

    if (e->scan_complete) {
        result.evidence_mask |= SAMBU_EVIDENCE_SCAN_COMPLETE;
        if (e->device_found) {
            result.evidence_mask |= SAMBU_EVIDENCE_DEVICE_VISIBLE;
            score += 40;
        }
    }
    if (e->service_running) {
        result.evidence_mask |= SAMBU_EVIDENCE_SERVICE_RUNNING;
        score += 25;
    }
    if (e->backup_found) {
        result.evidence_mask |= SAMBU_EVIDENCE_BACKUP_FOUND;
        score += 10;
    }
    if (e->backup_marker_valid) {
        result.evidence_mask |= SAMBU_EVIDENCE_BACKUP_MARKERS;
        score += 15;
    }
    if (e->usb_watch_registered)
        result.evidence_mask |= SAMBU_EVIDENCE_USB_WATCH;
    if (e->recent_usb_removals > 0)
        result.evidence_mask |= SAMBU_EVIDENCE_USB_REMOVAL;
    if (e->stability_window_elapsed)
        result.evidence_mask |= SAMBU_EVIDENCE_STABILITY_WINDOW;

    if (e->scan_complete && e->usb_watch_registered && e->device_found &&
        e->stability_window_elapsed && e->recent_usb_removals == 0) {
        score += 10;
    }

    if (score > 100) score = 100;
    result.readiness_score = score;

    /* The first matching condition is the highest-priority actionable finding. */
    if (!e->scan_complete) {
        result.finding = SAMBU_FINDING_SCAN_UNAVAILABLE;
    } else if (!e->device_found) {
        result.finding = SAMBU_FINDING_NO_DEVICE;
    } else if (!e->service_running) {
        result.finding = SAMBU_FINDING_SERVICE_UNAVAILABLE;
    } else if (e->recent_usb_removals > 0) {
        result.finding = SAMBU_FINDING_USB_UNSTABLE;
    } else if (e->backup_folders_seen > 0 && !e->backup_marker_valid) {
        result.finding = SAMBU_FINDING_BACKUP_STRUCTURE;
    } else if (e->backup_marker_valid && e->backup_age_known &&
               e->backup_age_days > 30u) {
        result.finding = SAMBU_FINDING_BACKUP_STALE;
    } else if (!e->backup_found || !e->backup_marker_valid) {
        result.finding = SAMBU_FINDING_BACKUP_MISSING;
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

uint32_t sambu_ledger_unique_count(const SambuLedger *ledger)
{
    if (!ledger) return 0u;
    return (uint32_t)ledger->count;
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
        case SAMBU_FINDING_NONE:
        default: return "none";
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
