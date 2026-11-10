# ToolNG relaxation-aware signal model

ToolNG's readiness score remains a deterministic assessment of completed Windows-side checks. The relaxation estimator is a separate advisory layer for timestamped USB arrival/removal observations.

## Update rule

For observation (x_t), previous estimate (mu_{t-1}), elapsed monotonic time (Delta t), and positive time constant (	au):

\[
\alpha = 1 - e^{-\Delta t / \tau}
\]

\[
\mu_t = \mu_{t-1} + \alpha(x_t-\mu_{t-1})
\]

The variance estimate is updated as:

\[
v_t = (1-\alpha)\left(v_{t-1}+\alpha(x_t-\mu_{t-1})^2\right)
\]

A backwards clock jump starts a fresh observation episode; identical timestamps do not advance the filter. After four accepted observations, the result is marked warm.

## USB Pulse integration

The Win32 shell feeds Apple USB arrival events as +1 and removal events as -1, using `GetTickCount64()` and a 30-second time constant. The local PulseVault JSONL schema is version 2 and includes the smoothed mean, variance, standardized residual and warm-up flag for each event. Local diagnostic reports show the trend as arrival-biased, removal-biased, balanced or not enough events.

## Safety and interpretation

- State is bounded: no device identifiers or raw event history are stored by the estimator.
- Invalid and non-finite observations, non-finite time constants and non-positive time constants are rejected.
- The update is transactional: invalid derived results do not partially mutate filter state.
- The estimator does not rewrite `SambuEvidence`, findings, the readiness score or the user's system.
- The standardized residual is descriptive, not a probability, fault verdict or device-health guarantee.
- The existing recurrence ledger remains the auditable counter of finding occurrences; relaxation is not a substitute for evidence.

The implementation is an ordinary finite-state signal filter, not a quantum simulation. The analogy with relaxation and fluctuations is used as a design model only.
