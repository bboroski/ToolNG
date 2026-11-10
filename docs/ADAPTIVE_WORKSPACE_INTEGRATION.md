# Adaptive Workspace Integration

> Repository: **ToolNG**  
> Status: integration contract / design proposal. A capability is not considered implemented merely because its interface is described.

## Project responsibility

ToolNG owns evidence-oriented Windows diagnostic surfaces. Add operator profiles, remembered panel geometry, calm-density mode, contextual accent and a privacy-safe report preview. Preserve read-only discovery: the UI may recommend a repair but must never silently modify PnP devices, services, drivers or backups. Sensitive identifiers remain redactable before export.

## Shared experience contract

- **Duck Black / Yellow remains canonical.** Graphite surfaces and yellow primary actions stay intact; project colors are secondary contextual markers and never replace labels, icons, or semantic status.
- **Living materials are progressive enhancement.** Glass, metal and paper effects require opaque fallbacks, GPU budgets, contrast checks and a reduced-motion path. CSS blur is not physical refraction.
- **Typography and density adapt without guessing about health.** Fluid sizing, readable long-form measure and comfortable/calm/compact modes are user-overridable. Interaction patterns may offer a calm mode; no emotion or medical state is inferred.
- **Context is versioned and minimal.** A workspace record may include project ID, window geometry, open document references, caret position, focus target, density and a snapshot reference. It must exclude credentials, private fields, privileged tokens and unapproved content.
- **Undo and time travel are transactional.** Preview the proposed restore, validate versions and permissions, commit atomically where possible, and report partial failure. UI motion must never imply that a backend operation succeeded before it does.
- **Sensors and automation are opt-in.** Camera, gaze, presence, audio and cross-device transfer are off until explicitly enabled. Local-first processing, scoped permissions, an audit trail and a manual bypass are required.
- **Agents are bounded.** Each agent receives named capabilities, a resource/time budget, cancellation and an inspectable log. Network access and destructive actions require explicit approval.

## Event envelope (proposed, versioned)

```json
{
  "schema_version": "1.0",
  "event_id": "stable-event-id",
  "source": "ToolNG",
  "sequence": 12,
  "monotonic_ms": 482201,
  "type": "workspace.focus.changed",
  "payload": {
    "workspace_id": "workspace-01",
    "focus_id": "editor-main",
    "density": "comfortable"
  }
}
```

Consumers must reject unsupported schema versions, tolerate unknown optional fields, preserve sequence ordering where required and distinguish `observed`, `unknown`, `failed` and `confirmed`. Never put secrets or raw biometric samples in this envelope.

## Capability map

| Capability family | Integration rule |
|---|---|
| Material UI, adaptive type, chromatic context, cursor intent | Use `design/duck-ui.tokens.json` and `assets/duck-ui/duck-ui.css`; keep a usable static fallback |
| Window memory, session capsules, moment timeline, global undo | Store only explicit safe checkpoints; preview before restore |
| Semantic mesh, context clipboard, cross-device transfer | Permission-scoped, provenance-aware, authenticated and revocable |
| Attention scheduler and adaptive density | Use foreground/focus and explicit priorities first; sensors remain optional |
| Sandbox, privacy shutter, attention firewall | Least privilege; no silent access expansion; critical warnings are never hidden |
| Voice/intent rewrite and micro-agents | Preserve source, preview diffs, require confirmation before applying or executing |

## Delivery gates

1. **UI gate:** keyboard parity, touch targets, high contrast, responsive breakpoints, reduced-motion and opaque fallback are tested.
2. **State gate:** schema versioning, corrupt/stale snapshot handling, bounded storage, retention and rollback tests pass.
3. **Security gate:** permission denial, revocation, sandbox escape attempts, secret redaction and audit-log tests pass.
4. **Platform gate:** native sensors, scheduling, cross-device transfer and hardware features are marked supported only after target-specific evidence.
5. **Release gate:** maintainers report each capability as `implemented`, `experimental`, `specified`, `hardware_dependent` or `unavailable`.

## Обмеження та безпека

ToolNG відповідає за діагностичні інтерфейси Windows, засновані на доказах. Додати профілі оператора, запам'ятовування геометрії панелей, спокійну щільність, контекстний акцент і безпечний попередній перегляд звітів. Зберегти read-only модель: інтерфейс може рекомендувати виправлення, але не повинен непомітно змінювати PnP-пристрої, служби, драйвери чи резервні копії. Чутливі ідентифікатори мають редагуватися до експорту.

Ефекти интерфейса — не доказ физических возможностей оборудования. Не выполнять удаление, восстановление, запуск кода, сетевые запросы или экспорт данных скрытно. Автоматические подсказки должны быть объяснимыми, отменяемыми и отключаемыми.

## Український підсумок

ToolNG відповідає за діагностичні інтерфейси Windows, засновані на доказах. Додати профілі оператора, запам'ятовування геометрії панелей, спокійну щільність, контекстний акцент і безпечний попередній перегляд звітів. Зберегти read-only модель: інтерфейс може рекомендувати виправлення, але не повинен непомітно змінювати PnP-пристрої, служби, драйвери чи резервні копії. Чутливі ідентифікатори мають редагуватися до експорту.

Візуальні ефекти мають безпечні статичні резервні стани. Автоматизація діє лише в межах явно наданих можливостей; відновлення має попередній перегляд і контроль помилок. Біометрія та доступ до камери за замовчуванням вимкнені.

## English summary

ToolNG owns evidence-oriented Windows diagnostic surfaces. Add operator profiles, remembered panel geometry, calm-density mode, contextual accent and a privacy-safe report preview. Preserve read-only discovery: the UI may recommend a repair but must never silently modify PnP devices, services, drivers or backups. Sensitive identifiers remain redactable before export.

Adaptive workspace features are integrated as versioned, permission-scoped capabilities. Hardware-dependent behavior remains gated until it has target-specific tests and evidence. Preserve the black/yellow visual identity, accessible fallbacks, reversibility and explicit user control.
