#ifndef TOOLNG_GUIDANCE_H
#define TOOLNG_GUIDANCE_H
#include "toolng_engine.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum ToolNGGuidanceSeverity {
    TOOLNG_GUIDANCE_INFO = 0,
    TOOLNG_GUIDANCE_NOTICE = 1,
    TOOLNG_GUIDANCE_WARNING = 2,
    TOOLNG_GUIDANCE_BLOCKER = 3
} ToolNGGuidanceSeverity;
typedef struct ToolNGGuidance {
    ToolNGGuidanceSeverity severity;
    const char *summary;
    const char *next_action;
    int requires_operator_action;
} ToolNGGuidance;
/* Deterministic, read-only guidance. It never starts services or mutates devices. */
ToolNGGuidance toolng_guidance_for_finding(ToolNGFinding finding);
#ifdef __cplusplus
}
#endif
#endif
