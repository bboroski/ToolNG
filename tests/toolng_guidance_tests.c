#include "toolng_guidance.h"
#include <assert.h>
#include <string.h>
int main(void)
{
    ToolNGGuidance g = toolng_guidance_for_finding(TOOLNG_FINDING_HEALTHY);
    assert(g.severity == TOOLNG_GUIDANCE_INFO);
    assert(g.requires_operator_action == 0);
    assert(strcmp(g.next_action, "no_action_required") == 0);
    g = toolng_guidance_for_finding(TOOLNG_FINDING_SCAN_UNAVAILABLE);
    assert(g.severity == TOOLNG_GUIDANCE_BLOCKER);
    assert(g.requires_operator_action == 1);
    assert(strcmp(g.next_action, "check_permissions_and_retry_scan") == 0);
    g = toolng_guidance_for_finding(TOOLNG_FINDING_BACKUP_UNKNOWN);
    assert(g.severity == TOOLNG_GUIDANCE_NOTICE);
    assert(strcmp(g.next_action, "complete_backup_scan_before_assessing_backup") == 0);
    g = toolng_guidance_for_finding((ToolNGFinding)999);
    assert(strcmp(g.summary, "finding_unknown") == 0);
    return 0;
}
