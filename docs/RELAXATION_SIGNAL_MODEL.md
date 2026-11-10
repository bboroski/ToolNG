# ToolzNG relaxation-aware signal model

ToolzNG's existing readiness score remains a deterministic, rule-based assessment. The relaxation estimator is a separate advisory layer for noisy, timestamped observations such as repeated USB arrivals and removals.

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

The result exposes the smoothed mean, variance, innovation residual, standardized residual, effective alpha, sample count and a warm-up flag. The warm-up flag becomes true after four accepted observations. A backwards monotonic timestamp starts a fresh window; identical timestamps do not advance the filter.

## USB Pulse integration

The Win32 shell feeds Apple USB arrival events as +1 and removal events as -1 into the estimator, using a 30-second time constant and `GetTickCount64()`. The local PulseVault JSONL schema is version 2 and includes the smoothed mean, variance, standardized residual and warm-up flag for each event. Local diagnostic reports show the trend as arrival-biased, removal-biased, balanced or not enough events. These values are explicitly advisory and never change Duck Readiness, the finding code or the evidence ledger.

## Safety and interpretation

- The estimator is bounded state: it stores no device identifiers or raw event history.
- Invalid, non-finite observations and non-positive time constants are rejected.
- A clock reset restarts state instead of producing a negative elapsed interval.
- The estimator does not rewrite `SambuEvidence`, findings, the weighted readiness score or the user's system.
- The z-score is a descriptive residual scale, not a probability, fault verdict or device-health guarantee.
- The existing recurrence ledger remains the auditable counter of finding occurrences; relaxation is not a substitute for evidence.

The model provides a lightweight interpretation of relaxation and fluctuations without pretending that a USB event stream is a quantum system.
