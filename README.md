<!-- ToolzNG Duck Edition -->
<div align="center">
  <img src="assets/readme-hero.svg" alt="ToolzNG — утка-инженер и диагностика устройств Apple" width="100%" />
  <br />
  <img alt="ToolzNG 0.6.0" src="https://img.shields.io/badge/ToolzNG-0.6.0-FFD600?style=for-the-badge&labelColor=171923" />
  <img alt="SAMBU Engine 1.2.0" src="https://img.shields.io/badge/SAMBU%20Engine-1.2.0-FFD600?style=for-the-badge&labelColor=171923" />
  <img alt="C17" src="https://img.shields.io/badge/C-17-FFD600?style=for-the-badge&labelColor=171923" />
  <img alt="Windows" src="https://img.shields.io/badge/Windows-10%2F11-FFD600?style=for-the-badge&labelColor=171923" />
  <img alt="MIT License" src="https://img.shields.io/badge/license-MIT-FFD600?style=for-the-badge&labelColor=171923" />
  <a href="https://github.com/bboroski/ToolNG/actions/workflows/windows-ci.yml"><img alt="CI" src="https://github.com/bboroski/ToolNG/actions/workflows/windows-ci.yml/badge.svg" /></a>
</div>

# 🦆 ToolzNG

**ToolzNG** — компактная нативная Windows-утилита на C17 для диагностики USB-подключения Apple, проверки структуры локальных резервных копий и создания приватных диагностических отчётов. В основе — нативный Win32/GDI UI, переносимое ядро SAMBU, графитово-чёрная и жёлтая тема и утка-инженер.

> **Duck Design:** сначала доказательства, потом выводы. Сначала безопасность, потом удобство. Ни одной кнопки, обещающей то, чего программа не умеет.

ToolzNG показывает только то, что Windows действительно может наблюдать. Приложение не является продуктом Apple и не пытается управлять закрытыми механизмами iOS.

## 📍 Состояние проекта

| Компонент | Версия / статус |
|---|---|
| Windows-приложение | **0.6.0** · Win32 / GDI |
| Диагностическое ядро | **SAMBU Engine 1.2.0** · переносимый C17 |
| Контракт отчёта | <code>toolzng.diagnostic.v1</code> |
| Windows GUI | Windows 10 / 11 x64 |
| Переносимые проверки ядра | Windows, Linux и macOS с C17 toolchain и CMake |
| Лицензия | MIT |
| Зрелость | Экспериментальный utility-проект; не сертифицирован Apple |

## 🚀 Что нового в 0.6.0

- **SAMBU Quality Gate:** проверяет согласованность доказательств до расчёта readiness.
- **Строгие границы доступа:** неизвестный origin и неизвестные операции закрываются по умолчанию.
- **Точные backup-счётчики:** частично проверенная коллекция не выдаётся за полностью валидную.
- **Объяснимый confidence:** отчёт показывает качество входных данных и битовые флаги предупреждений.
- **Актуализированный Duck Design:** правила доступности, приватности, контрастности, версионирования и правдивых UI-превью.
- **Усиленный CI:** Windows + Linux + macOS, отдельный sanitizer job, SHA-pinned GitHub Actions и Dependabot для обновления Actions.

## ✨ Возможности

