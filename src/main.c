#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WIN32_WINNT 0x0A00

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <setupapi.h>
#include <winsvc.h>
#include <commctrl.h>
#include <dbt.h>
#include <usbiodef.h>

/* Local GUID definition avoids a uuid.lib dependency for this interface class. */
const GUID GUID_DEVINTERFACE_USB_DEVICE =
    { 0xA5DCBF10L, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } };
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#define APP_TITLE           L"ToolzNG"
#define MAX_APPLE_DEVICES   32
#define DEVICE_NAME_CHARS   256
#define DEVICE_HWID_CHARS   512
#define DEVICE_MAKER_CHARS  128
#define REPORT_CHARS        18000
#define USB_EVENT_CAPACITY  64
#define USB_STABILITY_WINDOW_MS (10ULL * 60ULL * 1000ULL)
#define USB_STABILITY_WARMUP_MS 30000ULL
#define TIMER_USB_REFRESH   42
#define TIMER_READINESS      43

#define COL_BG              RGB(13, 17, 24)
#define COL_HEADER          RGB(17, 23, 32)
#define COL_SIDEBAR         RGB(18, 24, 34)
#define COL_PANEL           RGB(23, 31, 43)
#define COL_CARD            RGB(27, 37, 51)
#define COL_CARD_HOVER      RGB(34, 47, 64)
#define COL_BORDER          RGB(49, 65, 86)
#define COL_TEXT            RGB(235, 241, 249)
#define COL_MUTED           RGB(151, 166, 186)
#define COL_ACCENT          RGB(92, 157, 255)
#define COL_ACCENT_DARK     RGB(35, 83, 151)
#define COL_GREEN           RGB(83, 203, 151)
#define COL_AMBER           RGB(245, 183, 84)

#define PAGE_OVERVIEW 0
#define PAGE_DEVICES  1
#define PAGE_CARE     2
#define PAGE_ABOUT    3

#define ACTION_NONE       0
#define ACTION_SCAN       1
#define ACTION_REPORT     2
#define ACTION_DEVICES    3
#define ACTION_BACKUPS    4
#define ACTION_CARE       5
#define ACTION_SUPPORT    6
#define ACTION_STORE      7

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

typedef struct AppleDevice {
    WCHAR name[DEVICE_NAME_CHARS];
    WCHAR hardware_id[DEVICE_HWID_CHARS];
    WCHAR manufacturer[DEVICE_MAKER_CHARS];
} AppleDevice;

typedef struct UsbPulseEvent {
    ULONGLONG tick;
    BOOL arrival;
} UsbPulseEvent;

typedef struct AppState {
    HWND hwnd;
    int page;
    int client_width;
    int client_height;
    BOOL scan_complete;
    UINT device_count;
    AppleDevice devices[MAX_APPLE_DEVICES];
    WCHAR status[320];
    WCHAR service_status[320];
    WCHAR backup_path[MAX_PATH * 2];
    BOOL backup_found;
    BOOL apple_service_running;
    BOOL usb_watch_registered;
    HDEVNOTIFY usb_notification;
    ULONGLONG app_started_at;
    UsbPulseEvent usb_events[USB_EVENT_CAPACITY];
    UINT usb_event_count;
    UINT usb_event_cursor;
    UINT usb_arrivals_total;
    UINT usb_removals_total;
    DWORD backup_folder_count;
    DWORD backup_valid_count;
    WCHAR backup_audit[320];
    int readiness_score;
    WCHAR coach_title[180];
    WCHAR coach_body[420];
    HFONT font_brand;
    HFONT font_title;
    HFONT font_heading;
    HFONT font_body;
    HFONT font_small;
    HFONT font_button;
    RECT nav[4];
    RECT scan_button;
    RECT report_button;
    RECT devices_button;
    RECT backups_button;
    RECT care_button;
    RECT support_button;
    RECT store_button;
} AppState;

static AppState g_app;

static void copy_text(WCHAR *dst, size_t dst_count, const WCHAR *src)
{
    if (!dst || dst_count == 0) return;
    if (!src) src = L"";
    wcsncpy_s(dst, dst_count, src, _TRUNCATE);
}

static void draw_text(HDC dc, const WCHAR *text, RECT rect, HFONT font,
                      COLORREF color, UINT flags)
{
    HFONT old_font = NULL;
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    if (font) old_font = (HFONT)SelectObject(dc, font);
    DrawTextW(dc, text ? text : L"", -1, &rect, flags | DT_NOPREFIX);
    if (old_font) SelectObject(dc, old_font);
}

static void draw_round_rect(HDC dc, RECT rect, COLORREF fill,
                            COLORREF border, int radius)
{
    HBRUSH brush = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, 1, border);
    HGDIOBJ old_brush = SelectObject(dc, brush);
    HGDIOBJ old_pen = SelectObject(dc, pen);
    RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);
    SelectObject(dc, old_brush);
    SelectObject(dc, old_pen);
    DeleteObject(brush);
    DeleteObject(pen);
}

