<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="ToolNG mark" width="92"></p>
<h1 align="center">ToolNG</h1>
<p align="center"><strong>Windows Device Evidence Probe</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-WINDOWS%20DESKTOP-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/CORE-C17%20%2B%20WIN32-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/LICENSE-MIT-FFD600?style=for-the-badge&labelColor=111111"></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>

---

**ToolNG — локальний інструмент діагностики Windows, який відокремлює підтверджені сигнали пристрою від невідомого або неперевіреного стану.** Його головна цінність — прозора картина для оператора без автоматичного втручання в систему.

## Можливості

- Виявлення доступних Windows PnP/USB-інтерфейсів пристроїв.
- Перевірка стану відповідних системних служб.
- Read-only огляд локальних маркерів резервного копіювання.
- Обмежене спостереження за подіями підключення та відключення.
- Пояснювана оцінка готовності з окремим показником повноти доказів.
- Локальні звіти, журнали, експорт і редагування чутливих полів.
- Рекомендації щодо наступних кроків без автоматичного ремонту.

## Збірка та запуск

Потрібні Windows 10/11, CMake та сумісний C17-компілятор.

~~~powershell
cmake -S . -B build
cmake --build build --config Release
python one.py
python one.py check
~~~

У Windows також доступний <code>one.cmd</code>.

## Як читати результати

ToolNG розрізняє підтверджений сигнал, завершену перевірку без знахідки, невідомий стан і помилку перевірки. Висока оцінка за неповного покриття не подається як повне підтвердження. Звіти зберігаються локально; апаратні ідентифікатори й довгі локальні шляхи можуть бути приховані. Контрольна сума sidecar-файлу допомагає виявити подальшу зміну звіту, але не є цифровим підписом.

## Управління та контроль випуску

| Контроль | Правило |
|---|---|
| Платформа | Windows 10/11, C17, Win32 |
| Докази | Результат перевірки та coverage показуються окремо |
| Конфіденційність | Звіти локальні; чутливі поля можна редагувати |
| Безпечна дія | Порада в GUI не означає виконану операцію |
| Реліз | Перевірити збірку, PnP/службові сценарії та експорт |

## Межі безпеки

ToolNG не прошиває, не ремонтує, не скидає й не змінює стан пристрою; не читає приватний вміст резервних копій і не надсилає діагностичні дані до хмари. Будь-яка зміна системи виконується окремо й лише за рішенням оператора.

## Екосистема та ліцензія

**ToolNG** — канонічна назва репозиторію та інструмента. Суміжні проєкти: [SAMBU](https://github.com/bboroski/SAMBU), [ReaperCripple](https://github.com/bboroski/ReaperCripple), [Kloz](https://github.com/bboroski/Kloz), [CO](https://github.com/bboroski/CO), [SaintSI](https://github.com/bboroski/SaintSI), [Klipot](https://github.com/bboroski/Klipot), [NUN](https://github.com/bboroski/NUN), [NUNLauncher](https://github.com/bboroski/NUNLauncher), [Fragile-Baby](https://github.com/bboroski/Fragile-Baby) та [Luci](https://github.com/bboroski/Luci).

MIT — див. [LICENSE](LICENSE).

---

**Створено з любов'ю до надійних систем і людей.**