| Модуль | Реальная функция | Важное ограничение |
|---|---|---|
| **USB Diagnostics** | Перечисляет присутствующие Windows PnP-устройства и ищет Apple Vendor ID <code>05AC</code>; проверяет Apple Mobile Device Service. | Видимый USB-интерфейс не доказывает полноценный pairing iOS. |
| **Duck Readiness** | Объяснимая оценка готовности Windows-side подключения от 0 до 100. | Не является тестом скорости или аппаратного здоровья iPhone. |
| **SAMBU Quality Gate** | Выявляет неполное сканирование, противоречивые backup-счётчики, неполный набор backup-маркеров и неподтверждённое окно USB-стабильности. | Предупреждения уменьшают confidence; они не устанавливают аппаратную первопричину. |
| **USB Pulse** | Отслеживает Windows device notifications для Apple USB и считает события в ограниченном окне. | Не измеряет электрические характеристики и скорость USB. |
| **Recurrence Radar** | Классифицирует single / recurrent / persistent finding-события в 10-минутном окне. | Не считает повторную перерисовку окна новым событием; ledger ограничен 32 кодами. |
| **Backup Sentinel** | Read-only проверяет Info.plist и Manifest.db / Manifest.plist. | Не открывает payload и не доказывает целостность или восстановимость копии. |
| **Backup Freshness** | Смотрит время изменения самого свежего Info.plist. | Это свежесть маркера, а не всех данных backup. |
| **DuckFix** | Даёт следующий безопасный шаг по приоритетному диагностическому finding. | Только рекомендации; автоматического ремонта нет. |
| **LinkDNA** | Сравнивает локальный FNV-1a отпечаток набора видимых PnP-сигналов и состояния службы. | Не является серийным номером или криптографической идентичностью. |
| **PulseVault** | Хранит ограниченный JSONL-журнал USB-событий локально с ротацией. | Не сохраняет серийные номера и содержимое устройства. |
| **Privacy Shield** | Приватный отчёт скрывает Hardware ID и полный путь backup. | Перед отправкой файл всё равно нужно проверить вручную. |
| **Evidence Seal** | Создаёт SHA-256 sidecar для сохранённого TXT. | Подтверждает неизменность файла после создания, но не истинность исходных данных. |
| **Reports & Offline Help** | До 32 локальных отчётов, предпросмотр, копирование, экспорт TXT и справка F1. | PDF/XLSX и облачная синхронизация не реализованы. |
| **Duck UI** | Win32/GDI, High Contrast, Per-Monitor DPI v2 и zoom 80–160%. | Полный UI Automation provider и всеобъемлющая keyboard navigation пока в roadmap. |

## 🧮 Duck Readiness

Оценка — сумма независимых, объяснимых Windows-side сигналов:

| Сигнал | Баллы |
|---|---:|
| Сканирование завершено, Apple USB-интерфейс виден | +40 |
| Apple Mobile Device Service запущена | +25 |
| Локальный backup-каталог найден | +10 |
| Все найденные каталоги backup содержат ожидаемые маркеры | +15 |
| USB watcher активен, окно наблюдения завершено, отключений нет и устройство подтверждено | +10 |
| **Максимум** | **100** |

**Маркерная проверка — all-or-nothing.** Если найдено три каталога, но маркеры подтверждены только в одном, приложение не начисляет +15 за полную валидность. Оно показывает неполную структуру, снижает confidence и предлагает повторную проверку.

Confidence описывает полноту и согласованность локальных проверок, а не вероятность аппаратной исправности:

| Состояние проверки | Confidence |
|---|---:|
| Полная проверка сканирования и активный watcher, противоречий нет | 100% |
| Сканирование завершено, watcher недоступен до истечения окна наблюдения, integrity warnings нет | 85% |
| Сканирование не завершено, противоречий нет | 40% |
| Любой integrity warning от Quality Gate | 25% |

### Quality flags

| Flag | Что означает |
|---|---|
| <code>DEVICE_WITHOUT_SCAN</code> | Сигнал устройства присутствует без завершённого сканирования. |
| <code>BACKUP_COUNTS_INCONSISTENT</code> | Счётчики каталога и валидных маркеров противоречат друг другу. |
| <code>BACKUP_SET_INCOMPLETE</code> | Не все обнаруженные папки прошли проверку маркеров. |
| <code>STABILITY_WITHOUT_WATCH</code> | Временное окно истекло, но USB watcher не был активен. |

## 🛡️ Privacy and safety

- Локальное чтение и сканирование разрешены.
- Запись приватного отчёта, экспорт и буфер обмена требуют явного пользовательского действия.
- Изменение устройства и отправка данных по сети запрещены политикой SAMBU.
- ToolzNG не удаляет резервные копии, фото, документы, приложения или кэш.
- Не обходятся пароль, Activation Lock, pairing и другие механизмы защиты Apple.
- Не выполняются jailbreak, прошивка, сброс или восстановление.
- Приложение не читает переписки, контакты, фотографии и payload-файлы резервных копий.
- Данные хранятся в <code>%LOCALAPPDATA%\ToolzNG</code>; USB-журнал ограничен ротацией, история отчётов — 32 файла.
- Экспорт остаётся локальным и инициируется пользователем после просмотра.