static void draw_button(HDC dc, RECT rect, const WCHAR *label, BOOL primary)
{
    COLORREF fill = primary ? COL_ACCENT_DARK : COL_CARD;
    COLORREF edge = primary ? COL_ACCENT : COL_BORDER;
    draw_round_rect(dc, rect, fill, edge, 10);
    draw_text(dc, label, rect, g_app.font_button, COL_TEXT,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

static void set_status(const WCHAR *message)
{
    copy_text(g_app.status, ARRAYSIZE(g_app.status), message);
    if (g_app.hwnd) InvalidateRect(g_app.hwnd, NULL, FALSE);
}

static BOOL get_device_property(HDEVINFO set, SP_DEVINFO_DATA *device,
                                DWORD property, WCHAR *buffer,
                                DWORD buffer_bytes)
{
    DWORD type = 0;
    DWORD required = 0;
    if (!buffer || buffer_bytes < sizeof(WCHAR)) return FALSE;
    ZeroMemory(buffer, buffer_bytes);
    return SetupDiGetDeviceRegistryPropertyW(set, device, property, &type,
             (PBYTE)buffer, buffer_bytes, &required) != FALSE;
}

static BOOL is_apple_hardware_id(const WCHAR *multi_string, WCHAR *matched_id,
                                 size_t matched_count)
{
    const WCHAR *entry = multi_string;
    while (entry && *entry) {
        if (wcsstr(entry, L"VID_05AC") != NULL) {
            copy_text(matched_id, matched_count, entry);
            return TRUE;
        }
        entry += wcslen(entry) + 1;
    }
    return FALSE;
}

static void scan_apple_devices(void)
{
    HDEVINFO set;
    SP_DEVINFO_DATA device;
    DWORD index = 0;

    ZeroMemory(g_app.devices, sizeof(g_app.devices));
    g_app.device_count = 0;

    set = SetupDiGetClassDevsW(NULL, NULL, NULL,
                               DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (set == INVALID_HANDLE_VALUE) {
        set_status(L"Не удалось прочитать список USB/PnP-устройств Windows.");
        g_app.scan_complete = FALSE;
        return;
    }

    ZeroMemory(&device, sizeof(device));
    device.cbSize = sizeof(device);

    while (SetupDiEnumDeviceInfo(set, index++, &device)) {
        WCHAR hardware_ids[4096 / sizeof(WCHAR)];
        WCHAR matched_id[DEVICE_HWID_CHARS];
        WCHAR name[DEVICE_NAME_CHARS];
        WCHAR maker[DEVICE_MAKER_CHARS];

        ZeroMemory(hardware_ids, sizeof(hardware_ids));
        ZeroMemory(matched_id, sizeof(matched_id));
        ZeroMemory(name, sizeof(name));
        ZeroMemory(maker, sizeof(maker));

        if (!get_device_property(set, &device, SPDRP_HARDWAREID,
                                 hardware_ids, sizeof(hardware_ids))) {
            continue;
        }
        if (!is_apple_hardware_id(hardware_ids, matched_id,
                                  ARRAYSIZE(matched_id))) {
            continue;
        }

        if (g_app.device_count >= MAX_APPLE_DEVICES) break;

        if (!get_device_property(set, &device, SPDRP_FRIENDLYNAME,
                                 name, sizeof(name))) {
            (void)get_device_property(set, &device, SPDRP_DEVICEDESC,
                                      name, sizeof(name));
        }
        (void)get_device_property(set, &device, SPDRP_MFG,
                                  maker, sizeof(maker));

        if (name[0] == L'\0') copy_text(name, ARRAYSIZE(name), L"Apple USB device");

        copy_text(g_app.devices[g_app.device_count].name,
                  ARRAYSIZE(g_app.devices[g_app.device_count].name), name);
        copy_text(g_app.devices[g_app.device_count].hardware_id,
                  ARRAYSIZE(g_app.devices[g_app.device_count].hardware_id), matched_id);
        copy_text(g_app.devices[g_app.device_count].manufacturer,
                  ARRAYSIZE(g_app.devices[g_app.device_count].manufacturer), maker);
        g_app.device_count++;
    }

    SetupDiDestroyDeviceInfoList(set);
    g_app.scan_complete = TRUE;

    if (g_app.device_count == 0) {
        set_status(L"Apple USB-устройства не найдены. Разблокируй iPhone и проверь кабель и запрос доверия.");
    } else {
        WCHAR message[160];
        swprintf_s(message, ARRAYSIZE(message),
                   L"Сканирование завершено. Найдено устройств Apple: %u.",
                   g_app.device_count);
        set_status(message);
    }
}

static void read_apple_service_status(void)
{
    SC_HANDLE manager = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
    SC_HANDLE service = NULL;
    SERVICE_STATUS_PROCESS status;
    DWORD bytes_needed = 0;
    const WCHAR *service_names[] = {
        L"Apple Mobile Device Service",
        L"AppleMobileDeviceService"
    };
    size_t i;

    g_app.apple_service_running = FALSE;
    copy_text(g_app.service_status, ARRAYSIZE(g_app.service_status),
              L"Служба Apple не обнаружена.");

    if (!manager) {
        copy_text(g_app.service_status, ARRAYSIZE(g_app.service_status),
                  L"Статус службы недоступен (нет доступа к Service Control Manager).");
        return;
    }

    for (i = 0; i < ARRAYSIZE(service_names); ++i) {
        service = OpenServiceW(manager, service_names[i], SERVICE_QUERY_STATUS);
        if (service) break;
    }

    if (service) {
        ZeroMemory(&status, sizeof(status));
        if (QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO,
                (LPBYTE)&status, sizeof(status), &bytes_needed)) {
            switch (status.dwCurrentState) {
                case SERVICE_RUNNING:
                    g_app.apple_service_running = TRUE;
                    copy_text(g_app.service_status, ARRAYSIZE(g_app.service_status),
                              L"Служба Apple Mobile Device Service запущена.");
                    break;
                case SERVICE_STOPPED:
                    copy_text(g_app.service_status, ARRAYSIZE(g_app.service_status),
                              L"Служба Apple Mobile Device Service остановлена.");
                    break;
                case SERVICE_START_PENDING:
                    copy_text(g_app.service_status, ARRAYSIZE(g_app.service_status),
                              L"Служба Apple Mobile Device Service запускается.");
                    break;
                default:
                    copy_text(g_app.service_status, ARRAYSIZE(g_app.service_status),
                              L"Служба Apple Mobile Device Service обнаружена.");
                    break;
            }
        } else {
            copy_text(g_app.service_status, ARRAYSIZE(g_app.service_status),
                      L"Служба Apple найдена, но её статус прочитать не удалось.");
        }
        CloseServiceHandle(service);
    }
    CloseServiceHandle(manager);
}

static BOOL path_is_directory(const WCHAR *path)
{
    DWORD attributes = GetFileAttributesW(path);
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

static void append_path(WCHAR *destination, size_t capacity,
                        const WCHAR *base, const WCHAR *suffix)
{
    copy_text(destination, capacity, base);
    if (destination[0] == L'\0') return;
    if (destination[wcslen(destination) - 1] != L'\\') {
        wcscat_s(destination, capacity, L"\\");
    }
    wcscat_s(destination, capacity, suffix);
}

static BOOL find_backup_folder(WCHAR *result, size_t result_count)
{
    PWSTR roaming = NULL;
    PWSTR profile = NULL;
    WCHAR candidates[6][MAX_PATH * 2];
    size_t count = 0;
    size_t i;

    if (!result || result_count == 0) return FALSE;
    result[0] = L'\0';
    ZeroMemory(candidates, sizeof(candidates));

    if (SUCCEEDED(SHGetKnownFolderPath(&FOLDERID_RoamingAppData, 0, NULL, &roaming))) {
        append_path(candidates[count++], ARRAYSIZE(candidates[0]), roaming,
                    L"Apple Computer\\MobileSync\\Backup");
        append_path(candidates[count++], ARRAYSIZE(candidates[0]), roaming,
                    L"Apple\\MobileSync\\Backup");
    }
    if (SUCCEEDED(SHGetKnownFolderPath(&FOLDERID_Profile, 0, NULL, &profile))) {
        append_path(candidates[count++], ARRAYSIZE(candidates[0]), profile,
                    L"Apple\\MobileSync\\Backup");
        append_path(candidates[count++], ARRAYSIZE(candidates[0]), profile,
                    L"Apple Computer\\MobileSync\\Backup");
    }

    for (i = 0; i < count; ++i) {
        if (path_is_directory(candidates[i])) {
            copy_text(result, result_count, candidates[i]);
            if (roaming) CoTaskMemFree(roaming);
            if (profile) CoTaskMemFree(profile);
            return TRUE;
        }
    }

    if (roaming) CoTaskMemFree(roaming);
    if (profile) CoTaskMemFree(profile);
    return FALSE;
}


static BOOL backup_file_exists(const WCHAR *directory, const WCHAR *filename)
{
    WCHAR path[MAX_PATH * 2];
    DWORD attributes;
    append_path(path, ARRAYSIZE(path), directory, filename);
    attributes = GetFileAttributesW(path);
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

/* Read-only structural sentinel: marker files only; backup payloads are never opened. */
static void audit_backup_folder(void)
{
    WCHAR search_path[MAX_PATH * 2];
    WIN32_FIND_DATAW data;
    HANDLE find;
    DWORD folders = 0, valid = 0;

    g_app.backup_folder_count = 0;
    g_app.backup_valid_count = 0;
    if (!g_app.backup_found || g_app.backup_path[0] == L'\0') {
        copy_text(g_app.backup_audit, ARRAYSIZE(g_app.backup_audit),
                  L"Backup Sentinel: каталог резервных копий не найден.");
        return;
    }
    append_path(search_path, ARRAYSIZE(search_path), g_app.backup_path, L"*");
    find = FindFirstFileW(search_path, &data);
    if (find == INVALID_HANDLE_VALUE) {
        copy_text(g_app.backup_audit, ARRAYSIZE(g_app.backup_audit),
                  L"Backup Sentinel: не удалось прочитать список папок.");
        return;
    }
    do {
        WCHAR child[MAX_PATH * 2];
        BOOL has_info, has_manifest;
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
        if (wcscmp(data.cFileName, L".") == 0 || wcscmp(data.cFileName, L"..") == 0) continue;
        ++folders;
        append_path(child, ARRAYSIZE(child), g_app.backup_path, data.cFileName);
        has_info = backup_file_exists(child, L"Info.plist");
        has_manifest = backup_file_exists(child, L"Manifest.db") ||
                       backup_file_exists(child, L"Manifest.plist");
        if (has_info && has_manifest) ++valid;
    } while (FindNextFileW(find, &data));
    FindClose(find);

    g_app.backup_folder_count = folders;
    g_app.backup_valid_count = valid;
    if (folders == 0) {
        copy_text(g_app.backup_audit, ARRAYSIZE(g_app.backup_audit),
                  L"Backup Sentinel: папка найдена, но резервных копий внутри нет.");
    } else {
        swprintf_s(g_app.backup_audit, ARRAYSIZE(g_app.backup_audit),
                   L"Backup Sentinel: %lu из %lu папок имеют Info.plist и Manifest.",
                   (unsigned long)valid, (unsigned long)folders);
    }
}

static void record_usb_pulse(BOOL arrival)
{
    UsbPulseEvent *event = &g_app.usb_events[g_app.usb_event_cursor];
    event->tick = GetTickCount64();
    event->arrival = arrival;
    g_app.usb_event_cursor = (g_app.usb_event_cursor + 1U) % USB_EVENT_CAPACITY;
    if (g_app.usb_event_count < USB_EVENT_CAPACITY) ++g_app.usb_event_count;
    if (arrival) ++g_app.usb_arrivals_total;
    else ++g_app.usb_removals_total;

    set_status(arrival
        ? L"USB Pulse: обнаружено подключение интерфейса Apple. Обновляю диагностику..."
        : L"USB Pulse: интерфейс Apple отключился. Проверяю стабильность соединения...");
    if (g_app.hwnd) SetTimer(g_app.hwnd, TIMER_USB_REFRESH, 650, NULL);
}

static void get_recent_usb_counts(UINT *arrivals, UINT *removals)
{
    ULONGLONG now = GetTickCount64();
    UINT a = 0, r = 0, i;
    for (i = 0; i < g_app.usb_event_count; ++i) {
        const UsbPulseEvent *event = &g_app.usb_events[i];
        if (event->tick != 0 && now >= event->tick &&
            now - event->tick <= USB_STABILITY_WINDOW_MS) {
            if (event->arrival) ++a;
            else ++r;
        }
    }
    if (arrivals) *arrivals = a;
    if (removals) *removals = r;
}

static void register_usb_watch(HWND hwnd)
{
    DEV_BROADCAST_DEVICEINTERFACE_W filter;
    ZeroMemory(&filter, sizeof(filter));
    filter.dbcc_size = sizeof(filter);
    filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    filter.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;
    g_app.usb_notification = RegisterDeviceNotificationW(
        hwnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);
    g_app.usb_watch_registered = (g_app.usb_notification != NULL);
}

static void compute_readiness_and_coach(void)
{
    UINT removals = 0;
    ULONGLONG elapsed = GetTickCount64() - g_app.app_started_at;
    int score = 0;
    get_recent_usb_counts(NULL, &removals);

    /* Explainable score: only measurable Windows-side signals contribute. */
    if (g_app.device_count > 0) score += 40;
    if (g_app.apple_service_running) score += 25;
    if (g_app.backup_found) score += 10;
    if (g_app.backup_valid_count > 0) score += 15;
    if (g_app.usb_watch_registered && g_app.device_count > 0 &&
        elapsed >= USB_STABILITY_WARMUP_MS && removals == 0) score += 10;
    if (score > 100) score = 100;
    g_app.readiness_score = score;

    if (g_app.device_count == 0) {
        copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                  L"DuckFix: начни с физического подключения");
        copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                  L"Используй кабель с передачей данных, разблокируй iPhone и подтверди «Доверять этому компьютеру». Затем нажми «Сканировать USB». ToolzNG не может подтвердить доверие iOS напрямую.");
    } else if (!g_app.apple_service_running) {
        copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                  L"DuckFix: проверь компонент Apple для Windows");
        copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                  L"Windows видит USB-интерфейс, но служба Apple не подтверждена как запущенная. Установи или обнови Apple Devices; если служба установлена, проверь её состояние в приложении «Службы».");
    } else if (removals > 0) {
        copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                  L"DuckFix: проверь стабильность USB-линии");
        copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                  L"USB Pulse зарегистрировал отключения Apple-интерфейса за последние 10 минут. Попробуй другой порт без USB-хаба и исправный кабель передачи данных; наблюдай за счётчиком повторных отключений.");
    } else if (g_app.backup_folder_count > 0 && g_app.backup_valid_count == 0) {
        copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                  L"DuckFix: проверь структуру локальной копии");
        copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                  L"Backup Sentinel не нашёл ожидаемые маркеры Info.plist и Manifest.db/Manifest.plist. Не удаляй папки: создай новую копию через Apple Devices или iTunes и сравни результат.");
    } else if (!g_app.backup_found || g_app.backup_valid_count == 0) {
        copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                  L"DuckFix: создай резервную копию");
        copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                  L"Подключение Apple обнаружено. Для защиты данных создай локальную резервную копию штатным приложением Apple Devices или iTunes. ToolzNG не создаёт и не изменяет backup самостоятельно.");
    } else {
        copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                  L"DuckFix: базовые проверки пройдены");
        copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                  L"Windows видит Apple USB, служба доступна, а структура резервной копии выглядит ожидаемой. Это не тест скорости iOS и не проверка содержимого backup; продолжай пользоваться штатными средствами Apple.");
    }
}

