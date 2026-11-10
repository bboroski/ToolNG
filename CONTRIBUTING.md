# 🦆 Contributing to ToolzNG

1. Keep platform-specific code isolated behind Windows APIs.
2. Prefer read-only diagnostics. Any future write operation must be explicit, reversible where possible, and preceded by a clear confirmation.
3. Never add passcode bypass, activation-lock bypass, jailbreak, firmware modification, or silent deletion features.
4. Build with MSVC warnings enabled (`/W4`) and test on a Windows machine with a real Apple device before claiming hardware support.
5. Keep README claims aligned with behavior that is actually implemented.

## Duck design

Keep the graphite-and-gold palette consistent. The duck mascot should remain decorative and must never obscure diagnostic values, warnings, or accessibility labels. Avoid fake speed claims: document exactly what an action reads or changes.
