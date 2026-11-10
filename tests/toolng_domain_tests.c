#include "toolng_domain.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

int main(void)
{
    ToolNGDomainReading reading = {
        50.0, 0.0, 100.0, 5.0, "unit", 4u, 4u, 1, 1
    };
    ToolNGDomainAssessment result;

    assert(toolng_domain_assess(&reading, &result) == TOOLNG_DOMAIN_NOMINAL);
    assert(result.advice == TOOLNG_DOMAIN_ADVICE_NONE);

    reading.value = 98.0;
    assert(toolng_domain_assess(&reading, &result) == TOOLNG_DOMAIN_WARNING);
    assert(result.advice == TOOLNG_DOMAIN_ADVICE_OPERATOR_REVIEW);

    reading.value = 101.0;
    assert(toolng_domain_assess(&reading, &result) == TOOLNG_DOMAIN_CRITICAL);
    assert(result.advice == TOOLNG_DOMAIN_ADVICE_STOP_DIAGNOSTIC);

    reading.value = 50.0;
    reading.calibration_valid = 0;
    assert(toolng_domain_assess(&reading, &result) == TOOLNG_DOMAIN_UNKNOWN);
    assert((result.flags & TOOLNG_DOMAIN_FLAG_UNCALIBRATED) != 0u);

    reading.calibration_valid = 1;
    reading.sample_count = 1u;
    assert(toolng_domain_assess(&reading, &result) == TOOLNG_DOMAIN_UNKNOWN);
    assert((result.flags & TOOLNG_DOMAIN_FLAG_INSUFFICIENT_SAMPLES) != 0u);

    reading.sample_count = 4u;
    reading.value = NAN;
    assert(toolng_domain_assess(&reading, &result) == TOOLNG_DOMAIN_UNKNOWN);
    assert((result.flags & TOOLNG_DOMAIN_FLAG_SENSOR_INVALID) != 0u);

    reading.value = 50.0;
    reading.minimum = 120.0;
    reading.maximum = 100.0;
    assert(toolng_domain_assess(&reading, &result) == TOOLNG_DOMAIN_INVALID);
    assert(toolng_domain_assess(NULL, &result) == TOOLNG_DOMAIN_INVALID);
    assert(toolng_domain_assess(&reading, NULL) == TOOLNG_DOMAIN_INVALID);

    puts("ToolNG domain tests passed");
    return 0;
}
