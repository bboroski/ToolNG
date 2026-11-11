<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="ToolNG mark" width="92"></p>
<h1 align="center">ToolNG</h1>
<p align="center"><strong>Windows Device Evidence Probe</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-WINDOWS%20DESKTOP%20UTILITY%20%C2%B7%20HOST-SIDE%20DIAGNOSTICS-FFD600?style=for-the-badge&labelColor=111111" alt="Windows desktop utility · host-side diagnostics"></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>
<sub>Оновлено: 2026-11-11</sub>


## Навігація

- [Поточні можливості](#поточні-можливості)
- [Швидкий старт і перевірка](#швидкий-старт-і-перевірка)
- [Архітектура, межі та достовірність](#архітектура-межі-та-достовірність)
- [Прев'ю інтерфейсу](#превю-інтерфейсу)
- [Інженерна якість](#інженерна-якість)
- [Карта документації](#карта-документації)
- [NUN Mesh та пов'язані репозиторії](#nun-mesh-та-повязані-репозиторії)
- [Дизайн і документація](#дизайн-і-документація)
- [Ліцензія](#ліцензія)

> **ToolNG** — ToolNG — локальний read-only інструмент діагностики Windows, який відокремлює підтверджені сигнали від невідомого стану та неповного покриття.

## Поточні можливості
- Огляд доступних Windows PnP/USB-пристроїв та пов'язаних служб.
- Read-only перевірка локальних маркерів резервного копіювання.
- Спостереження за подіями підключення/відключення, локальні звіти й журнали.
- Пояснювана оцінка готовності з окремим показником повноти доказів.
- Редагування чутливих полів і рекомендації оператору.

## Швидкий старт і перевірка
```bash
cmake -S . -B build
cmake --build build --config Release
python one.py check
```

## Архітектура, межі та достовірність
ToolNG не ремонтує пристрої, не змінює драйвери чи служби, не читає приватний вміст backup і не відправляє діагностику в хмару. Рекомендація не означає виконану дію.

## Прев'ю інтерфейсу

Це статичні векторні прев'ю інтерфейсу ToolNG, а не живий знімок запущеного сканування. Усі значення пристроїв і діагностики — демонстраційні SAMPLE DATA.

<p align="center"><img src="assets/screenshots/overview.svg" alt="ToolNG: оглядова панель, демонстраційні дані" width="960"></p>
<p align="center"><em>Оглядова панель</em></p>

<p align="center"><img src="assets/screenshots/usb-devices.svg" alt="ToolNG: список USB-пристроїв, демонстраційні дані" width="960"></p>
<p align="center"><em>USB-пристрої</em></p>

<p align="center"><img src="assets/screenshots/reports.svg" alt="ToolNG: локальні звіти, демонстраційні дані" width="960"></p>
<p align="center"><em>Локальні звіти</em></p>

## Інженерна якість
- Перевіряйте збірку й тести саме для цільової платформи; host CI не замінює hardware qualification.
- Статуси **implemented**, **partial**, **planned** мають підтверджуватися кодом, тестами та документацією.
- Не оголошуйте функцію production-ready лише на підставі макета, специфікації чи відкритого Pull Request.
- Зовнішні інтеграції мають бути явними, версіонованими, ізольованими й перевірюваними.

## Карта документації

- [Збірка та перевірка](docs/BUILDING.md)
- [Безпека й межі застосування](docs/SAFETY_AND_SCOPE.md)
- [Проєктування GUI](docs/GUI_DESIGN.md)
- [Система Duck UI](docs/DUCK_UI_SYSTEM.md)

## NUN Mesh та пов'язані репозиторії

<p align="center"><img src="assets/nun-mesh.svg" alt="NUN Mesh architecture diagram showing independent repositories connected by versioned contracts" width="960"></p>
[SAMBU](https://github.com/bboroski/SAMBU) · [ToolNG](https://github.com/bboroski/ToolNG) · [ReaperCripple](https://github.com/bboroski/ReaperCripple) · [Kloz](https://github.com/bboroski/Kloz) · [CO](https://github.com/bboroski/CO) · [SaintSI](https://github.com/bboroski/SaintSI) · [Klipot](https://github.com/bboroski/Klipot) · [NUN](https://github.com/bboroski/NUN) · [NUNLauncher](https://github.com/bboroski/NUNLauncher)

Репозиторії зберігають власні назви, код, API, цикли випуску та межі відповідальності. Інтеграція означає узгоджені контракти, а не автоматичне злиття незалежних runtime.

## Дизайн і документація
Базовий інтерфейсний напрям екосистеми — **Duck Black / Yellow**: чорні поверхні, жовтий акцент для дій і станів, читабельна типографіка, доступний контраст, клавіатурна навігація та підтримка reduced motion. Деталі дивіться в [design/DUCK_UI_SYSTEM.md](design/DUCK_UI_SYSTEM.md), якщо файл присутній у цьому репозиторії.

## Ліцензія
MIT

---
**Створено з любов'ю до інженерії, системності та людей.**


## Графічний інтерфейс Diagnostic Console

Доступні [прототип консолі діагностики ToolNG](docs/gui/index.html) та [векторне прев'ю 1600 × 900](docs/gui/preview.svg). Інтерфейс показує інвентаризацію Windows-пристроїв, часову шкалу підключень, read-only маркери резервних копій та звіти з поясненням доказів. **Усі пристрої, лічильники й події — демонстраційні дані; прототип не звертається до обладнання Windows і не змінює систему.**

Інтерфейс адаптивний, підтримує клавіатурну навігацію та reduced motion. Дивіться [проєктування GUI](docs/GUI_DESIGN.md) і [межі безпеки](docs/SAFETY_AND_SCOPE.md).
