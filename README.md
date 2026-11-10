<div align="center">

<img src="assets/readme-hero.svg" alt="ToolzNG — нативна діагностика Apple-пристроїв у Windows" width="100%" />

# ToolzNG

### Діагностика підключення Apple-пристроїв у Windows

**Перевіряй. Розумій. Дій безпечно.**

Локальний C17 / Win32-інструмент, який пояснює видимі Windows сигнали Apple-пристроїв, USB-події та маркери резервних копій — без прихованої зміни пристрою чи хмарного передавання даних.

[Українська](README.md) · [English](README.en.md) · [Русский](README.ru.md)

![ToolzNG 0.6.1](https://img.shields.io/badge/ToolzNG-0.6.1-FFD600?style=for-the-badge&labelColor=171923)
![C17](https://img.shields.io/badge/C-17-FFD600?style=for-the-badge&labelColor=171923)
![Windows](https://img.shields.io/badge/Windows-10%20%7C%2011-FFD600?style=for-the-badge&labelColor=171923)
![MIT](https://img.shields.io/badge/License-MIT-FFD600?style=for-the-badge&labelColor=171923)
[![CI](https://github.com/bboroski/ToolNG/actions/workflows/windows-ci.yml/badge.svg)](https://github.com/bboroski/ToolNG/actions/workflows/windows-ci.yml)

[Огляд](#-що-таке-toolzng) · [Інтерфейс](#-інтерфейс) · [Оцінювання](#-як-працює-оцінювання) · [Збірка](#-збірка) · [Приватність](#-приватність-та-межі)

</div>

---

## Що таке ToolzNG

Проблеми з підключенням iPhone або iPad можуть виникати на різних рівнях Windows: пристрій не з'являється в Plug and Play, служба Apple Mobile Device Service недоступна, USB-з'єднання переривається або локальна резервна копія має неповні структурні маркери. ToolzNG розділяє ці сигнали замість спрощеного статусу «підключено / не підключено».

Це **діагностичний помічник для Windows**, а не пакет ремонту iPhone, менеджер backup чи монітор здоров’я iOS.

## Інтерфейс

<p align="center"><img src="assets/toolzng-duck.svg" alt="Фірмовий маскот ToolzNG" width="220" /></p>

<p align="center">
<img src="assets/screenshots/overview.svg" alt="Прев'ю панелі ToolzNG; демонстраційні дані" width="100%" />
**01 · Огляд** — готовність, покриття доказами та пріоритетна знахідка.
</p>

<p align="center">
<img src="assets/screenshots/usb-devices.svg" alt="Прев'ю USB-інспекції Apple; демонстраційні дані" width="100%" />
**02 · USB Apple** — видимість PnP, стан служби та повнота перевірок.
</p>

<p align="center">
<img src="assets/screenshots/reports.svg" alt="Прев'ю локальних звітів; демонстраційні дані" width="100%" />
**03 · Звіти** — історія, докази, прапорці якості та попередній перегляд.
</p>

<img src="assets/diagnostic-flow.svg" alt="Процес ToolzNG: спостереження, перевірка, оцінювання, рекомендація та звіт за дією користувача" width="100%" />

> **Візуальна примітка:** інтерфейсні панелі — статичні SVG-прев'ю з демонстраційними SAMPLE DATA, а не знімки живого сканування чи реальні дані пристрою.

## Можливості

| Модуль | Практична цінність |
|---|---|
| **Apple USB Discovery** | Виявляє Windows PnP-інтерфейси з Apple Vendor ID <code>05AC</code> і показує стан Apple Mobile Device Service. |
| **Duck Readiness** | Пояснювана оцінка 0–100 за вимірюваними сигналами Windows із окремим покриттям доказами. |
| **SAMBU Quality Gate** | Виявляє неповні сканування, суперечливі лічильники backup і непідтверджені висновки про стабільність. |
| **USB Pulse** | Відстежує події підключення/відключення Apple USB в обмеженому часовому вікні. |
| **Recurrence Radar** | Розрізняє одиничні, повторювані та стійкі переходи станів у 10-хвилинному вікні. |
| **Backup Sentinel** | Read-only перевірка маркерів <code>Info.plist</code> та <code>Manifest.db</code> / <code>Manifest.plist</code>. |
| **Backup Freshness** | Показує час зміни найновішого маркера <code>Info.plist</code>. |
| **DuckFix Guide** | Пропонує безпечний наступний крок, але не виконує автоматичний ремонт. |
| **LinkDNA** | Порівнює локальний FNV-1a відбиток сигналів Apple; це не криптографічна ідентичність. |
| **PulseVault** | Обмежений локальний JSONL-журнал USB-подій із ротацією. |
| **Privacy Shield / Evidence Seal** | Приховує Hardware ID і повні шляхи backup; SHA-256 sidecar виявляє подальшу зміну файла. |
| **Offline Reports & Help** | До 32 локальних звітів, перегляд, копіювання, TXT-експорт і довідка F1. |

## Як працює оцінювання

| Підтверджений сигнал | Максимум |
|---|---:|
| Сканування завершене, інтерфейс Apple USB видимий | 40 |
| Apple Mobile Device Service підтверджено запущена | 25 |
| Локальний корінь backup знайдено | 10 |
| Усі знайдені каталоги backup мають очікувані маркери | 15 |
| Пристрій підтверджений, watcher активний, вікно спостереження завершене, видалень не було | 10 |
| **Разом** | **100** |

Оцінка нормалізується за вагами, які реально вдалося виміряти. **Evidence coverage** показує частку повної моделі, доступну для перевірки; неповне покриття робить результат попереднім. Ідеальний бал із частковими даними не є повним проходженням перевірки.

Quality Gate розрізняє:
- **Observed** — перевірку завершено, сигнал підтверджено.
- **Not found** — перевірку завершено, сигнал відсутній.
- **Unknown** — доступ не отримано, перелік неповний або спостереження не завершене.

Відмова Service Control Manager не означає, що службу зупинено. Неповна перевірка backup не доводить відсутність копії. Неактивний watcher не підтверджує стабільність USB.

## Приватність та межі

ToolzNG дотримується local-first підходу:

- не потрібні акаунт, телеметрія чи хмарне передавання;
- payload резервних копій не читається для структурної перевірки;
- приватні звіти приховують Hardware ID і повні шляхи backup;
- створення звіту, буфер обміну та експорт вимагають явної дії користувача;
- політика SAMBU забороняє зміну пристрою та мережеве передавання;
- журнали й історія звітів мають фіксовані межі;
- звичайна робота не потребує прав адміністратора.

**Межі доказів:** VID <code>05AC</code> не доводить iOS pairing; маркери не доводять цілісність або відновлюваність backup; timestamp <code>Info.plist</code> не є віком усіх даних; FNV-1a не є криптографічною ідентичністю; SHA-256 sidecar не доводить правдивість звіту.

ToolzNG не виконує jailbreak, не обходить пароль або Activation Lock, не прошиває та не скидає iPhone, не читає особисті дані, не ремонтує драйвери автоматично й не вимірює електричні параметри кабелю, USB throughput, стан батареї чи температуру iOS.

## Архітектура

| Рівень | Технологія |
|---|---|
| Desktop UI | C17 · Win32 API · GDI |
| Device discovery | Windows SetupAPI / Plug and Play |
| Apple service | Windows Service Control Manager |
| USB events | Windows device notifications |
| Diagnostic core | Портативне C17-ядро політик і workflow SAMBU |
| Reports | Локальний TXT · SHA-256 sidecar |
| Build | CMake 3.20+ |

Ядро відповідає за детерміновану оцінку, дозволи, якість доказів та обмежену пам’ять findings. Windows API ізольовані на платформному рівні. Workflow DSL викликає п’ять фіксованих дій; це не універсальний інтерпретатор, shell чи скриптова мова.

## Збірка

**Windows x64:** Windows 10/11, Visual Studio 2022 або MSVC Build Tools із Desktop development with C++, CMake 3.20+.

~~~powershell
cmake -S . -B build -A x64 -DBUILD_TESTING=ON
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
~~~

Виконуваний файл: <code>build\Release\ToolzNG.exe</code>. Графічний інтерфейс працює лише у Windows. Збірка ядра не перевіряє драйвери Apple, pairing, реальний пристрій чи фізичний кабель.

## Гарячі клавіші

| Комбінація | Дія |
|---|---|
| <code>F5</code> | Повторити USB-сканування |
| <code>Ctrl+R</code> | Створити локальний звіт |
| <code>F1</code> | Відкрити довідку |
| <code>Ctrl++</code> / <code>Ctrl+-</code> | Змінити масштаб |
| <code>Ctrl+0</code> | Повернути 100% |
| <code>A−</code> / <code>A+</code> | Масштаб панелі |

Наявні Per-Monitor DPI v2 та збереження масштабу 80–160%. Повна підтримка UI Automation і завершена клавіатурна навігація залишаються в roadmap.

## Дорожня карта

1. Необов’язковий Windows adapter для сумісної збірки <code>libimobiledevice</code> із перевіркою pairing.
2. Модель пристрою та версія iOS — лише коли їх повертає довірений pairing API.
3. Поглиблена діагностика Apple USB-драйверів без прихованої зміни системи.
4. RU/EN локалізація, доступність і клавіатурна навігація.
5. Підписаний інсталятор, відтворювана упаковка та апаратні smoke-тести.

## Інженерні стандарти

- [Duck Design Rules](docs/DUCK_DESIGN_RULES.md) — докази, приватність, обмеження та дизайн.
- [SAMBU Integration](docs/SAMBU_INTEGRATION.md) — оцінювання й обмежений workflow.
- [Engineering Quality Gates](docs/ENGINEERING_QUALITY_GATES.md) — тести, надійність і релізні критерії.

Для суттєвих змін UI додавайте реальні знімки програми або явно позначайте прев’ю як демонстраційні.

## Ліцензія

MIT — див. [LICENSE](LICENSE).

<div align="center">

**ToolzNG — менше здогадок, більше зрозумілих сигналів.** 🦆

</div>