## 🎨 Duck Design System

| Токен | Цвет | Роль |
|---|---|---|
| Graphite | <code>#0D0D0D</code> | Фон приложения |
| Surface | <code>#141414</code> | Шапка и sidebar |
| Panel | <code>#1A1A1A</code> | Карточки и рабочие области |
| Duck Yellow | <code>#FFD600</code> | Главные действия и фокус |
| Deep Gold | <code>#BFA100</code> | Вторичный акцент |
| Primary Text | <code>#E6E6E6</code> | Основной текст |
| Muted Text | <code>#999999</code> | Вспомогательный текст |

На фоне <code>#0D0D0D</code> контраст составляет приблизительно **15.6:1** для основного текста, **13.8:1** для жёлтого и **6.8:1** для вторичного текста. Статус не кодируется одним цветом: используются также текстовые подписи. Windows High Contrast и Per-Monitor DPI v2 учитываются текущим UI.

Полный норматив: **[docs/DUCK_DESIGN_RULES.md](docs/DUCK_DESIGN_RULES.md)**.

## 🖼️ UI previews

SVG ниже — **статические макеты, а не фотографии работающего приложения**. Значения и состояния демонстрационные и отмечены SAMPLE DATA.

<p align="center">
  <img src="assets/screenshots/overview.svg" alt="ToolzNG overview UI preview — sample data" width="100%" />
  <em>Обзор: readiness, статус USB и основные действия.</em>
</p>

<p align="center">
  <img src="assets/screenshots/usb-devices.svg" alt="ToolzNG Apple USB devices UI preview — sample data" width="100%" />
  <em>USB: видимые PnP-интерфейсы и состояние службы Apple.</em>
</p>

<p align="center">
  <img src="assets/screenshots/reports.svg" alt="ToolzNG reports UI preview — sample data" width="100%" />
  <em>Отчёты: локальная история, предпросмотр и экспорт.</em>
</p>

## 🧬 SAMBU architecture

ToolzNG использует переносимое ядро SAMBU на C17. Это не встраивание Python-интерпретатора SAMBU.

| Закон | Реализация |
|---|---|
| **S — Safety** | Fail-closed capabilities: неизвестные операции и origin запрещены. |
| **A — Adaptive** | Детерминированная оценка по наблюдаемым сигналам. |
| **M — Managed** | Bounded ledger с ограниченными счётчиками и монотонными timestamp. |
| **B — Branchable** | Ядро отделено от Win32 UI, platform adapter и экспорта. |
| **U — Unforced** | Стабильные finding IDs, версионированная схема отчёта, локальный контроль пользователя. |

Дополнительные семейства SAMBU структурируют архитектуру, поток выполнения, значения, память, безопасность, измерения, доказательства, координацию, преобразование и язык представления.

Подробности и формальные контракты: [docs/SAMBU_INTEGRATION.md](docs/SAMBU_INTEGRATION.md).

## 🏗️ Project layout

~~~text
ToolNG/
├── src/
│   ├── main.c                  # Win32/GDI application
│   ├── sambu_engine.c          # portable C17 policy and assessment
│   ├── sambu_engine.h          # stable engine contract
│   ├── toolzng.rc
│   └── toolzng.manifest
├── tests/
│   └── sambu_engine_tests.c
├── docs/
│   ├── DUCK_DESIGN_RULES.md
│   └── SAMBU_INTEGRATION.md
├── assets/
│   ├── screenshots/            # static SVG previews
│   └── readme-hero.svg
├── scripts/
│   ├── build-windows.ps1
│   └── verify_docs.py
└── .github/
    ├── workflows/windows-ci.yml
    └── dependabot.yml
~~~

## ⚡ Build

### Windows x64 GUI

**Requirements:** Windows 10/11 x64, Visual Studio 2022 / Build Tools with Desktop development with C++, CMake 3.20+.

~~~powershell
.\scripts\build-windows.ps1
~~~

Manual build and test:

~~~powershell
cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
~~~

Executable:

~~~text
build\Release\ToolzNG.exe
~~~

