# 🦆 Contributing to ToolzNG

1. Keep platform-specific code isolated behind Windows APIs.
2. Prefer read-only diagnostics. Any future write operation must be explicit, reversible where possible, and preceded by a clear confirmation.
3. Never add passcode bypass, activation-lock bypass, jailbreak, firmware modification, or silent deletion features.
4. Build with MSVC warnings enabled (`/W4`) and test on a Windows machine with a real Apple device before claiming hardware support.
5. Keep README claims aligned with behavior that is actually implemented.

## Duck design

Keep the graphite-and-gold palette consistent. The duck mascot should remain decorative and must never obscure diagnostic values, warnings, or accessibility labels. Avoid fake speed claims: document exactly what an action reads or changes.


## Commit and release discipline

Use Conventional Commit subjects (`feat:`, `fix:`, `docs:`, `refactor:`, `perf:`, `test:`, `build:`, `ci:`, `chore:`) and keep the header at or below 72 characters. Breaking changes require `!:` and a `BREAKING CHANGE:` footer. PR titles and every commit in the PR are validated automatically.

Use SemVer tags (`vMAJOR.MINOR.PATCH`) and release candidates (`vMAJOR.MINOR.PATCH-rc.N`). Update `CHANGELOG.md`, test the exact candidate, verify a maintainer-signed annotated tag, and retain a rollback path before publishing. See `RELEASE_POLICY.md`.