static void refresh_diagnostics(void)
{
    scan_apple_devices();
    read_apple_service_status();
    g_app.backup_found = find_backup_folder(g_app.backup_path,
                                             ARRAYSIZE(g_app.backup_path));
    audit_backup_folder();
    compute_readiness_and_coach();
    InvalidateRect(g_app.hwnd, NULL, FALSE);
}

static void open_backup_folder(void)
{
    if (!g_app.backup_found) {
        (void)MessageBoxW(g_app.hwnd,
            L"Стандартная папка локальных резервных копий не найдена.\n\n"
            L"Создай резервную копию через приложение Apple Devices или iTunes — после этого ToolzNG сможет открыть её расположение.\n\n"
            L"ToolzNG ничего не удаляет из резервных копий.",
            APP_TITLE, MB_OK | MB_ICONINFORMATION);
        (void)ShellExecuteW(g_app.hwnd, L"open",
            L"https://support.apple.com/en-us/108809", NULL, NULL, SW_SHOWNORMAL);
        return;
    }

    (void)ShellExecuteW(g_app.hwnd, L"open", g_app.backup_path,
                        NULL, NULL, SW_SHOWNORMAL);
}

static BOOL write_utf8_file(const WCHAR *path, const WCHAR *contents)
{
    HANDLE file;
    int bytes_needed;
    char *utf8;
    DWORD written = 0;
    BOOL ok;

    bytes_needed = WideCharToMultiByte(CP_UTF8, 0, contents, -1,
                                       NULL, 0, NULL, NULL);
    if (bytes_needed <= 1) return FALSE;

    utf8 = (char *)malloc((size_t)bytes_needed);
    if (!utf8) return FALSE;

    if (!WideCharToMultiByte(CP_UTF8, 0, contents, -1,
                             utf8, bytes_needed, NULL, NULL)) {
        free(utf8);
        return FALSE;
    }

    file = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        free(utf8);
        return FALSE;
    }

    ok = WriteFile(file, utf8, (DWORD)(bytes_needed - 1), &written, NULL) &&
         written == (DWORD)(bytes_needed - 1);
    CloseHandle(file);
    free(utf8);
    return ok;
}