The program runs as the current user (<code>asInvoker</code>). Administrator rights are not required for normal USB inspection and local report creation.

### Portable SAMBU C17 core

~~~bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
python3 scripts/verify_docs.py
~~~

Linux and macOS build the portable core and tests. The native Win32 GUI is Windows-only.

## 🧪 Continuous integration

The workflow includes:
- C17 core build and CTest on Linux and macOS;
- Windows x64 GUI build and CTest;
- AddressSanitizer + UndefinedBehaviorSanitizer for the portable core;
- README local-link and SVG preview validation;
- SHA-pinned GitHub Actions and weekly Dependabot update checks.

CI runs on pushes, pull requests and manual dispatch. The workflow has minimum <code>contents: read</code> permissions and cancels superseded runs for the same ref.

## 🧭 Roadmap and explicit non-goals

**Not implemented in 0.6.0:**
- iPhone model and iOS version discovery through pairing APIs;
- electrical cable, power, USB throughput or port hardware tests;
- live iOS battery, temperature, network or internal-storage telemetry;
- automatic Apple driver repair or service restart;
- backup creation, modification, deletion or restore;
- cloud reporting, telemetry, auto-update, signed installer;
- PDF/XLSX exports;
- complete UI Automation provider and full keyboard navigation for every custom-drawn element.

**Next engineering milestones:**
1. Optional Windows adapter around a supported libimobiledevice build, gated by an explicit pairing capability check.
2. Model and iOS version display only after a successful, supported API response.
3. Better Apple driver and port diagnostics without silent system mutation.
4. RU/EN localization and full keyboard navigation.
5. UI Automation names, roles, state, focus and accessibility smoke tests across DPI and High Contrast.
6. Signed installer and reproducible release packaging.

A roadmap item is not an implemented feature until code, tests, documentation and CI all agree.

## 📚 References

- [Duck Design Rules](docs/DUCK_DESIGN_RULES.md)
- [SAMBU Integration](docs/SAMBU_INTEGRATION.md)
- [Microsoft — Win32 accessibility best practices](https://learn.microsoft.com/en-us/windows/win32/winauto/accessibility-best-practices)
- [Microsoft — Accessible text and contrast requirements](https://learn.microsoft.com/en-us/windows/apps/design/accessibility/accessible-text-requirements)
- [Microsoft — UI Automation for Win32](https://learn.microsoft.com/en-us/windows/desktop/winauto/entry-uiauto-win32)
- [Microsoft — High DPI desktop development](https://learn.microsoft.com/en-us/windows/win32/hidpi/high-dpi-desktop-application-development-on-windows)
- [GitHub — Secure use of Actions](https://docs.github.com/en/actions/reference/security/secure-use)
- [Apple — Если iPhone или iPad работает медленно](https://support.apple.com/ru-ru/102598)
- [Apple — Резервные копии iPhone и iPad](https://support.apple.com/ru-ru/108809)
- [Apple Devices for Windows](https://support.apple.com/guide/devices-windows/install-the-apple-devices-app-mchl5ded2763/windows)

## 🤝 Contributing

Небольшие проверяемые pull request, воспроизводимые bug reports и уточнения документации приветствуются.

Для ошибки укажи версию ToolzNG, версию Windows, точные шаги, ожидаемое и фактическое поведение и релевантный диагностический вывод без личных данных. Для изменений SAMBU добавляй positive, negative и boundary tests. Для новых диагностических сигналов документируй источник, предусловия, confidence и ограничения измерения.

Перед отправкой изменений запускай CMake build, CTest и документационный валидатор. UI-изменения должны пройти проверки High Contrast, DPI, keyboard focus и читаемости.

## 📄 License

MIT. См. [LICENSE](LICENSE).

<div align="center">
  Нехай у кожному дні знаходиться місце для любові — тихої, щирої та сміливої. Нехай поруч будуть люди, з якими тепло мовчати й радісно мріяти; нехай слова підтримки знаходяться вчасно, а маленькі вчинки стають великими доказами турботи. Бережіть тих, хто робить світ світлішим, даруйте любов без страху й приймайте її без сумнівів. Хай добра у вашому житті буде більше — щодня. 🦆💛
</div>
