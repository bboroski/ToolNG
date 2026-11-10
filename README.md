<div align="center">

  <img src="assets/readme-hero.svg" alt="ToolzNG — native Apple device diagnostics for Windows" width="100%" />

  <br />
  <img alt="ToolzNG 0.6.1" src="https://img.shields.io/badge/ToolzNG-0.6.1-FFD600?style=for-the-badge&labelColor=171923" />
  <img alt="SAMBU Engine 1.2.0" src="https://img.shields.io/badge/SAMBU-1.2.0-FFD600?style=for-the-badge&labelColor=171923" />
  <img alt="C17" src="https://img.shields.io/badge/C-17-FFD600?style=for-the-badge&labelColor=171923" />
  <img alt="Windows 10 and 11" src="https://img.shields.io/badge/Windows-10%20%7C%2011-FFD600?style=for-the-badge&labelColor=171923" />
  <img alt="MIT" src="https://img.shields.io/badge/License-MIT-FFD600?style=for-the-badge&labelColor=171923" />
  <a href="https://github.com/bboroski/ToolNG/actions/workflows/windows-ci.yml"><img alt="CI" src="https://github.com/bboroski/ToolNG/actions/workflows/windows-ci.yml/badge.svg" /></a>

  **Проверяй. Понимай. Действуй безопасно.**

  Нативная диагностика Apple USB, read-only аудит локальных backup-маркеров и приватные отчёты — в одном лёгком Windows-приложении.

</div>

---

## ToolzNG — когда нужно знать, а не гадать

Подключение iPhone к Windows иногда превращается в квест: устройство не отображается, служба Apple недоступна, USB-соединение периодически пропадает, а папка резервной копии выглядит подозрительно.

**ToolzNG собирает понятную картину Windows-side состояния** и показывает, какие сигналы реально подтверждены, какие требуют повторной проверки и что безопасно сделать дальше.

Это не «ускоритель iPhone» и не магическая кнопка ремонта. Это компактная инженерная панель на C17 и Win32 API: без Electron, без обязательного аккаунта, без облачной отправки диагностических данных.

## Что ToolzNG умеет

| Возможность | Польза |
|---|---|
| **Apple USB Discovery** | Находит присутствующие Windows PnP-интерфейсы Apple по Vendor ID 05AC и показывает доступность Apple Mobile Device Service. |
| **Duck Readiness** | Сводит проверяемые Windows-side сигналы в объяснимую оценку от 0 до 100. |
| **SAMBU Quality Gate** | Выявляет неполные и противоречивые данные, прежде чем они попадут в итоговую оценку. |
| **USB Pulse + Recurrence Radar** | Отслеживает подключения и отключения Apple USB и помогает отличать одиночный эпизод от повторяющегося сбоя. |
| **Backup Sentinel** | Read-only проверяет структурные маркеры локальных резервных копий, не открывая их личное содержимое. |
| **Backup Freshness** | Показывает возраст самого свежего Info.plist по времени изменения. |
| **DuckFix Guide** | Предлагает следующий безопасный шаг на основании приоритетного finding. |
| **LinkDNA** | Сравнивает локальный FNV-1a отпечаток набора Windows-visible сигналов Apple. |
| **PulseVault** | Ведёт ограниченный локальный JSONL-журнал USB-событий с ротацией. |
| **Privacy Shield + Evidence Seal** | Скрывает Hardware ID и полный путь backup в приватном отчёте; SHA-256 sidecar помогает заметить последующее изменение файла. |
| **Offline Reports & Help** | Даёт локальную историю до 32 отчётов, встроенный предпросмотр, копирование, TXT-экспорт и справку F1. |

## Посмотри на интерфейс

Ниже — обновлённые превью экранов ToolzNG 0.6.1. Они собраны по актуальной Win32/GDI-разметке и показывают новые состояния Quality Gate. **Это статические UI-превью с демонстрационными значениями, а не фотографии работающего приложения и не результаты реального сканирования.**

<p align="center">
  <img src="assets/screenshots/overview.svg" alt="ToolzNG 0.6.1 overview — Duck Readiness and SAMBU Quality Gate, sample data" width="100%" />
  <strong>01 · Центр управления</strong><br />
  Duck Readiness, USB Pulse, Backup Sentinel и состояние качества диагностических сигналов.
</p>

<p align="center">
  <img src="assets/screenshots/usb-devices.svg" alt="ToolzNG 0.6.1 USB devices — Apple PnP and backup marker coverage, sample data" width="100%" />
  <strong>02 · USB-устройства Apple</strong><br />
  Windows PnP discovery, состояние службы Apple, Quality Gate и покрытие backup-маркеров.
