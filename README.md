<p align="center">
  <img src="assets/duck-ui/duck-mark.svg" alt="Duck mark" width="104" />
</p>
<h1 align="center">ToolNG</h1>
<p align="center"><strong>Windows Device Evidence Probe</strong></p>
<p align="center">
  <img src="https://img.shields.io/badge/STATUS-WINDOWS%20DESKTOP-FFD600?style=for-the-badge&labelColor=111111" alt="Project status" />
  <img src="https://img.shields.io/badge/CORE-C17%20%2B%20WIN32-FFD600?style=for-the-badge&labelColor=111111" alt="Core technology" />
  <img src="https://img.shields.io/badge/LICENSE-MIT-FFD600?style=for-the-badge&labelColor=111111" alt="MIT license" />
</p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<hr />

**Локальна діагностика Windows, яка показує, що саме підтверджено сигналами пристрою, а що залишається невідомим.**

## Продуктова роль

ToolNG — нативний C17/Win32-зонд для інженерів підтримки, адміністраторів і команд, яким потрібна обережна перевірка підключення пристроїв без автоматичного втручання в систему. Його головна цінність — прозорі докази, а не магічний «зелений статус».

## Можливості

- Виявлення видимих Windows PnP/USB-інтерфейсів пристроїв.
- Перевірка стану відповідної системної служби.
- Read-only огляд локальних маркерів резервних копій.
- Обмежене спостереження за подіями підключення та відключення.
- Пояснювана оцінка готовності з окремим показником покриття доказами.
- Локальні звіти, журнали, експорт і редагування чутливих полів.
- Рекомендації з безпечних наступних кроків без автоматичного ремонту.

## Збірка та запуск

Потрібні Windows 10/11, CMake і сумісний C17-компілятор.

~~~powershell
cmake -S . -B build
cmake --build build --config Release
~~~

Єдина точка запуску та перевірки:

~~~powershell
python one.py
python one.py check
~~~

У Windows також доступний файл <code>one.cmd</code>.

## Модель доказів

ToolNG розрізняє підтверджений сигнал, завершену перевірку без знахідки, невідомий стан і помилку перевірки. Висока оцінка за неповного coverage не подається як повне підтвердження.

Звіти зберігаються локально. Апаратні ідентифікатори та довгі локальні шляхи можуть бути приховані. Контрольна сума sidecar-файла допомагає виявити подальшу зміну звіту, але не є цифровим підписом.

## Управління проєктом і критерії випуску

| Контрольна точка | Правило |
|---|---|
| Платформа | Windows 10/11, C17, Win32 |
| Докази | Окремо показувати результат перевірки та повноту покриття |
| Конфіденційність | Звіти локальні; чутливі поля можна редагувати |
| Безпечна дія | Рекомендація в GUI не означає виконану операцію |
| Випуск | Перевірити збірку, сценарії PnP/служб і коректність експорту звітів |

## Межі безпеки

ToolNG не прошиває, не ремонтує, не скидає й не змінює стан пристрою; не читає приватний вміст backup; не надсилає діагностичні дані до хмари. Рішення про будь-яку зміну системи залишається за оператором.

## Технічна ідентичність

**ToolNG** — канонічна назва Windows-інструмента. Публічне ядро policy/workflow використовує типи <code>ToolNG*</code>, функції <code>toolng_*</code> і константи <code>ToolNG_*</code>. Історичні шляхи файлів і старі CMake-цілі збережені лише для сумісності.

## Екосистема

[ SAMBU ](https://github.com/bboroski/SAMBU) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUN](https://github.com/bboroski/NUN) · [NUNLauncher](https://github.com/bboroski/NUNLauncher).

## Ліцензія

MIT — див. [LICENSE](LICENSE).

## Академический биологический словарь

- [500 академических терминов EN–RU](data/biology/academic_terms_500.tsv)
- [Контракт и правила использования](docs/BIOLOGY_DOMAIN_PACK.md)

Справочный офлайн-набор для обучения и метаданных; он не управляет системами, устройствами и диагностикой.