static void save_report(void)
{
    PWSTR documents = NULL;
    WCHAR report_path[MAX_PATH * 2];
    WCHAR report[REPORT_CHARS];
    WCHAR backup_display[MAX_PATH * 2];
    SYSTEMTIME now;
    size_t used = 0;
    UINT i;
    UINT recent_arrivals = 0, recent_removals = 0;
    int count;
    int detail_choice;
    BOOL include_technical_details;
    WCHAR preview[1200];

    if (!g_app.scan_complete) refresh_diagnostics();
    get_recent_usb_counts(&recent_arrivals, &recent_removals);

    detail_choice = MessageBoxW(g_app.hwnd,
        L"Выбери уровень приватности отчёта:\n\n"
        L"ДА — полный технический отчёт: Hardware ID и точный путь к папке backup.\n"
        L"НЕТ — приватный отчёт: чувствительные идентификаторы и точный путь будут скрыты.\n"
        L"ОТМЕНА — ничего не сохранять.\n\n"
        L"По умолчанию безопаснее выбрать «НЕТ».",
        L"ToolzNG Privacy Shield", MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2);
    if (detail_choice == IDCANCEL) return;
    include_technical_details = (detail_choice == IDYES);

    if (FAILED(SHGetKnownFolderPath(&FOLDERID_Documents, KF_FLAG_CREATE,
                                    NULL, &documents))) {
        (void)MessageBoxW(g_app.hwnd,
            L"Не удалось определить папку Documents.", APP_TITLE,
            MB_OK | MB_ICONERROR);
        return;
    }

    GetLocalTime(&now);
    swprintf_s(report_path, ARRAYSIZE(report_path),
        L"%ls\\ToolzNG-report-%04u%02u%02u-%02u%02u%02u.txt",
        documents, now.wYear, now.wMonth, now.wDay,
        now.wHour, now.wMinute, now.wSecond);
    CoTaskMemFree(documents);

    if (g_app.backup_found) {
        copy_text(backup_display, ARRAYSIZE(backup_display), g_app.backup_path);
    } else {
        copy_text(backup_display, ARRAYSIZE(backup_display),
                  L"Стандартная папка не найдена");
    }

    count = swprintf_s(report, ARRAYSIZE(report),
        L"ToolzNG — диагностический отчёт\r\n"
        L"================================\r\n"
        L"Дата: %04u-%02u-%02u %02u:%02u:%02u\r\n"
        L"ОС: Windows\r\n\r\n"
        L"USB-устройства Apple (VID_05AC): %u\r\n",
        now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute,
        now.wSecond, g_app.device_count);
    if (count < 0) {
        (void)MessageBoxW(g_app.hwnd, L"Не удалось сформировать отчёт.",
                          APP_TITLE, MB_OK | MB_ICONERROR);
        return;
    }
    used = (size_t)count;

    for (i = 0; i < g_app.device_count; ++i) {
        count = swprintf_s(report + used, ARRAYSIZE(report) - used,
            include_technical_details
              ? L"\r\n[%u] %ls\r\n    Производитель: %ls\r\n    Hardware ID: %ls\r\n"
              : L"\r\n[%u] %ls\r\n    Производитель: %ls\r\n    Hardware ID: [СКРЫТО ПРИВАТНЫМ РЕЖИМОМ]\r\n",
            i + 1, g_app.devices[i].name,
            g_app.devices[i].manufacturer[0] ? g_app.devices[i].manufacturer : L"не указан",
            include_technical_details ? g_app.devices[i].hardware_id : L"[скрыто]");
        if (count < 0) break;
        used += (size_t)count;
    }

    count = swprintf_s(report + used, ARRAYSIZE(report) - used,
        L"\r\nСлужба Apple: %ls\r\n"
        L"Папка резервных копий: %ls\r\n"
        L"Backup Sentinel: %ls\r\n"
        L"Duck Readiness: %d/100 (оценка Windows-side сигналов)\r\n"
        L"USB Pulse за 10 минут: %u подключений / %u отключений\r\n\r\n"
        L"Ограничения отчёта\r\n"
        L"------------------\r\n"
        L"Отчёт читает только сведения Windows PnP и статус службы.\r\n"
        L"Он не читает личные данные iPhone/iPad, не изменяет прошивку,\r\n"
        L"не удаляет файлы и не выполняет операции сброса или jailbreak.\r\n",
        g_app.service_status,
        include_technical_details ? backup_display : L"[СКРЫТО ПРИВАТНЫМ РЕЖИМОМ]",
        g_app.backup_audit, g_app.readiness_score,
        recent_arrivals, recent_removals);
    if (count < 0) {
        (void)MessageBoxW(g_app.hwnd, L"Не удалось сформировать отчёт.",
                          APP_TITLE, MB_OK | MB_ICONERROR);
        return;
    }

    swprintf_s(preview, ARRAYSIZE(preview),
        L"ПРЕДПРОСМОТР СОСТАВА ОТЧЁТА\n\n"
        L"Apple USB-интерфейсов: %u\n"
        L"Duck Readiness: %d/100\n"
        L"Backup Sentinel: %lu/%lu структурно отмеченных папок\n"
        L"Режим: %ls\n"
        L"Hardware ID: %ls\n"
        L"Точный путь backup: %ls\n\n"
        L"Сохранить TXT-отчёт в Documents?",
        g_app.device_count, g_app.readiness_score,
        (unsigned long)g_app.backup_valid_count,
        (unsigned long)g_app.backup_folder_count,
        include_technical_details ? L"полный технический" : L"приватный",
        include_technical_details ? L"будет включён" : L"будет скрыт",
        include_technical_details ? L"будет включён" : L"будет скрыт");
    if (MessageBoxW(g_app.hwnd, preview, L"ToolzNG — предварительный просмотр",
                    MB_YESNO | MB_ICONINFORMATION | MB_DEFBUTTON2) != IDYES) {
        set_status(L"Сохранение отчёта отменено пользователем.");
        return;
    }

    if (!write_utf8_file(report_path, report)) {
        (void)MessageBoxW(g_app.hwnd,
            L"Не удалось сохранить отчёт. Проверь права доступа к Documents.",
            APP_TITLE, MB_OK | MB_ICONERROR);
        return;
    }

    (void)MessageBoxW(g_app.hwnd, report_path,
                      L"Отчёт ToolzNG сохранён", MB_OK | MB_ICONINFORMATION);
}