</p>

<p align="center">
  <img src="assets/screenshots/reports.svg" alt="ToolzNG 0.6.1 reports — local diagnostic report preview, sample data" width="100%" />
  <strong>03 · Локальные отчёты</strong><br />
  Предпросмотр диагностического отчёта с readiness, evidence mask, quality flags и версией SAMBU.
</p>

## Почему это полезно

**Не просто «устройство найдено».** ToolzNG показывает отдельные уровни проверки: видит ли Windows Apple USB, доступна ли служба, что произошло с соединением за последние десять минут и какие маркеры найдены в локальных backup-каталогах.

**Не прячет сомнительные данные за красивым баллом.** SAMBU Quality Gate проверяет целостность входных сигналов. Если три папки резервных копий обнаружены, а маркеры подтверждены только в одной, программа не выдаёт частичную проверку за полную.

**Не забирает контроль у пользователя.** Приложение не исправляет систему тайком, не удаляет файлы и не загружает отчёты в облако. Оно объясняет состояние и предлагает следующий безопасный шаг.

## Duck Readiness — оценка с понятной математикой

Текущая модель использует следующие веса:

| Подтверждённый сигнал | Баллы |
|---|---:|
| Сканирование завершено и Apple USB-интерфейс виден | +40 |
| Apple Mobile Device Service запущена | +25 |
| Локальный каталог backup найден | +10 |
| Все обнаруженные папки прошли проверку ожидаемых маркеров | +15 |
| Устройство подтверждено, watcher активен, окно наблюдения завершено и отключений нет | +10 |
| **Максимум** | **100** |

### SAMBU Quality Gate

- Устройство не засчитывается без завершённого сканирования.
- Валидных backup-каталогов не может быть больше, чем просмотренных.
- Полный бонус за маркеры выдаётся только тогда, когда проверены все обнаруженные каталоги.
- Окно стабильности не считается доказательством, если USB watcher не был активен.
- Противоречия и неполные сигналы формируют integrity flags и снижают confidence.

| Integrity flag | Значение |
|---|---|
| <code>DEVICE_WITHOUT_SCAN</code> | Сигнал устройства есть, завершённого сканирования нет. |
| <code>BACKUP_COUNTS_INCONSISTENT</code> | Счётчики backup противоречат друг другу. |
| <code>BACKUP_SET_INCOMPLETE</code> | Не все найденные папки прошли проверку маркеров. |
| <code>STABILITY_WITHOUT_WATCH</code> | Окно наблюдения прошло без активного USB watcher. |

Confidence описывает полноту и согласованность локальной диагностики. Это не вероятность аппаратной исправности iPhone и не benchmark производительности.

## Быстрые клавиши

| Клавиша | Действие |
|---|---|
| <kbd>F5</kbd> | Повторить USB-сканирование |
| <kbd>Ctrl</kbd> + <kbd>R</kbd> | Создать локальный диагностический отчёт |
| <kbd>F1</kbd> | Открыть встроенную справку |
| <kbd>Ctrl</kbd> + <kbd>+</kbd> / <kbd>−</kbd> | Увеличить или уменьшить масштаб |
| <kbd>Ctrl</kbd> + <kbd>0</kbd> | Вернуть масштаб 100% |
| <kbd>A−</kbd> / <kbd>A+</kbd> | Изменить масштаб кнопками панели |

## Приватность и безопасность

ToolzNG проектируется по принципу local-first:

- локальное чтение и USB-сканирование доступны приложению;
- создание отчёта, экспорт и копирование требуют явного действия пользователя;
- изменение устройства и сетевая отправка данных запрещены политикой SAMBU;
- Backup Sentinel проверяет имена и атрибуты маркерных файлов, не читая payload резервной копии;
- приватный отчёт скрывает Hardware ID и полный путь backup;
- USB-журнал ограничен ротацией, история отчётов — 32 файла;
- LinkDNA хранит хэш-профиль, а не исходный Hardware ID;
- обычная работа не требует прав администратора.

**Честные границы:** видимый VID_05AC не доказывает pairing iOS; маркерные файлы не доказывают восстановимость backup; SHA-256 sidecar не доказывает правдивость самого отчёта.

ToolzNG не выполняет jailbreak, не обходит пароль или Activation Lock, не прошивает и не сбрасывает устройство, не читает личные данные и не разгоняет CPU/GPU iPhone.

