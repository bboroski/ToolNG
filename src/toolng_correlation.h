#ifndef TOOLNG_CORRELATION_H
#define TOOLNG_CORRELATION_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define TOOLNG_CORRELATION_MAX_SIGNALS 8u
#define TOOLNG_CORRELATION_MAX_OBSERVATIONS 64u
#define TOOLNG_CORRELATION_SCALE 1000u

/* Only presence bits and known bits are retained; raw device payloads are not. */
typedef struct ToolNGBinaryObservation {
    uint32_t known_mask;
    uint32_t present_mask;
} ToolNGBinaryObservation;

typedef struct ToolNGCorrelationMatrix {
    uint16_t similarity_milli[TOOLNG_CORRELATION_MAX_SIGNALS]
                             [TOOLNG_CORRELATION_MAX_SIGNALS];
    uint16_t comparisons[TOOLNG_CORRELATION_MAX_SIGNALS]
                        [TOOLNG_CORRELATION_MAX_SIGNALS];
    uint16_t co_occurrences[TOOLNG_CORRELATION_MAX_SIGNALS]
                           [TOOLNG_CORRELATION_MAX_SIGNALS];
} ToolNGCorrelationMatrix;

/*
 * Compute pairwise Jaccard co-occurrence scores over a bounded local window.
 * A bit is compared only when both observations mark it as known. This is a
 * diagnostic similarity measure, not causal inference or a readiness score.
 */
static inline int toolng_correlate_observations(
    const ToolNGBinaryObservation *observations,
    size_t count,
    ToolNGCorrelationMatrix *result)
{
    ToolNGCorrelationMatrix computed;
    size_t i, j, sample;
    const uint32_t allowed = (UINT32_C(1) << TOOLNG_CORRELATION_MAX_SIGNALS) - 1u;

    if (observations == NULL || result == NULL || count == 0u ||
        count > TOOLNG_CORRELATION_MAX_OBSERVATIONS) {
        return 0;
    }
    memset(&computed, 0, sizeof(computed));
    for (sample = 0u; sample < count; ++sample) {
        if ((observations[sample].known_mask & ~allowed) != 0u ||
            (observations[sample].present_mask & ~observations[sample].known_mask) != 0u) {
            return 0;
        }
    }

    for (i = 0u; i < TOOLNG_CORRELATION_MAX_SIGNALS; ++i) {
        for (j = i; j < TOOLNG_CORRELATION_MAX_SIGNALS; ++j) {
            uint32_t comparable = 0u;
            uint32_t both = 0u;
            uint32_t either = 0u;
            for (sample = 0u; sample < count; ++sample) {
                const uint32_t pair = (UINT32_C(1) << i) | (UINT32_C(1) << j);
                if ((observations[sample].known_mask & pair) != pair) {
                    continue;
                }
                ++comparable;
                if ((observations[sample].present_mask & pair) == pair) {
                    ++both;
                }
                if ((observations[sample].present_mask & pair) != 0u) {
                    ++either;
                }
            }
            computed.comparisons[i][j] = (uint16_t)comparable;
            computed.comparisons[j][i] = (uint16_t)comparable;
            computed.co_occurrences[i][j] = (uint16_t)both;
            computed.co_occurrences[j][i] = (uint16_t)both;
            if (either != 0u) {
                const uint16_t score = (uint16_t)((both * TOOLNG_CORRELATION_SCALE) / either);
                computed.similarity_milli[i][j] = score;
                computed.similarity_milli[j][i] = score;
            }
        }
    }
    *result = computed;
    return 1;
}

#endif
