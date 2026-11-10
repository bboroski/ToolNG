#include "toolng_domain.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

static int toolng_finite(double value)
{
    return isfinite(value) != 0;
}

ToolNGDomainStatus toolng_domain_assess(
    const ToolNGDomainReading *reading,
    ToolNGDomainAssessment *assessment)
{
    double lower_margin;
    double upper_margin;

    if (reading == NULL || assessment == NULL ||
        reading->unit == NULL || reading->unit[0] == '\0' ||
        reading->required_samples == 0u ||
        !toolng_finite(reading->minimum) ||
        !toolng_finite(reading->maximum) ||
        !toolng_finite(reading->warning_margin) ||
        reading->minimum > reading->maximum ||
        reading->warning_margin < 0.0) {
        return TOOLNG_DOMAIN_INVALID;
    }

    assessment->status = TOOLNG_DOMAIN_UNKNOWN;
    assessment->advice = TOOLNG_DOMAIN_ADVICE_REPEAT_READ;
    assessment->flags = 0u;

    if (!toolng_finite(reading->value)) {
        assessment->flags |= TOOLNG_DOMAIN_FLAG_SENSOR_INVALID;
    }
    if (!reading->sensor_valid) {
        assessment->flags |= TOOLNG_DOMAIN_FLAG_SENSOR_INVALID;
    }
    if (!reading->calibration_valid) {
        assessment->flags |= TOOLNG_DOMAIN_FLAG_UNCALIBRATED;
    }
    if (reading->sample_count < reading->required_samples) {
        assessment->flags |= TOOLNG_DOMAIN_FLAG_INSUFFICIENT_SAMPLES;
    }
    if (assessment->flags != 0u) {
        return assessment->status;
    }

    if (reading->value < reading->minimum ||
        reading->value > reading->maximum) {
        assessment->status = TOOLNG_DOMAIN_CRITICAL;
        assessment->advice = TOOLNG_DOMAIN_ADVICE_STOP_DIAGNOSTIC;
        assessment->flags = TOOLNG_DOMAIN_FLAG_OUT_OF_RANGE;
        return assessment->status;
    }

    lower_margin = reading->value - reading->minimum;
    upper_margin = reading->maximum - reading->value;
    if (lower_margin <= reading->warning_margin ||
        upper_margin <= reading->warning_margin) {
        assessment->status = TOOLNG_DOMAIN_WARNING;
        assessment->advice = TOOLNG_DOMAIN_ADVICE_OPERATOR_REVIEW;
        assessment->flags = TOOLNG_DOMAIN_FLAG_NEAR_LIMIT;
        return assessment->status;
    }

    assessment->status = TOOLNG_DOMAIN_NOMINAL;
    assessment->advice = TOOLNG_DOMAIN_ADVICE_NONE;
    return assessment->status;
}
