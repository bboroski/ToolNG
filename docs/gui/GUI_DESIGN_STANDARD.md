# GUI / Visual System Standard

> Shared contract for application surfaces. This document is a design specification, not evidence that every repository ships a GUI.

## Visual identity

| Token | Value | Use |
|---|---|---|
| Primary yellow | `#FFCC00` | Main action, selection and key emphasis |
| Highlight yellow | `#FFE600` | Hover, active progress and focus |
| Deep black / graphite | `#000000` / `#1A1A1A` | Structure, panels and high-contrast outlines |
| Alert red | `#E53935` | Errors and critical states only |
| Signal cyan | `#29B6F6` | Live activity and data-flow telemetry |
| White | `#FFFFFF` | Text and high-contrast details |

Existing Duck tokens, stylesheet and logo remain canonical where already implemented. Do not fork them silently; a token migration must update tokens, CSS, previews, GUI code and quality checks together.

## Reference workspace

- Header: app mark; measured system status/progress/activity; local time `HH:MM`.
- Main area: task-specific data canvas, object list and optional annotation surface.
- Control area: contextual actions and secondary controls, separated from data.
- Footer/pager: only for applications with multiple views.

`55 / ~~~ / 21:33` are wireframe placeholders, never production telemetry. Show real values, units, source and stale/unknown states; never animate fabricated progress.

## Interaction and evidence

- Long-running probes are asynchronous and report queued/running/succeeded/failed states; cancellation is explicit.
- Latency is measured and communicated, not renamed as performance without evidence.
- Freehand/vector annotations preserve source data and expose validation results. Do not claim neural processing without a tested model.
- Headless modules expose CLI/API contracts; GUIs are optional clients, not implied capabilities.
- Red means error; cyan means active signal; yellow means action/focus. State is also shown with text or icons.

## Accessibility

Provide keyboard-visible focus, semantic labels, scalable layouts, reduced-motion support and WCAG AA contrast targets (4.5:1 normal text; 3:1 large text). Touch controls should expose at least 44×44 CSS-pixel hit areas where applicable. Validate keyboard-only use, 200% zoom, narrow windows and reduced motion.

## Implementation boundary and release gate

Native libraries/firmware without GUI document their host-facing API and must not imply a GUI exists. Conceptual wireframes are labelled **concept**; screenshots must come from a runnable build. A GUI change records purpose, loading/empty/error states, focus behavior, responsive behavior, accessibility and real evidence.

## Українська

Чорний і графітовий — основа; `#FFCC00` — головний жовтий акцент, `#FFE600` — підсвічування, `#E53935` — критичні помилки, `#29B6F6` — активність і потоки даних. Верхня панель показує виміряний статус і реальний час; робоча область — фактичні сигнали, а не декоративну імітацію. Значення `55 / ~~~ / 21:33` є лише макетними. Для компонентів без GUI описуються CLI/API та зовнішній клієнт. Стани, фокус клавіатури, масштабування та reduced motion обов'язкові.

## Русский

Чёрный и графитовый — основа; `#FFCC00` — главный жёлтый акцент, `#FFE600` — подсветка, `#E53935` — критические ошибки, `#29B6F6` — активность и потоки данных. Верхняя панель показывает измеренный статус и реальное время; рабочая область — фактические сигналы, а не декоративную имитацию. Значения `55 / ~~~ / 21:33` являются только макетными. Для компонентов без GUI описываются CLI/API и внешний клиент. Состояния, клавиатурный фокус, масштабирование и reduced motion обязательны.
