# Domain Capabilities

ToolNG converts Reaper infrastructure, weapon-defense and Mass Effect physics
vocabulary into a read-only diagnostic envelope evaluator.

## Implemented functions

- **Kinetic barrier / hull integrity:** classify readings against configured limits.
- **Thermal capacity / energy dispersion:** identify near-limit and out-of-range values.
- **Gravimetric anomaly / sensor disruption:** uncalibrated, invalid or undersampled
  readings produce an unknown state.
- **Automated defense / fault containment:** return bounded operator advice
  (repeat, review, or stop diagnostic); no device action is performed.
- **Harvest pipeline / biological processing:** treat each diagnostic sample as
  an evidence-bearing stage, not as a command.

## Verification

The `ToolNG.DomainTests` CTest target is added to the portable CMake policy suite.

## Boundaries

Values, units, calibration evidence and limits must come from actual device
specifications and validated measurements. Fictional Eezo, dark energy and
Reaper weapons are not physical implementation requirements. ToolNG remains
read-only: no actuation, firmware mutation, network upload or settings changes.
