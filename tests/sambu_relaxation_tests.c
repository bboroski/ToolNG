#include "sambu_engine.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    SambuRelaxationState state = {0};
    SambuRelaxationResult result;
    double previous_mean;
    double previous_variance;
    SambuRelaxationState snapshot;

    memset(&result, 0, sizeof(result));
    CHECK(sambu_relaxation_update(&state, 1.0, 1000u, 1000.0, &result));
    CHECK(result.mean == 1.0);
    CHECK(result.variance == 0.0);
    CHECK(result.alpha == 1.0);
    CHECK(result.samples == 1u);
    CHECK(!result.warm);

    CHECK(sambu_relaxation_update(&state, -1.0, 2000u, 1000.0, &result));
    CHECK(fabs(result.alpha - (1.0 - exp(-1.0))) < 1e-12);
    CHECK(fabs(result.mean - (1.0 - 2.0 * (1.0 - exp(-1.0)))) < 1e-12);
    CHECK(result.variance > 0.0);
    CHECK(result.residual == -2.0);
    CHECK(result.samples == 2u);
    CHECK(!result.warm);

    previous_mean = result.mean;
    previous_variance = result.variance;
    CHECK(sambu_relaxation_update(&state, 1.0, 2000u, 1000.0, &result));
    CHECK(result.alpha == 0.0);
    CHECK(result.mean == previous_mean);
    CHECK(result.variance == previous_variance);
    CHECK(result.samples == 3u);
    CHECK(!result.warm);

    CHECK(sambu_relaxation_update(&state, -1.0, 3000u, 1000.0, &result));
    CHECK(result.samples == 4u);
    CHECK(result.warm);

    /* A backwards monotonic clock starts a new observation episode. */
    CHECK(sambu_relaxation_update(&state, 0.25, 2500u, 1000.0, &result));
    CHECK(result.mean == 0.25);
    CHECK(result.variance == 0.0);
    CHECK(result.samples == 1u);
    CHECK(result.alpha == 1.0);
    CHECK(!result.warm);

    snapshot = state;
    CHECK(!sambu_relaxation_update(&state, NAN, 2600u, 1000.0, &result));
    CHECK(!sambu_relaxation_update(&state, 1.0, 2600u, 0.0, &result));
    CHECK(!sambu_relaxation_update(&state, 1.0, 2600u, INFINITY, &result));
    CHECK(!sambu_relaxation_update(&state, 1e308, 2600u, 1e-300, &result));
    CHECK(memcmp(&snapshot, &state, sizeof(state)) == 0);

    CHECK(!sambu_relaxation_update(NULL, 1.0, 1u, 1.0, &result));
    CHECK(!sambu_relaxation_update(&state, 1.0, 1u, 1.0, NULL));

    puts("SAMBU relaxation tests passed.");
    return 0;
}
