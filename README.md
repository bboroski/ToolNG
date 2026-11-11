<!-- Title: ToolNG — Windows USB та Apple Device Diagnostics -->
<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="Логотип ToolNG" width="76"></p>

# ToolNG

[![Windows CI](https://github.com/bboroski/ToolNG/actions/workflows/windows-ci.yml/badge.svg?branch=main)](https://github.com/bboroski/ToolNG/actions/workflows/windows-ci.yml) [![MIT License](https://img.shields.io/badge/license-MIT-FFD600?style=flat&labelColor=111111)](LICENSE)

**ToolNG** — локальна Windows-утиліта для read-only спостереження за PnP/USB-пристроями, пов'язаними службами та маркерами резервних копій із пояснюваними діагностичними звітами.

<sub>Оновлено: 2026-11-11 · Версія: 0.6.1 · Цільовий desktop: Windows 10/11</sub>

> **TL;DR:** CMake збирає portable policy core і тести на host-системах; повна desktop-оболонка збирається для Windows. Діагностика не змінює стан пристрою.

[Українська](README.md) · [English](README.en.md) · [Русский](README.ru.md)

## Зміст

- [Призначення](#призначення)
- [Можливості](#можливості)
- [Вимоги та збірка](#вимоги-та-збірка)
- [Тести та CI](#тести-та-ci)
- [Межі та модель безпеки](#межі-та-модель-безпеки)
- [Прев'ю інтерфейсу](#превю-інтерфейсу)
- [Документація й інтеграції](#документація-й-інтеграції)
- [FAQ](#faq)
- [Підтримка та внески](#підтримка-та-внески)
- [Ліцензія](#ліцензія)

## Призначення

ToolNG допомагає оператору зрозуміти, які локальні сигнали про пристрій підтверджені, які відсутні та наскільки повними є докази. Це діагностичний інструмент, а не repair utility або device-management service.

## Можливості

- Спостереження за Windows PnP/USB-пристроями та пов'язаними службами.
- Read-only перевірка локальних маркерів backup; приватний вміст резервної копії не читається.
- Обмежена історія подій підключення/відключення та локальні звіти.
- Оцінка готовності з окремою повнотою доказів.
- Редагування чутливих полів перед показом або експортом звіту.

## Вимоги та збірка

- CMake **3.20+**.
- Компілятор C17: MSVC для Windows desktop або GCC/Clang для portable core.
- Windows 10/11 для повної GUI-програми; Windows SDK/Win32 API надає системні бібліотеки.
- На не-Windows host CMake конфігурує policy core і тести, але не збирає Win32 desktop shell.

~~~bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Release
ctest --test-dir build --build-config Release --output-on-failure
python one.py check
~~~

Після успішної збірки Windows desktop executable має назву ToolNG. Точний шлях і формат артефакту залежать від generator та конфігурації CMake.

## Тести та CI

Тестові цілі перевіряють policy engine, workflow, platform layer та relaxation behavior. Запускайте їх через CTest. Основні перевірки: [Windows CI](.github/workflows/windows-ci.yml), [Windows build](.github/workflows/build-windows.yml) і [quality gate](.github/workflows/duck-quality-gate.yml).

## Межі та модель безпеки

> **Важливо:** ToolNG працює в режимі спостереження. Він не ремонтує пристрої, не змінює драйвери чи служби, не виконує jailbreak або activation-lock bypass і не завантажує діагностику в хмару.

Маркер backup не доводить цілісність його вмісту чи можливість відновлення. Порада оператору не є виконаною дією. Не запускайте додаткові дії поза явним дозволом користувача.

## Прев'ю інтерфейсу

Зображення нижче — статичні SVG-макети з демонстраційними SAMPLE DATA, а не результат живого сканування.

<p align="center"><img src="assets/screenshots/overview.svg" alt="ToolNG: оглядова панель із демонстраційними даними" width="960"></p>
<p align="center"><img src="assets/screenshots/usb-devices.svg" alt="ToolNG: список USB-пристроїв із демонстраційними даними" width="960"></p>
<p align="center"><img src="assets/screenshots/reports.svg" alt="ToolNG: локальні діагностичні звіти" width="960"></p>

## Документація й інтеграції

- [Збірка](docs/BUILDING.md) · [безпека й межі](docs/SAFETY_AND_SCOPE.md)
- [Проєктування GUI](docs/GUI_DESIGN.md) · [інженерні quality gates](docs/ENGINEERING_QUALITY_GATES.md)
- [Інтеграція із SAMBU](docs/SAMBU_INTEGRATION.md) · [manifest](sambu.module.json)
- [SAMBU](https://github.com/bboroski/SAMBU) надає окремі мовні й політичні контракти; ToolNG використовує вибрані правила, портовані в C, а не вбудовує Python runtime.

## FAQ

**Чи працює повний desktop GUI на Linux/macOS?** Ні. На не-Windows платформах збираються portable core та його тести.

**Чи читає ToolNG приватний вміст backup?** Ні. Перевірка обмежена локальними маркерами.

**Чи надсилаються звіти на сервер?** Ні. Проєкт не виконує cloud upload діагностики.

## Підтримка та внески

Баги: [Issue tracker](https://github.com/bboroski/ToolNG/issues) · [bug report](https://github.com/bboroski/ToolNG/issues/new). PR має містити сценарій відтворення, відповідні тести та опис змін. Перед внеском прочитайте [CONTRIBUTING.md](CONTRIBUTING.md).

Профіль супроводу: [bboroski](https://github.com/bboroski). Не публікуйте серійні номери користувачів або приватні діагностичні дані у відкритих issue.

## Ліцензія

ToolNG поширюється за **MIT License**. Повний текст — у [LICENSE](LICENSE).
