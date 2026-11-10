<p align="center"><img src="assets/duck-ui/duck-mark.svg" alt="ToolNG mark" width="92"></p>
<h1 align="center">ToolNG</h1>
<p align="center"><strong>Windows Device Evidence Probe</strong></p>
<p align="center"><img src="https://img.shields.io/badge/STATUS-WINDOWS%20DESKTOP-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/CORE-C17%20%2B%20WIN32-FFD600?style=for-the-badge&labelColor=111111"><img src="https://img.shields.io/badge/LICENSE-MIT-FFD600?style=for-the-badge&labelColor=111111"></p>
<p align="center"><a href="README.md">🇺🇦 Українська</a> · <a href="README.en.md">🇬🇧 English</a> · <a href="README.ru.md">🇷🇺 Русский</a></p>

---

**ToolNG — локальная диагностическая утилита Windows, которая отделяет подтверждённые сигналы устройства от неизвестного или непроверенного состояния.** Её ценность — прозрачная информация для оператора без автоматического вмешательства в систему.

## Возможности

- Обнаружение видимых интерфейсов устройств Windows PnP/USB.
- Проверка состояния соответствующих системных служб.
- Read-only просмотр локальных маркеров резервного копирования.
- Ограниченное наблюдение за событиями подключения и отключения.
- Объяснимая оценка готовности с отдельным показателем полноты доказательств.
- Локальные отчёты, журналы, экспорт и редактирование чувствительных полей.
- Рекомендации по следующим шагам без автоматического ремонта.

## Сборка и запуск

Требуются Windows 10/11, CMake и совместимый компилятор C17.

~~~powershell
cmake -S . -B build
cmake --build build --config Release
python one.py
python one.py check
~~~

В Windows также доступен <code>one.cmd</code>.

## Как читать результаты

ToolNG различает подтверждённый сигнал, завершённую проверку без находки, неизвестное состояние и ошибку проверки. Высокая оценка при неполном покрытии не представляется полной проверкой. Отчёты остаются локальными; аппаратные идентификаторы и длинные локальные пути можно скрывать. Контрольная сумма sidecar-файла помогает обнаружить последующее изменение отчёта, но не является цифровой подписью.

## Управление и выпуск

| Контроль | Правило |
|---|---|
| Платформа | Windows 10/11, C17, Win32 |
| Доказательства | Результат проверки и coverage показываются отдельно |
| Конфиденциальность | Отчёты локальные; чувствительные поля можно скрыть |
| Безопасные действия | Рекомендация GUI не означает, что операция выполнена |
| Релиз | Проверить сборку, PnP/служебные сценарии и экспорт отчётов |

## Границы безопасности

ToolNG не прошивает, не ремонтирует, не сбрасывает и не меняет состояние устройства; не читает приватное содержимое резервных копий и не передаёт диагностические данные в облако. Любое изменение системы остаётся отдельным действием, разрешённым оператором.

## Экосистема и лицензия

**ToolNG** — каноническое название репозитория и инструмента. Связанные проекты: [SAMBU](https://github.com/bboroski/SAMBU), [ReaperCripple](https://github.com/bboroski/ReaperCripple), [Kloz](https://github.com/bboroski/Kloz), [CO](https://github.com/bboroski/CO), [SaintSI](https://github.com/bboroski/SaintSI), [Klipot](https://github.com/bboroski/Klipot), [NUN](https://github.com/bboroski/NUN), [NUNLauncher](https://github.com/bboroski/NUNLauncher), [Fragile-Baby](https://github.com/bboroski/Fragile-Baby) и [Luci](https://github.com/bboroski/Luci).

MIT — см. [LICENSE](LICENSE).

---

**Создано с любовью к надёжным системам и людям.**