## Нативная архитектура

| Слой | Технология |
|---|---|
| Desktop UI | C17 · Win32 API · GDI |
| Device discovery | Windows SetupAPI / PnP |
| Apple service check | Windows Service Control Manager |
| Device events | Windows device notifications |
| Diagnostic core | SAMBU Engine 1.2.0 · portable C17 |
| Reports | Local TXT · SHA-256 sidecar |
| Build | CMake 3.20+ |

### SAMBU Engine

| Закон | Применение |
|---|---|
| **S — Safety** | Fail-closed permissions; неизвестные operation и origin запрещены. |
| **A — Adaptive** | Детерминированная оценка по наблюдаемым сигналам. |
| **M — Managed** | Ограниченный finding ledger и десятиминутное окно повторяемости. |
| **B — Branchable** | Переносимое ядро отделено от Win32 UI и platform adapter. |
| **U — Unforced** | Стабильные finding IDs, локальный экспорт и контроль пользователя. |

Подробности: [SAMBU Integration](docs/SAMBU_INTEGRATION.md) · [Duck Design Rules](docs/DUCK_DESIGN_RULES.md).

## Установка и сборка

### Windows x64

**Требования:** Windows 10/11 x64, Visual Studio 2022 или Build Tools с workload Desktop development with C++, CMake 3.20+.

Открой PowerShell в корне репозитория:

~~~powershell
.\scripts\build-windows.ps1
~~~

Или вручную:

~~~powershell
cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
~~~

Готовая программа:

~~~text
build\Release\ToolzNG.exe
~~~

Приложение запускается с правами текущего пользователя (<code>asInvoker</code>).

### Переносимое ядро SAMBU

Ядро и его тесты можно собирать на Windows, Linux и macOS:

~~~bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
python3 scripts/verify_docs.py
~~~

Графический интерфейс Win32 остаётся Windows-only.

## Проверки проекта

GitHub Actions выполняет:
- C17 build + CTest на Linux и macOS;
- Windows x64 GUI build + CTest;
- AddressSanitizer и UndefinedBehaviorSanitizer для переносимого ядра;
- проверку локальных ссылок README и SVG-превью;
- workflow с минимальными permissions и SHA-pinned GitHub Actions.

[Открыть GitHub Actions](https://github.com/bboroski/ToolNG/actions/workflows/windows-ci.yml).

## Что пока не реализовано

Мы предпочитаем честный roadmap красивым обещаниям.

- Получение модели iPhone и версии iOS через pairing API.
- Электрические тесты кабеля, порта и пропускной способности USB.
- Чтение температуры, состояния батареи, внутреннего хранилища или сети iOS.
- Автоматическое исправление Apple-драйверов и перезапуск служб.
- Создание, изменение, удаление или восстановление backup.
- PDF/XLSX, облачные отчёты, телеметрия и автообновление.
- Полный UI Automation provider для каждого саморисуемого элемента и законченная клавиатурная навигация.

### Куда движется проект

1. Опциональный Windows adapter для поддерживаемой сборки <code>libimobiledevice</code> с явной проверкой pairing capability.
2. Модель и версия iOS — только после подтверждённого ответа поддерживаемого API.
3. Более глубокая диагностика Apple USB-драйверов без скрытой модификации системы.
4. RU/EN локализация, доступность и полноценная клавиатурная навигация.
5. Подписанный установщик, воспроизводимая упаковка и аппаратные smoke-тесты.

## Участие в проекте

Баг-репорты и небольшие проверяемые pull request приветствуются. Для ошибки укажи версию ToolzNG, Windows, шаги воспроизведения, ожидаемый и фактический результат. Не прикладывай личные данные и содержимое резервных копий.

Перед отправкой изменений запускай CMake build, CTest и валидатор документации. Новые диагностические функции должны иметь источник сигнала, описанные ограничения и позитивные, негативные и граничные тесты.

## Лицензия

MIT — см. [LICENSE](LICENSE).

<div align="center">

  **ToolzNG — меньше догадок, больше понятных сигналов.** 🦆

  Нехай у кожному дні знаходиться місце для любові — тихої, щирої та сміливої. Нехай поруч будуть люди, з якими тепло мовчати й радісно мріяти; нехай слова підтримки знаходяться вчасно, а маленькі вчинки стають великими доказами турботи. Бережіть тих, хто робить світ світлішим, даруйте любов без страху й приймайте її без сумнівів. Хай добра у вашому житті буде більше — щодня. 💛

</div>
