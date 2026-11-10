#ifndef TOOLNG_DOMAIN_H
#define TOOLNG_DOMAIN_H

#include <stdint.h>

typedef enum ToolNGDomainStatus {
    TOOLNG_DOMAIN_INVALID = -1,
    TOOLNG_DOMAIN_UNKNOWN = 0,
    TOOLNG_DOMAIN_NOMINAL = 1,
    TOOLNG_DOMAIN_WARNING = 2,
    TOOLNG_DOMAIN_CRITICAL = 3
} ToolNGDomainStatus;

typedef enum ToolNGDomainAdvice {
    TOOLNG_DOMAIN_ADVICE_NONE = 0,
    TOOLNG_DOMAIN_ADVICE_REPEAT_READ = 1,
    TOOLNG_DOMAIN_ADVICE_OPERATOR_REVIEW = 2,
    TOOLNG_DOMAIN_ADVICE_STOP_DIAGNOSTIC = 3
} ToolNGDomainAdvice;

typedef struct ToolNGDomainReading {
    double value;
    double minimum;
    double maximum;
    double warning_margin;
    const char *unit;
    uint32_t sample_count;
    uint32_t required_samples;
    int sensor_valid;
    int calibration_valid;
} ToolNGDomainReading;

typedef struct ToolNGDomainAssessment {
    ToolNGDomainStatus status;
    ToolNGDomainAdvice advice;
    uint32_t flags;
} ToolNGDomainAssessment;

enum {
    TOOLNG_DOMAIN_FLAG_SENSOR_INVALID = 1u << 0,
    TOOLNG_DOMAIN_FLAG_UNCALIBRATED = 1u << 1,
    TOOLNG_DOMAIN_FLAG_INSUFFICIENT_SAMPLES = 1u << 2,
    TOOLNG_DOMAIN_FLAG_OUT_OF_RANGE = 1u << 3,
    TOOLNG_DOMAIN_FLAG_NEAR_LIMIT = 1u << 4
};

/* Read-only classification. The module never changes a device or its settings. */
ToolNGDomainStatus toolng_domain_assess(
    const ToolNGDomainReading *reading,
    ToolNGDomainAssessment *assessment);

#endif
