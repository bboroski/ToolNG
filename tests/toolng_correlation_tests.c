#include "toolng_correlation.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    const ToolNGBinaryObservation samples[] = {
        {0x07u, 0x03u},
        {0x07u, 0x01u},
        {0x07u, 0x03u},
        {0x05u, 0x04u} /* signal 1 is unknown in this observation */
    };
    ToolNGCorrelationMatrix result = {{{0}}};

    assert(toolng_correlate_observations(samples, 4u, &result));
    assert(result.comparisons[0][1] == 3u);
    assert(result.co_occurrences[0][1] == 2u);
    assert(result.similarity_milli[0][1] == 666u);
    assert(result.similarity_milli[0][1] == result.similarity_milli[1][0]);
    assert(result.comparisons[0][2] == result.comparisons[2][0]);

    {
        const ToolNGBinaryObservation invalid[] = {{0x01u, 0x02u}};
        ToolNGCorrelationMatrix unchanged = {{{123u}}};
        assert(!toolng_correlate_observations(invalid, 1u, &unchanged));
        assert(unchanged.similarity_milli[0][0] == 123u);
    }
    assert(!toolng_correlate_observations(samples, 0u, &result));
    assert(!toolng_correlate_observations(samples, 65u, &result));
    assert(!toolng_correlate_observations(NULL, 1u, &result));
    assert(!toolng_correlate_observations(samples, 1u, NULL));

    puts("ToolNG correlation tests: PASS");
    return 0;
}