static void open_apple_support(void)
{
    (void)ShellExecuteW(g_app.hwnd, L"open",
        L"https://support.apple.com/ru-ru/102598", NULL, NULL, SW_SHOWNORMAL);
}

static void open_apple_devices_store(void)
{
    (void)ShellExecuteW(g_app.hwnd, L"open",
        L"ms-windows-store://search/?query=Apple%20Devices",
        NULL, NULL, SW_SHOWNORMAL);
}

static void draw_header(HDC dc, int width)
{
    RECT header = { 0, 0, width, 88 };
    RECT logo = { 28, 22, 66, 60 };
    RECT status = { width - 220, 28, width - 28, 60 };
    HBRUSH brush = CreateSolidBrush(COL_HEADER);
    FillRect(dc, &header, brush);
    DeleteObject(brush);

    draw_round_rect(dc, logo, COL_ACCENT_DARK, COL_ACCENT, 11);
    draw_text(dc, L"T", logo, g_app.font_brand, COL_TEXT,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"ToolzNG", (RECT){ 80, 20, 260, 55 },
              g_app.font_title, COL_TEXT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"APPLE DEVICE DIAGNOSTICS", (RECT){ 82, 54, 360, 74 },
              g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    draw_round_rect(dc, status, COL_PANEL, COL_BORDER, 18);
    draw_text(dc, L"●  USB SCAN", status, g_app.font_small,
              g_app.scan_complete ? COL_GREEN : COL_MUTED,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    HPEN line = CreatePen(PS_SOLID, 1, COL_BORDER);
    HGDIOBJ old = SelectObject(dc, line);
    MoveToEx(dc, 0, 87, NULL);
    LineTo(dc, width, 87);
    SelectObject(dc, old);
    DeleteObject(line);
}

static void draw_sidebar(HDC dc)
{
    RECT side = { 0, 88, 224, g_app.client_height };
    HBRUSH brush = CreateSolidBrush(COL_SIDEBAR);
    int i;
    const WCHAR *labels[] = {
        L"Обзор",
        L"USB-устройства",
        L"Оптимизация",
        L"О программе"
    };
    const WCHAR *hints[] = {
        L"Панель состояния",
        L"Поиск Apple VID_05AC",
        L"Безопасные проверки",
        L"Версия и ограничения"
    };

    FillRect(dc, &side, brush);
    DeleteObject(brush);

    draw_text(dc, L"РАЗДЕЛЫ", (RECT){ 26, 112, 190, 136 },
              g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    for (i = 0; i < 4; ++i) {
        int top = 148 + i * 60;
        g_app.nav[i] = (RECT){ 16, top, 208, top + 48 };
        if (g_app.page == i) {
            draw_round_rect(dc, g_app.nav[i], COL_ACCENT_DARK, COL_ACCENT, 9);
        } else {
            draw_round_rect(dc, g_app.nav[i], COL_SIDEBAR, COL_SIDEBAR, 9);
        }
        RECT text_rect = { 30, top + 3, 196, top + 26 };
        RECT hint_rect = { 30, top + 25, 198, top + 44 };
        draw_text(dc, labels[i], text_rect, g_app.font_button,
                  g_app.page == i ? COL_TEXT : COL_MUTED,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        draw_text(dc, hints[i], hint_rect, g_app.font_small,
                  COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }

    draw_text(dc, L"TOOLZNG  /  C17", (RECT){ 24, g_app.client_height - 48, 200,
              g_app.client_height - 28 }, g_app.font_small, COL_MUTED,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

static void draw_page_heading(HDC dc, const WCHAR *title, const WCHAR *subtitle)
{
    draw_text(dc, title, (RECT){ 252, 112, g_app.client_width - 28, 151 },
              g_app.font_heading, COL_TEXT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, subtitle, (RECT){ 254, 150, g_app.client_width - 28, 176 },
              g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

static void draw_overview(HDC dc)
{
    int x = 250;
    int right = g_app.client_width - 28;
    int content_width = right - x;
    int gap = 16;
    int card_width = (content_width - 2 * gap) / 3;
    RECT hero = { x, 188, right, 388 };
    WCHAR headline[160];
    WCHAR subline[240];
    int card_y = 416;

    draw_page_heading(dc, L"Центр управления", L"Проверяй подключение и обслуживай устройства Apple без рискованных вмешательств.");
    draw_round_rect(dc, hero, COL_PANEL, COL_BORDER, 16);

    draw_text(dc, L"СОСТОЯНИЕ ПОДКЛЮЧЕНИЯ", (RECT){ x + 24, 208, right - 24, 232 },
              g_app.font_small, COL_ACCENT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    {
        WCHAR score_line[96];
        swprintf_s(score_line, ARRAYSIZE(score_line), L"DUCK READINESS  ·  %d/100", g_app.readiness_score);
        draw_text(dc, score_line, (RECT){ x + 24, 232, right - 176, 253 },
                  g_app.font_small, COL_AMBER, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }

    if (!g_app.scan_complete) {
        copy_text(headline, ARRAYSIZE(headline), L"Сканирование ещё не выполнялось");
        copy_text(subline, ARRAYSIZE(subline), L"Подключи устройство кабелем и запусти проверку USB.");
    } else if (g_app.device_count == 0) {
        copy_text(headline, ARRAYSIZE(headline), L"Apple-устройства не обнаружены");
        copy_text(subline, ARRAYSIZE(subline), L"Разблокируй устройство, подтверди «Доверять этому компьютеру» и проверь USB-кабель.");
    } else {
        swprintf_s(headline, ARRAYSIZE(headline), L"Найдено Apple-устройств: %u", g_app.device_count);
        copy_text(subline, ARRAYSIZE(subline), L"Windows видит оборудование Apple. Подробности доступны в разделе USB-устройства.");
    }

    draw_text(dc, headline, (RECT){ x + 24, 257, right - 176, 292 },
              g_app.font_title, g_app.device_count ? COL_GREEN : COL_TEXT,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    draw_text(dc, subline, (RECT){ x + 26, 294, right - 176, 329 },
              g_app.font_body, COL_MUTED, DT_LEFT | DT_VCENTER | DT_WORDBREAK | DT_END_ELLIPSIS);

    g_app.scan_button = (RECT){ x + 24, 338, x + 190, 376 };
    g_app.report_button = (RECT){ x + 204, 338, x + 382, 376 };
    draw_button(dc, g_app.scan_button, L"Сканировать USB", TRUE);
    draw_button(dc, g_app.report_button, L"Создать отчёт", FALSE);

    g_app.devices_button = (RECT){ x, card_y, x + card_width, card_y + 176 };
    g_app.backups_button = (RECT){ x + card_width + gap, card_y,
                                  x + 2 * card_width + gap, card_y + 176 };
    g_app.care_button = (RECT){ x + 2 * (card_width + gap), card_y,
                               right, card_y + 176 };

    draw_round_rect(dc, g_app.devices_button, COL_CARD, COL_BORDER, 13);
    draw_round_rect(dc, g_app.backups_button, COL_CARD, COL_BORDER, 13);
    draw_round_rect(dc, g_app.care_button, COL_CARD, COL_BORDER, 13);

    draw_text(dc, L"01  /  USB-ДИАГНОСТИКА",
              (RECT){ g_app.devices_button.left + 16, card_y + 18,
                      g_app.devices_button.right - 12, card_y + 43 },
              g_app.font_small, COL_ACCENT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"Список устройств", (RECT){ g_app.devices_button.left + 16,
              card_y + 53, g_app.devices_button.right - 12, card_y + 82 },
              g_app.font_button, COL_TEXT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"Просмотр USB-идентификаторов и статуса драйверной службы Apple.",
              (RECT){ g_app.devices_button.left + 16, card_y + 88,
                      g_app.devices_button.right - 16, card_y + 137 },
              g_app.font_small, COL_MUTED, DT_LEFT | DT_TOP | DT_WORDBREAK);

    draw_text(dc, L"02  /  РЕЗЕРВНЫЕ КОПИИ",
              (RECT){ g_app.backups_button.left + 16, card_y + 18,
                      g_app.backups_button.right - 12, card_y + 43 },
              g_app.font_small, COL_ACCENT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"Backup Sentinel", (RECT){ g_app.backups_button.left + 16,
              card_y + 53, g_app.backups_button.right - 12, card_y + 82 },
              g_app.font_button, COL_TEXT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, g_app.backup_audit,
              (RECT){ g_app.backups_button.left + 16, card_y + 88,
                      g_app.backups_button.right - 16, card_y + 137 },
              g_app.font_small, COL_MUTED, DT_LEFT | DT_TOP | DT_WORDBREAK);

    draw_text(dc, L"03  /  SAFE CARE",
              (RECT){ g_app.care_button.left + 16, card_y + 18,
                      g_app.care_button.right - 12, card_y + 43 },
              g_app.font_small, COL_ACCENT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"Проверка скорости", (RECT){ g_app.care_button.left + 16,
              card_y + 53, g_app.care_button.right - 12, card_y + 82 },
              g_app.font_button, COL_TEXT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"Практические настройки iOS: хранилище, аккумулятор, температура и сеть.",
              (RECT){ g_app.care_button.left + 16, card_y + 88,
                      g_app.care_button.right - 16, card_y + 137 },
              g_app.font_small, COL_MUTED, DT_LEFT | DT_TOP | DT_WORDBREAK);

    draw_text(dc, g_app.status, (RECT){ x + 2, 603, right, 624 },
              g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    draw_text(dc, g_app.backup_audit, (RECT){ x + 2, 625, right, 646 },
              g_app.font_small, COL_AMBER, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    {
        UINT arrivals = 0, removals = 0;
        WCHAR pulse[220];
        get_recent_usb_counts(&arrivals, &removals);
        swprintf_s(pulse, ARRAYSIZE(pulse),
            L"USB Pulse · 10 мин: подключений %u / отключений %u%s",
            arrivals, removals,
            g_app.usb_watch_registered ? L" · наблюдение активно" : L" · наблюдение недоступно");
        draw_text(dc, pulse, (RECT){ x + 2, 647, right, 668 },
                  g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    }
}

static void draw_devices(HDC dc)
{
    int x = 250;
    int right = g_app.client_width - 28;
    int i;

    draw_page_heading(dc, L"USB-устройства Apple", L"Поиск устройств с USB Vendor ID 05AC. Список отражает только то, что обнаруживает Windows.");
    g_app.scan_button = (RECT){ right - 178, 119, right, 159 };
    draw_button(dc, g_app.scan_button, L"Повторить поиск", TRUE);

    draw_round_rect(dc, (RECT){ x, 188, right, 254 }, COL_PANEL, COL_BORDER, 12);
    draw_text(dc, g_app.service_status,
              (RECT){ x + 18, 198, right - 18, 244 },
              g_app.font_body, COL_TEXT, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    if (g_app.device_count == 0) {
        draw_round_rect(dc, (RECT){ x, 274, right, 374 }, COL_CARD, COL_BORDER, 12);
        draw_text(dc, g_app.scan_complete ? L"Устройства Apple не найдены" : L"Нажми «Повторить поиск»",
                  (RECT){ x + 20, 292, right - 20, 326 },
                  g_app.font_title, COL_TEXT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        draw_text(dc, L"Проверь разблокировку iPhone/iPad, кабель с передачей данных и драйвер Apple Devices.",
                  (RECT){ x + 20, 331, right - 20, 360 },
                  g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    } else {
        for (i = 0; i < (int)g_app.device_count && i < 4; ++i) {
            int top = 274 + i * 86;
            RECT item = { x, top, right, top + 74 };
            WCHAR id_line[600];
            draw_round_rect(dc, item, COL_CARD, COL_BORDER, 10);
            draw_text(dc, g_app.devices[i].name,
                      (RECT){ x + 18, top + 8, right - 18, top + 34 },
                      g_app.font_button, COL_TEXT, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            swprintf_s(id_line, ARRAYSIZE(id_line), L"%ls  |  %ls",
                g_app.devices[i].manufacturer[0] ? g_app.devices[i].manufacturer : L"Apple",
                g_app.devices[i].hardware_id);
            draw_text(dc, id_line,
                      (RECT){ x + 18, top + 37, right - 18, top + 64 },
                      g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        }
        if (g_app.device_count > 4) {
            WCHAR more[96];
            swprintf_s(more, ARRAYSIZE(more), L"Дополнительных записей: %u",
                       g_app.device_count - 4);
            draw_text(dc, more, (RECT){ x + 2, 625, right, 648 },
                      g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
    }

    {
        UINT arrivals = 0, removals = 0;
        WCHAR pulse[220];
        get_recent_usb_counts(&arrivals, &removals);
        swprintf_s(pulse, ARRAYSIZE(pulse),
            L"USB Pulse (10 мин): %u подключений · %u отключений · всего событий: %u",
            arrivals, removals, g_app.usb_event_count);
        draw_text(dc, pulse, (RECT){ x + 2, g_app.client_height - 74, right, g_app.client_height - 52 },
                  g_app.font_small, COL_AMBER, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    }
    draw_text(dc, L"События учитываются только для USB interface path с VID_05AC; это индикатор связи, не тест скорости передачи.",
              (RECT){ x + 2, g_app.client_height - 48, right, g_app.client_height - 22 },
              g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
}

static void draw_bullet(HDC dc, int y, const WCHAR *title, const WCHAR *body)
{
    RECT dot = { 258, y + 6, 269, y + 17 };
    RECT title_rect = { 286, y, g_app.client_width - 42, y + 26 };
    RECT body_rect = { 286, y + 27, g_app.client_width - 42, y + 65 };
    HBRUSH brush = CreateSolidBrush(COL_ACCENT);
    FillRect(dc, &dot, brush);
    DeleteObject(brush);
    draw_text(dc, title, title_rect, g_app.font_button, COL_TEXT,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, body, body_rect, g_app.font_small, COL_MUTED,
              DT_LEFT | DT_TOP | DT_WORDBREAK);
}

static void draw_care(HDC dc)
{
    int x = 250;
    int right = g_app.client_width - 28;
    RECT panel = { x, 188, right, 700 };

    draw_page_heading(dc, L"Безопасная оптимизация", L"ToolzNG не обещает программно разогнать iOS — здесь только реальные и безопасные проверки.");
    draw_round_rect(dc, panel, COL_PANEL, COL_BORDER, 14);

    draw_bullet(dc, 210, L"1. Свободное место",
        L"Открой Настройки → Основные → Хранилище iPhone. Удали ненужные загрузки или выгрузи редко используемые приложения. Не удаляй резервные копии наугад.");
    draw_bullet(dc, 278, L"2. Аккумулятор и производительность",
        L"Проверь Настройки → Аккумулятор → Состояние аккумулятора и зарядка. Изношенный аккумулятор может влиять на пиковую производительность.");
    draw_bullet(dc, 346, L"3. Температура устройства",
        L"При перегреве дай iPhone остыть. Не запускай тяжёлые задачи во время зарядки, если устройство заметно нагревается.");
    draw_bullet(dc, 414, L"4. Сеть и приложения",
        L"Проверь качество Wi-Fi или мобильной сети. Не закрывай все приложения принудительно без причины — их повторная загрузка может занять больше времени.");
    draw_bullet(dc, 482, L"5. Обновления и резервная копия",
        L"Используй штатные настройки iOS или Apple Devices для обновления и создания резервной копии перед серьёзными действиями.");

    draw_round_rect(dc, (RECT){ x, 558, right, 642 }, COL_CARD, COL_BORDER, 12);
    draw_text(dc, g_app.coach_title, (RECT){ x + 16, 566, right - 14, 590 },
              g_app.font_button, COL_ACCENT, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    draw_text(dc, g_app.coach_body, (RECT){ x + 16, 590, right - 14, 634 },
              g_app.font_small, COL_MUTED, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_END_ELLIPSIS);

    g_app.support_button = (RECT){ x + 22, 651, x + 234, 691 };
    g_app.store_button = (RECT){ x + 250, 651, x + 486, 691 };
    draw_button(dc, g_app.support_button, L"Советы Apple", TRUE);
    draw_button(dc, g_app.store_button, L"Найти Apple Devices", FALSE);
}

static void draw_about(HDC dc)
{
    int x = 250;
    int right = g_app.client_width - 28;
    RECT panel = { x, 188, right, 612 };

    draw_page_heading(dc, L"О ToolzNG", L"Нативная Windows-утилита на C17 и Win32 API.");
    draw_round_rect(dc, panel, COL_PANEL, COL_BORDER, 14);
    draw_text(dc, L"ToolzNG  0.1.0", (RECT){ x + 24, 216, right - 24, 254 },
              g_app.font_title, COL_TEXT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"Технологии", (RECT){ x + 24, 280, right - 24, 306 },
              g_app.font_button, COL_ACCENT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"C17 · Win32 API · GDI 2D · SetupAPI · Windows Service Control Manager · CMake",
              (RECT){ x + 24, 310, right - 24, 344 },
              g_app.font_body, COL_TEXT, DT_LEFT | DT_VCENTER | DT_WORDBREAK);
    draw_text(dc, L"Что программа делает", (RECT){ x + 24, 374, right - 24, 402 },
              g_app.font_button, COL_ACCENT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"• Находит видимые Windows PnP-устройства Apple по VID_05AC.\n"
                  L"• Считает прозрачный Duck Readiness Score по сигналам Windows.\n"
                  L"• USB Pulse замечает подключения/отключения Apple USB в реальном времени.\n"
                  L"• Backup Sentinel проверяет маркерные файлы резервных копий без чтения данных.\n"
                  L"• DuckFix строит следующий безопасный шаг по результатам проверки.\n"
                  L"• Privacy Shield предварительно показывает отчёт и скрывает детали по умолчанию.",
              (RECT){ x + 24, 408, right - 24, 500 },
              g_app.font_body, COL_TEXT, DT_LEFT | DT_TOP | DT_WORDBREAK);
    draw_text(dc, L"Границы и безопасность", (RECT){ x + 24, 516, right - 24, 544 },
              g_app.font_button, COL_AMBER, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"Нет jailbreak, обхода блокировок, прошивки, сброса, чтения личных данных или автоматического удаления файлов. Программа не может напрямую управлять внутренними процессами iOS.",
              (RECT){ x + 24, 546, right - 24, 590 },
              g_app.font_small, COL_MUTED, DT_LEFT | DT_TOP | DT_WORDBREAK);
}

static void paint_window(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC target = BeginPaint(hwnd, &ps);
    RECT client;
    HDC buffer;
    HBITMAP bitmap;
    HGDIOBJ old_bitmap;
    HBRUSH background;

    GetClientRect(hwnd, &client);
    g_app.client_width = client.right - client.left;
    g_app.client_height = client.bottom - client.top;

    buffer = CreateCompatibleDC(target);
    bitmap = CreateCompatibleBitmap(target, g_app.client_width, g_app.client_height);
    old_bitmap = SelectObject(buffer, bitmap);

    background = CreateSolidBrush(COL_BG);
    FillRect(buffer, &client, background);
    DeleteObject(background);

    draw_header(buffer, g_app.client_width);
    draw_sidebar(buffer);

    switch (g_app.page) {
        case PAGE_DEVICES: draw_devices(buffer); break;
        case PAGE_CARE:    draw_care(buffer); break;
        case PAGE_ABOUT:   draw_about(buffer); break;
        case PAGE_OVERVIEW:
        default:           draw_overview(buffer); break;
    }

    BitBlt(target, 0, 0, g_app.client_width, g_app.client_height,
           buffer, 0, 0, SRCCOPY);

    SelectObject(buffer, old_bitmap);
    DeleteObject(bitmap);
    DeleteDC(buffer);
    EndPaint(hwnd, &ps);
}

static BOOL point_in_rect(POINT point, RECT rect)
{
    return PtInRect(&rect, point) != FALSE;
}

static void handle_action(int action)
{
    switch (action) {
        case ACTION_SCAN:
            set_status(L"Выполняется проверка устройств Windows...");
            refresh_diagnostics();
            break;
        case ACTION_REPORT:
            save_report();
            break;
        case ACTION_DEVICES:
            g_app.page = PAGE_DEVICES;
            InvalidateRect(g_app.hwnd, NULL, FALSE);
            break;
        case ACTION_BACKUPS:
            open_backup_folder();
            break;
        case ACTION_CARE:
            g_app.page = PAGE_CARE;
            InvalidateRect(g_app.hwnd, NULL, FALSE);
            break;
        case ACTION_SUPPORT:
            open_apple_support();
            break;
        case ACTION_STORE:
            open_apple_devices_store();
            break;
        default:
            break;
    }
}

static void handle_click(POINT point)
{
    int i;
    for (i = 0; i < 4; ++i) {
        if (point_in_rect(point, g_app.nav[i])) {
            g_app.page = i;
            InvalidateRect(g_app.hwnd, NULL, FALSE);
            return;
        }
    }

    if (g_app.page == PAGE_OVERVIEW) {
        if (point_in_rect(point, g_app.scan_button))       handle_action(ACTION_SCAN);
        else if (point_in_rect(point, g_app.report_button)) handle_action(ACTION_REPORT);
        else if (point_in_rect(point, g_app.devices_button)) handle_action(ACTION_DEVICES);
        else if (point_in_rect(point, g_app.backups_button)) handle_action(ACTION_BACKUPS);
        else if (point_in_rect(point, g_app.care_button))    handle_action(ACTION_CARE);
    } else if (g_app.page == PAGE_DEVICES) {
        if (point_in_rect(point, g_app.scan_button)) handle_action(ACTION_SCAN);
    } else if (g_app.page == PAGE_CARE) {
        if (point_in_rect(point, g_app.support_button)) handle_action(ACTION_SUPPORT);
        else if (point_in_rect(point, g_app.store_button)) handle_action(ACTION_STORE);
    }
}

static void create_fonts(void)
{
    g_app.font_brand = CreateFontW(27, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_app.font_title = CreateFontW(25, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_app.font_heading = CreateFontW(22, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_app.font_body = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_app.font_small = CreateFontW(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_app.font_button = CreateFontW(15, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

static void delete_fonts(void)
{
    if (g_app.font_brand) DeleteObject(g_app.font_brand);
    if (g_app.font_title) DeleteObject(g_app.font_title);
    if (g_app.font_heading) DeleteObject(g_app.font_heading);
    if (g_app.font_body) DeleteObject(g_app.font_body);
    if (g_app.font_small) DeleteObject(g_app.font_small);
    if (g_app.font_button) DeleteObject(g_app.font_button);
}

static LRESULT CALLBACK window_proc(HWND hwnd, UINT message,
                                    WPARAM wparam, LPARAM lparam)
{
    switch (message) {
        case WM_CREATE:
            g_app.hwnd = hwnd;
            g_app.page = PAGE_OVERVIEW;
            g_app.scan_complete = FALSE;
            g_app.app_started_at = GetTickCount64();
            create_fonts();
            register_usb_watch(hwnd);
            SetTimer(hwnd, TIMER_READINESS, (UINT)USB_STABILITY_WARMUP_MS, NULL);
            copy_text(g_app.status, ARRAYSIZE(g_app.status),
                      L"Готово к проверке. Подключи iPhone/iPad через USB.");
            refresh_diagnostics();
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            paint_window(hwnd);
            return 0;

        case WM_SIZE:
            g_app.client_width = LOWORD(lparam);
            g_app.client_height = HIWORD(lparam);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;

        case WM_DEVICECHANGE:
            if ((wparam == DBT_DEVICEARRIVAL || wparam == DBT_DEVICEREMOVECOMPLETE) && lparam) {
                const DEV_BROADCAST_HDR *header = (const DEV_BROADCAST_HDR *)lparam;
                if (header->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
                    const DEV_BROADCAST_DEVICEINTERFACE_W *info =
                        (const DEV_BROADCAST_DEVICEINTERFACE_W *)lparam;
                    if (wcsstr(info->dbcc_name, L"VID_05AC") != NULL) {
                        record_usb_pulse(wparam == DBT_DEVICEARRIVAL);
                    }
                }
            }
            return TRUE;

        case WM_TIMER:
            if (wparam == TIMER_USB_REFRESH) {
                KillTimer(hwnd, TIMER_USB_REFRESH);
                refresh_diagnostics();
                return 0;
            }
            if (wparam == TIMER_READINESS) {
                compute_readiness_and_coach();
                InvalidateRect(hwnd, NULL, FALSE);
                SetTimer(hwnd, TIMER_READINESS, (UINT)USB_STABILITY_WARMUP_MS, NULL);
                return 0;
            }
            return DefWindowProcW(hwnd, message, wparam, lparam);

        case WM_LBUTTONUP: {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            handle_click(point);
            return 0;
        }

        case WM_GETMINMAXINFO: {
            MINMAXINFO *info = (MINMAXINFO *)lparam;
            info->ptMinTrackSize.x = 920;
            info->ptMinTrackSize.y = 760;
            return 0;
        }

        case WM_DESTROY:
            if (g_app.usb_notification) {
                UnregisterDeviceNotification(g_app.usb_notification);
                g_app.usb_notification = NULL;
            }
            delete_fonts();
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProcW(hwnd, message, wparam, lparam);
    }
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previous,
                    PWSTR command_line, int show_command)
{
    WNDCLASSEXW wc;
    HWND hwnd;
    MSG message;
    (void)previous;
    (void)command_line;

    ZeroMemory(&g_app, sizeof(g_app));
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = window_proc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wc.hbrBackground = NULL;
    wc.lpszClassName = L"ToolzNG.MainWindow";

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"Не удалось зарегистрировать класс окна.",
                    APP_TITLE, MB_OK | MB_ICONERROR);
        return 1;
    }

    hwnd = CreateWindowExW(0, wc.lpszClassName, APP_TITLE,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        1120, 790, NULL, NULL, instance, NULL);
    if (!hwnd) {
        MessageBoxW(NULL, L"Не удалось создать главное окно.",
                    APP_TITLE, MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, show_command);
    UpdateWindow(hwnd);

    while (GetMessageW(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return (int)message.wParam;
}