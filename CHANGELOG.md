# Changelog

## SAMBU Core 1.4.0 — NUN State Reliability

- Added an explicit validator for the bounded 32-entry finding ledger, including cursor, counter, timestamp and duplicate-code invariants.
- Made malformed ledger reads return neutral results and malformed writes fail closed instead of risking out-of-bounds access.
- Hardened the USB exponential-relaxation lifecycle against invalid initialization markers and initialized states with no samples.
- Added regression coverage for malformed ledger metadata and invalid relaxation states.
- Documented the NUN/Naunet metaphor as a practical model for bounded resources, evidence/context separation, temporal recurrence and controlled workflow transitions; no mythological or pseudoscientific behavior is implemented.
- Preserved the diagnostic report schema, scoring weights, device-mutation prohibition and network-upload prohibition.

## 0.6.1 — SAMBU Workflow Runtime

- Added a bounded C17 workflow lexer/parser and source-aware AST with task manifests and explicit autonomy classes.
- Restricted executable actions to five allowlisted Apple diagnostic host operations; arbitrary commands, network actions and device mutation are not expressible.
- Added a dual preflight/capability gate for local LinkDNA profile recording.
- Added one-action-per-Win32-timer cooperative dispatch and per-step evidence in local TXT reports.
- Split the portable SAMBU core from the Win32 desktop target so CTest can run on non-Windows hosts.
- Added parser/runtime tests and CI execution of the CTest suite.
- Updated SAMBU integration, build and scope documentation to distinguish implemented, limited and planned capabilities.

## 0.5.0 — Duck Desktop Experience

- Added per-monitor DPI-awareness v2 and retained user zoom (80–160%) with A−/A+ controls and Ctrl +/-/0 shortcuts.
- Replaced blue UI accents with the black/graphite/yellow design tokens.
- Added local private-by-default TXT report generation, an in-app history list, scrollable preview, clipboard copy, optional TXT Save As export, and SHA-256 sidecars.
- Moved DuckFix route output into the application-local Reports directory.
- Added native offline Help navigation and F1 shortcut; removed routine Apple support/store browser launch buttons.
- Kept legacy LinkDNA and PulseVault files in place; new display settings use a separate settings.ini file.


## 0.4.0 — Duck Intelligence Plus

- Added LinkDNA local baseline signature to detect changes in the Windows-visible Apple USB/service profile. The FNV-1a digest is a change detector, not a security identity.
- Added PulseVault JSONL persistence with bounded 256 KiB rotation.
- Added Backup Freshness Radar based on the newest backup Info.plist last-write timestamp.
- Added exportable DuckFix Route checklist.
- Added SHA-256 sidecar for generated diagnostic reports (Evidence Seal).
- Expanded privacy and measurement-boundary documentation.

## 0.3.0 — Duck Intelligence Edition

- Added transparent Duck Readiness Score based only on observable Windows-side connection signals.
- Added USB Pulse event monitoring for Apple USB interface arrival/removal notifications.
- Added read-only Backup Sentinel checking expected backup marker files (`Info.plist` and `Manifest.db`/`Manifest.plist`).
- Added Privacy Shield with a private-by-default report choice, technical-detail mode, and save preview.
- Added DuckFix context-sensitive next-step guidance.
- Updated the interface and documentation to state measurement limits and avoid unsupported iOS speed claims.

## 0.2.0 — Duck Edition

- Added a custom GDI-rendered duck mascot to the native Win32 interface.
- Switched the UI to a graphite-and-gold visual system.
- Added a duck-themed SVG README banner and illustrations.

## 0.1.0 — Initial MVP

- Added native Win32/GDI interface written in C17.
- Added Apple USB PnP detection using `VID_05AC`.
- Added Apple Mobile Device Service status inspection.
- Added local backup folder discovery and UTF-8 diagnostic reports.
- Added CMake and Windows GitHub Actions build workflow.
Все заметные изменения ToolzNG собраны здесь. Формат — хронологический и ориентирован на пользовательские изменения.

## [0.6.1] — 2026-11-10

### Added
- Видимая строка SAMBU Quality Gate на главной панели и в разделе USB-устройств.
- Отображение точного покрытия backup-маркеров: проверенные папки / просмотренные папки.
- Горячая клавиша F5 для повторного USB-сканирования.
- Горячая клавиша Ctrl+R для создания локального диагностического отчёта.
- Подсказки по новым сочетаниям клавиш во встроенной справке.
- Обновлённые SVG UI-превью для обзора, USB-устройств и локальных отчётов.

### Improved
- Карточка оптимизации переименована в «Рекомендации iOS», чтобы не подразумевать несуществующий тест скорости.
- Версия ToolzNG 0.6.1 отображается в заголовке и системных сведениях программы.
- README перестроен как продуктовая страница с быстрым обзором, галереей, безопасностью, сборкой и roadmap.
- README и UI-превью синхронизированы с SAMBU Engine 1.2.0 и Quality Gate.

### Fixed
- Устаревшие макеты отчётов с SAMBU Engine 1.1.0 заменены актуальными демонстрационными превью с readiness и quality flags.
- Уточнена маркировка демонстрационных значений: изображения README не являются реальными снимками работающего процесса.

## [0.6.0] — 2026-11-10

### Added
- SAMBU Quality Gate для согласованности диагностических доказательств.
- Точная проверка полноты backup-маркеров по всем обнаруженным папкам.
- Целостность входных сигналов и quality flags в диагностических отчётах.
- Усиленный CI с Linux, macOS, Windows и sanitizer-проверками.
- Duck Design Rules как норматив интерфейса, архитектуры, приватности и качества.
