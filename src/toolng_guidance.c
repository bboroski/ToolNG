#include "toolng_guidance.h"

ToolNGGuidance toolng_guidance_for_finding(ToolNGFinding finding)
{
    ToolNGGuidance result = { TOOLNG_GUIDANCE_NOTICE, "finding_unknown",
                              "review_diagnostic_evidence", 1 };
    switch (finding) {
    case TOOLNG_FINDING_NONE:
        result.severity = TOOLNG_GUIDANCE_INFO;
        result.summary = "no_finding_recorded";
        result.next_action = "run_a_local_scan";
        break;
    case TOOLNG_FINDING_SCAN_UNAVAILABLE:
        result.severity = TOOLNG_GUIDANCE_BLOCKER;
        result.summary = "scan_not_completed";
        result.next_action = "check_permissions_and_retry_scan";
        break;
    case TOOLNG_FINDING_NO_DEVICE:
        result.severity = TOOLNG_GUIDANCE_WARNING;
        result.summary = "device_not_visible";
        result.next_action = "check_cable_port_and_device_unlock_state";
        break;
    case TOOLNG_FINDING_SERVICE_UNAVAILABLE:
        result.severity = TOOLNG_GUIDANCE_WARNING;
        result.summary = "required_service_not_running";
        result.next_action = "review_service_status_and_start_it_manually";
        break;
    case TOOLNG_FINDING_USB_UNSTABLE:
        result.severity = TOOLNG_GUIDANCE_WARNING;
        result.summary = "usb_connection_unstable";
        result.next_action = "stabilize_connection_and_repeat_stability_check";
        break;
    case TOOLNG_FINDING_BACKUP_STRUCTURE:
        result.severity = TOOLNG_GUIDANCE_WARNING;
        result.summary = "backup_structure_incomplete";
        result.next_action = "review_backup_markers_and_folder_coverage";
        break;
    case TOOLNG_FINDING_BACKUP_STALE:
        result.severity = TOOLNG_GUIDANCE_NOTICE;
        result.summary = "backup_is_older_than_policy_window";
        result.next_action = "create_and_verify_a_fresh_backup";
        break;
    case TOOLNG_FINDING_BACKUP_MISSING:
        result.severity = TOOLNG_GUIDANCE_WARNING;
        result.summary = "backup_not_found";
        result.next_action = "create_a_backup_and_run_a_new_local_check";
        break;
    case TOOLNG_FINDING_HEALTHY:
        result.severity = TOOLNG_GUIDANCE_INFO;
        result.summary = "all_checked_signals_are_healthy";
        result.next_action = "no_action_required";
        result.requires_operator_action = 0;
        break;
    case TOOLNG_FINDING_SERVICE_UNKNOWN:
        result.severity = TOOLNG_GUIDANCE_NOTICE;
        result.summary = "service_state_is_unknown";
        result.next_action = "repeat_service_query_before_drawing_a_conclusion";
        break;
    case TOOLNG_FINDING_BACKUP_UNKNOWN:
        result.severity = TOOLNG_GUIDANCE_NOTICE;
        result.summary = "backup_state_is_unknown";
        result.next_action = "complete_backup_scan_before_assessing_backup";
        break;
    case TOOLNG_FINDING_STABILITY_UNCONFIRMED:
        result.severity = TOOLNG_GUIDANCE_NOTICE;
        result.summary = "usb_stability_window_not_confirmed";
        result.next_action = "keep_device_connected_until_stability_window_completes";
        break;
    default:
        break;
    }
    return result;
}
