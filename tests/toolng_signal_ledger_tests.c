#include "toolng_signal_ledger.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

int main(void)
{
    toolng_signal_ledger_t ledger;
    toolng_signal_event_t snapshot[TOOLNG_LEDGER_CAPACITY];
    size_t i, count;
    toolng_signal_ledger_init(&ledger);
    assert(ledger.count == 0u);
    assert(toolng_signal_ledger_append(&ledger, 10u, 1u, 7u, 1u, 900u,
        TOOLNG_LEDGER_FLAG_OBSERVED, 0u) == 1);
    assert(toolng_signal_ledger_append(&ledger, 11u, 2u, 7u, 2u, 800u,
        TOOLNG_LEDGER_FLAG_OBSERVED | TOOLNG_LEDGER_FLAG_CORROBORATED, 1u) == 1);
    assert(toolng_signal_ledger_append(&ledger, 9u, 3u, 7u, 1u, 500u,
        TOOLNG_LEDGER_FLAG_OBSERVED, 0u) == 0); /* monotonic time regression */
    assert(toolng_signal_ledger_append(&ledger, 12u, 3u, 7u, 1u, 1001u,
        TOOLNG_LEDGER_FLAG_OBSERVED, 0u) == 0); /* invalid confidence */
    assert(toolng_signal_ledger_append(&ledger, 12u, 3u, 7u, 1u, 500u,
        TOOLNG_LEDGER_FLAG_CORROBORATED, 0u) == 0); /* corroboration needs observation */
    assert(toolng_signal_ledger_append(&ledger, 12u, 3u, 7u, 1u, 500u,
        TOOLNG_LEDGER_FLAG_OBSERVED, ledger.next_sequence) == 0); /* future reference */
    assert(toolng_signal_ledger_snapshot(&ledger, NULL, 0u) == 2u);
    count = toolng_signal_ledger_snapshot(&ledger, snapshot, TOOLNG_LEDGER_CAPACITY);
    assert(count == 2u);
    assert(snapshot[0].sequence == 1u && snapshot[1].sequence == 2u);
    assert(snapshot[1].related_sequence == 1u);

    for (i = 0u; i < TOOLNG_LEDGER_CAPACITY + 3u; ++i) {
        assert(toolng_signal_ledger_append(&ledger, 20u + (uint64_t)i, 10u + (uint32_t)i,
            7u, 0u, 1000u, TOOLNG_LEDGER_FLAG_OBSERVED, 0u) == 1);
    }
    assert(ledger.count == TOOLNG_LEDGER_CAPACITY);
    assert(ledger.dropped == 5u);
    count = toolng_signal_ledger_snapshot(&ledger, snapshot, TOOLNG_LEDGER_CAPACITY);
    assert(count == TOOLNG_LEDGER_CAPACITY);
    assert(snapshot[0].sequence == 6u);
    puts("bounded event history tests passed");
    return 0;
}
