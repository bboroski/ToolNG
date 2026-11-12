# ToolNG
**Windows Device Evidence Probe**

[Українська](README.md) · [English](README.en.md) · [Русский](README.ru.md)

A local, read-only Windows diagnostic utility that separates confirmed device signals from unknown state and incomplete evidence coverage.

## Features
- Observe Windows PnP/USB devices and related services.
- Read-only inspection of local backup markers and connection events.
- Local reports, logs, exports and sensitive-field redaction.
- Explainable readiness and evidence-coverage reporting.

## Run and test
```bash
cmake -S . -B build
cmake --build build --config Release
python one.py check
```

## Screenshots

No verified capture of the running application is tracked. Vector previews containing SAMPLE DATA are not screenshots.

## Scope
ToolNG does not repair devices, modify drivers or services, inspect private backup contents or upload diagnostics. A recommendation is not an executed action.

## Project documentation



*Static previews with SAMPLE DATA, not live scan results.*

## Related
[Build guide](docs/BUILDING.md) · [safety and scope](docs/SAFETY_AND_SCOPE.md)

[Hellduck](https://github.com/bboroski/Hellduck) · [NUN](https://github.com/bboroski/NUN)

## License
MIT

---
**执子之手，与子偕老。**
*To hold your hand and grow old together.*
— 《诗经·邶风·击鼓》
