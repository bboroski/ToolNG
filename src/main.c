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
#include <wincrypt.h>
#include <commctrl.h>
#include <commdlg.h>
#include <dbt.h>
#include <usbiodef.h>

/* Local GUID definition avoids a uuid.lib dependency for this interface class. */
const GUID GUID_DEVINTERFACE_USB_DEVICE =
    { 0xA5DCBF10L, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } };
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "sambu_engine.h"

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

/* Theme tokens: graphite base, controlled yellow accents, accessible status colors. */
#define COL_BG              (g_app.high_contrast ? RGB(0, 0, 0) : RGB(13, 13, 13))
#define COL_HEADER          (g_app.high_contrast ? RGB(0, 0, 0) : RGB(20, 20, 20))
#define COL_SIDEBAR         (g_app.high_contrast ? RGB(0, 0, 0) : RGB(20, 20, 20))
#define COL_PANEL           (g_app.high_contrast ? RGB(0, 0, 0) : RGB(26, 26, 26))
#define COL_CARD            (g_app.high_contrast ? RGB(0, 0, 0) : RGB(26, 26, 26))
#define COL_CARD_HOVER      (g_app.high_contrast ? RGB(32, 32, 32) : RGB(38, 38, 38))
#define COL_BORDER          (g_app.high_contrast ? RGB(255, 255, 255) : RGB(38, 38, 38))
#define COL_TEXT            (g_app.high_contrast ? RGB(255, 255, 255) : RGB(230, 230, 230))
#define COL_MUTED           (g_app.high_contrast ? RGB(255, 255, 255) : RGB(153, 153, 153))
#define COL_ACCENT          (g_app.high_contrast ? RGB(255, 255, 0) : RGB(255, 214, 0))
#define COL_ACCENT_DARK     (g_app.high_contrast ? RGB(255, 255, 0) : RGB(191, 161, 0))
#define COL_GREEN           (g_app.high_contrast ? RGB(255, 255, 255) : RGB(76, 175, 80))
#define COL_AMBER           (g_app.high_contrast ? RGB(255, 255, 0) : RGB(255, 214, 0))
#define COL_ERROR           RGB(255, 68, 68)

#define PAGE_OVERVIEW 0
#define PAGE_DEVICES  1
#define PAGE_CARE     2
#define PAGE_ABOUT    3
#define PAGE_REPORTS  4
#define PAGE_HELP     5
#define PAGE_COUNT    6
#define MAX_REPORTS   32
#define REPORT_ROWS   8

#define ACTION_NONE       0
#define ACTION_SCAN       1
#define ACTION_REPORT     2
#define ACTION_DEVICES    3
#define ACTION_BACKUPS    4
#define ACTION_CARE       5
#define ACTION_HELP       6
#define ACTION_PLAYBOOK   7

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

typedef struct ReportEntry {
    WCHAR name[MAX_PATH];
    WCHAR path[MAX_PATH * 2];
} ReportEntry;

typedef struct AppState {
    HWND hwnd;
    int page;
    int client_width;
    int client_height;
    UINT dpi;
    UINT effective_dpi;
    UINT zoom_percent;
    BOOL high_contrast;
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
    ULONGLONG backup_age_days;
    ULONGLONG link_signature;
    BOOL link_baseline_known;
    BOOL link_profile_changed;
    WCHAR local_store[MAX_PATH * 2];
    WCHAR pulse_journal[MAX_PATH * 2];
    WCHAR link_profile_file[MAX_PATH * 2];
    WCHAR settings_file[MAX_PATH * 2];
    WCHAR reports_dir[MAX_PATH * 2];
    WCHAR last_report_path[MAX_PATH * 2];
    WCHAR report_content[REPORT_CHARS];
    int report_scroll;
    int report_list_scroll;
    int report_content_height;
    int report_count;
    int selected_report;
    ReportEntry reports[MAX_REPORTS];
    RECT report_rows[REPORT_ROWS];
    int report_row_index[REPORT_ROWS];
    RECT report_copy_button;
    RECT report_export_button;
    RECT report_refresh_button;
    RECT zoom_minus_button;
    RECT zoom_plus_button;
    RECT playbook_button;
    int readiness_score;
    SambuAssessment sambu_assessment;
    SambuLedger sambu_ledger;
    SambuFinding last_ledger_finding;
    WCHAR coach_title[180];
    WCHAR coach_body[640];
    HFONT font_brand;
    HFONT font_title;
    HFONT font_heading;
    HFONT font_body;
    HFONT font_small;
    HFONT font_button;
    RECT nav[PAGE_COUNT];
    RECT scan_button;
    RECT report_button;
    RECT devices_button;
    RECT backups_button;
    RECT care_button;
    RECT support_button;
    RECT store_button;
} AppState;

static AppState g_app;

static void create_fonts(void);
static void delete_fonts(void);
static void copy_text(WCHAR *dst, size_t dst_count, const WCHAR *src)
{
    if (!dst || dst_count == 0) return;
    if (!src) src = L"";
    wcsncpy_s(dst, dst_count, src, _TRUNCATE);
}

static void copy_ascii_to_wide(WCHAR *dst, size_t dst_count, const char *src)
{
    if (!dst || dst_count == 0) return;
    dst[0] = L'\0';
    if (!src) return;
    (void)MultiByteToWideChar(CP_UTF8, 0, src, -1, dst, (int)dst_count);
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
    draw_text(dc, label, rect, g_app.font_button,
              primary ? COL_BG : COL_ACCENT,
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


/* ToolzNG 0.6 local-only intelligence store. No device serials or backup payloads are written. */
static void initialize_local_store(void)
{
    WCHAR base[MAX_PATH * 2] = {0};
    DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", base, ARRAYSIZE(base));
    if (!n || n >= ARRAYSIZE(base)) return;
    append_path(g_app.local_store, ARRAYSIZE(g_app.local_store), base, L"ToolzNG");
    (void)CreateDirectoryW(g_app.local_store, NULL);
    append_path(g_app.pulse_journal, ARRAYSIZE(g_app.pulse_journal),
                g_app.local_store, L"usb-pulse.jsonl");
    append_path(g_app.link_profile_file, ARRAYSIZE(g_app.link_profile_file),
                g_app.local_store, L"link-profile.ini");
    append_path(g_app.settings_file, ARRAYSIZE(g_app.settings_file),
                g_app.local_store, L"settings.ini");
    append_path(g_app.reports_dir, ARRAYSIZE(g_app.reports_dir),
                g_app.local_store, L"Reports");
    (void)CreateDirectoryW(g_app.reports_dir, NULL);
}

static void update_high_contrast(void)
{
    HIGHCONTRASTW settings;
    ZeroMemory(&settings, sizeof(settings));
    settings.cbSize = sizeof(settings);
    g_app.high_contrast =
        SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(settings), &settings, 0) &&
        ((settings.dwFlags & HCF_HIGHCONTRASTON) != 0);
}

static void migrate_legacy_reports(void)
{
    PWSTR documents = NULL;
    WCHAR pattern[MAX_PATH * 2];
    WIN32_FIND_DATAW data;
    HANDLE find;
    if (!g_app.settings_file[0] || !g_app.reports_dir[0]) return;
    if (GetPrivateProfileIntW(L"Migration", L"legacy_reports_v1", 0,
                              g_app.settings_file) != 0) return;
    if (SUCCEEDED(SHGetKnownFolderPath(&FOLDERID_Documents, 0, NULL, &documents))) {
        append_path(pattern, ARRAYSIZE(pattern), documents, L"ToolzNG-report-*.txt");
        find = FindFirstFileW(pattern, &data);
        if (find != INVALID_HANDLE_VALUE) {
            do {
                WCHAR source[MAX_PATH * 2], destination[MAX_PATH * 2];
                if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
                append_path(source, ARRAYSIZE(source), documents, data.cFileName);
                append_path(destination, ARRAYSIZE(destination),
                            g_app.reports_dir, data.cFileName);
                if (GetFileAttributesW(destination) == INVALID_FILE_ATTRIBUTES)
                    (void)CopyFileW(source, destination, TRUE);
            } while (FindNextFileW(find, &data));
            FindClose(find);
        }
        CoTaskMemFree(documents);
    }
    /* Mark the one-time migration complete; source files are deliberately preserved. */
    (void)WritePrivateProfileStringW(L"Migration", L"legacy_reports_v1", L"1",
                                      g_app.settings_file);
}

static void load_display_settings(void)
{
    UINT stored = 100;
    if (g_app.settings_file[0])
        stored = (UINT)GetPrivateProfileIntW(L"Display", L"zoom", 100,
                                              g_app.settings_file);
    if (stored < 80) stored = 80;
    if (stored > 160) stored = 160;
    g_app.zoom_percent = stored;
}

static void save_display_settings(void)
{
    WCHAR value[16];
    if (!g_app.settings_file[0]) return;
    swprintf_s(value, ARRAYSIZE(value), L"%u", g_app.zoom_percent);
    (void)WritePrivateProfileStringW(L"Display", L"zoom", value,
                                      g_app.settings_file);
}

static UINT effective_dpi(void)
{
    UINT dpi = g_app.dpi ? g_app.dpi : 96;
    UINT zoom = g_app.zoom_percent ? g_app.zoom_percent : 100;
    UINT result = (dpi * zoom + 50) / 100;
    if (result < 60) result = 60;
    if (result > 384) result = 384;
    return result;
}

static void adjust_zoom(int delta)
{
    int next = (int)(g_app.zoom_percent ? g_app.zoom_percent : 100) + delta;
    if (next < 80) next = 80;
    if (next > 160) next = 160;
    if ((UINT)next == g_app.zoom_percent) return;
    g_app.zoom_percent = (UINT)next;
    g_app.effective_dpi = effective_dpi();
    save_display_settings();
    delete_fonts();
    create_fonts();
    if (g_app.hwnd) InvalidateRect(g_app.hwnd, NULL, FALSE);
}

static void append_pulse_vault(const UsbPulseEvent *event)
{
    SYSTEMTIME st;
    WCHAR line[256], old_path[MAX_PATH * 2];
    char utf8[600];
    int bytes;
    HANDLE file;
    WIN32_FILE_ATTRIBUTE_DATA attrs;
    ULONGLONG size = 0;
    if (!event || !g_app.pulse_journal[0]) return;
    if (GetFileAttributesExW(g_app.pulse_journal, GetFileExInfoStandard, &attrs)) {
        ULARGE_INTEGER n;
        n.HighPart = attrs.nFileSizeHigh;
        n.LowPart = attrs.nFileSizeLow;
        size = n.QuadPart;
    }
    if (size > 262144ULL) {
        copy_text(old_path, ARRAYSIZE(old_path), g_app.pulse_journal);
        wcscat_s(old_path, ARRAYSIZE(old_path), L".old");
        (void)MoveFileExW(g_app.pulse_journal, old_path,
                          MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
    }
    GetLocalTime(&st);
    swprintf_s(line, ARRAYSIZE(line),
        L"{\"schema\":1,\"time\":\"%04u-%02u-%02uT%02u:%02u:%02u\","
        L"\"event\":\"%ls\",\"devices\":%u}\r\n",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
        event->arrival ? L"arrival" : L"removal", g_app.device_count);
    bytes = WideCharToMultiByte(CP_UTF8, 0, line, -1, utf8,
                                (int)sizeof(utf8), NULL, NULL);
    if (bytes <= 1) return;
    file = CreateFileW(g_app.pulse_journal, FILE_APPEND_DATA,
        FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        (void)WriteFile(file, utf8, (DWORD)(bytes - 1), &written, NULL);
        CloseHandle(file);
    }
}

/* FNV-1a is a local change detector, not a security or identity credential. */
static ULONGLONG link_hash_text(ULONGLONG hash, const WCHAR *value)
{
    const WCHAR *p = value ? value : L"";
    while (*p) {
        hash ^= (BYTE)(*p & 0xFF);
        hash *= 1099511628211ULL;
        hash ^= (BYTE)((*p >> 8) & 0xFF);
        hash *= 1099511628211ULL;
        ++p;
    }
    hash ^= 0xFF;
    hash *= 1099511628211ULL;
    return hash;
}

static void update_linkdna_profile(void)
{
    ULONGLONG hash = 14695981039346656037ULL;
    WCHAR stored[40] = {0}, current[40] = {0};
    UINT i;
    if (!g_app.link_profile_file[0] || g_app.device_count == 0) return;
    hash ^= g_app.device_count;
    hash *= 1099511628211ULL;
    for (i = 0; i < g_app.device_count; ++i) {
        hash = link_hash_text(hash, g_app.devices[i].hardware_id);
        hash = link_hash_text(hash, g_app.devices[i].manufacturer);
    }
    hash ^= g_app.apple_service_running ? 1ULL : 0ULL;
    hash *= 1099511628211ULL;
    g_app.link_signature = hash;
    swprintf_s(current, ARRAYSIZE(current), L"%016llX", hash);
    (void)GetPrivateProfileStringW(L"LinkDNA", L"signature", L"",
        stored, ARRAYSIZE(stored), g_app.link_profile_file);
    if (!stored[0]) {
        (void)WritePrivateProfileStringW(L"LinkDNA", L"signature",
            current, g_app.link_profile_file);
        g_app.link_baseline_known = TRUE;
        g_app.link_profile_changed = FALSE;
    } else {
        g_app.link_baseline_known = TRUE;
        g_app.link_profile_changed = (_wcsicmp(stored, current) != 0);
    }
    if (g_app.link_profile_changed) {
        set_status(L"LinkDNA: набор USB/службы отличается от локального базового профиля.");
    } else {
        set_status(L"LinkDNA: локальный профиль подключения совпадает с базовым.");
    }
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
    FILETIME newest_write = {0};
    BOOL have_newest_write = FALSE;
    g_app.backup_age_days = ~(ULONGLONG)0;

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
        if (has_info) {
            WCHAR info_path[MAX_PATH * 2];
            WIN32_FILE_ATTRIBUTE_DATA marker;
            append_path(info_path, ARRAYSIZE(info_path), child, L"Info.plist");
            if (GetFileAttributesExW(info_path, GetFileExInfoStandard, &marker)) {
                if (!have_newest_write || CompareFileTime(&marker.ftLastWriteTime, &newest_write) > 0) {
                    newest_write = marker.ftLastWriteTime;
                    have_newest_write = TRUE;
                }
            }
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);

    if (have_newest_write) {
        FILETIME now_file;
        ULARGE_INTEGER now_value, backup_value;
        GetSystemTimeAsFileTime(&now_file);
        now_value.HighPart = now_file.dwHighDateTime;
        now_value.LowPart = now_file.dwLowDateTime;
        backup_value.HighPart = newest_write.dwHighDateTime;
        backup_value.LowPart = newest_write.dwLowDateTime;
        if (now_value.QuadPart >= backup_value.QuadPart) {
            g_app.backup_age_days =
                (now_value.QuadPart - backup_value.QuadPart) / 864000000000ULL;
        }
    }

    g_app.backup_folder_count = folders;
    g_app.backup_valid_count = valid;
    if (folders == 0) {
        copy_text(g_app.backup_audit, ARRAYSIZE(g_app.backup_audit),
                  L"Backup Sentinel: папка найдена, но резервных копий внутри нет.");
    } else {
        if (g_app.backup_age_days != ~(ULONGLONG)0) {
            swprintf_s(g_app.backup_audit, ARRAYSIZE(g_app.backup_audit),
                L"Backup Sentinel: %lu/%lu маркеры на месте · свежесть: %llu дн.",
                (unsigned long)valid, (unsigned long)folders,
                (unsigned long long)g_app.backup_age_days);
        } else {
            swprintf_s(g_app.backup_audit, ARRAYSIZE(g_app.backup_audit),
                L"Backup Sentinel: %lu из %lu папок имеют Info.plist и Manifest.",
                (unsigned long)valid, (unsigned long)folders);
        }
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

    append_pulse_vault(event);
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
    SambuEvidence evidence;
    UINT removals = 0;
    ULONGLONG elapsed = GetTickCount64() - g_app.app_started_at;

    ZeroMemory(&evidence, sizeof(evidence));
    get_recent_usb_counts(NULL, &removals);

    evidence.scan_complete = g_app.scan_complete != FALSE;
    evidence.device_found = g_app.device_count > 0;
    evidence.service_running = g_app.apple_service_running != FALSE;
    evidence.backup_found = g_app.backup_found != FALSE;
    evidence.backup_marker_valid = g_app.backup_valid_count > 0;
    evidence.backup_folders_seen = (uint32_t)g_app.backup_folder_count;
    evidence.backup_folders_valid = (uint32_t)g_app.backup_valid_count;
    evidence.backup_age_known = g_app.backup_age_days != ~(ULONGLONG)0;
    evidence.backup_age_days = evidence.backup_age_known
        ? (g_app.backup_age_days > UINT32_MAX
            ? UINT32_MAX : (uint32_t)g_app.backup_age_days)
        : 0u;
    evidence.recent_usb_removals = removals;
    evidence.usb_watch_registered = g_app.usb_watch_registered != FALSE;
    evidence.stability_window_elapsed = elapsed >= USB_STABILITY_WARMUP_MS;

    g_app.sambu_assessment = sambu_assess(&evidence);
    g_app.readiness_score = g_app.sambu_assessment.readiness_score;
    if (g_app.sambu_assessment.finding != SAMBU_FINDING_NONE &&
        g_app.sambu_assessment.finding != g_app.last_ledger_finding) {
        sambu_ledger_record(&g_app.sambu_ledger,
            (uint32_t)g_app.sambu_assessment.finding, GetTickCount64());
    }
    g_app.last_ledger_finding = g_app.sambu_assessment.finding;

    switch (g_app.sambu_assessment.finding) {
        case SAMBU_FINDING_SCAN_UNAVAILABLE:
            copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                      L"DuckFix: результат сканирования недоступен");
            copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                      L"Windows не завершила перечисление устройств. Повтори проверку; отсутствие устройства пока нельзя считать подтверждённым. ToolzNG не меняет параметры iPhone и не выполняет ремонт автоматически.");
            break;
        case SAMBU_FINDING_NO_DEVICE:
            copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                      L"DuckFix: начни с физического подключения");
            copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                      L"Используй кабель с передачей данных, разблокируй iPhone и подтверди «Доверять этому компьютеру». Затем нажми «Сканировать USB». ToolzNG не может подтвердить доверие iOS напрямую.");
            break;
        case SAMBU_FINDING_SERVICE_UNAVAILABLE:
            copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                      L"DuckFix: проверь компонент Apple для Windows");
            copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                      L"Windows видит USB-интерфейс, но служба Apple не подтверждена как запущенная. Установи или обнови Apple Devices; если служба установлена, проверь её состояние в приложении «Службы».");
            break;
        case SAMBU_FINDING_USB_UNSTABLE:
            copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                      L"DuckFix: проверь стабильность USB-линии");
            copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                      L"USB Pulse зарегистрировал отключения Apple-интерфейса за последние 10 минут. Попробуй другой порт без USB-хаба и исправный кабель передачи данных; наблюдай за счётчиком повторных отключений.");
            break;
        case SAMBU_FINDING_BACKUP_STRUCTURE:
            copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                      L"DuckFix: проверь структуру локальной копии");
            copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                      L"Backup Sentinel не нашёл ожидаемые маркеры Info.plist и Manifest.db/Manifest.plist. Не удаляй папки: создай новую копию через Apple Devices или iTunes и сравни результат.");
            break;
        case SAMBU_FINDING_BACKUP_STALE:
            copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                      L"DuckFix: резервная копия давно не обновлялась");
            copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                      L"Самая свежая Info.plist старше 30 дней. Если на устройстве появились новые данные, создай актуальную копию через Apple Devices/iTunes. Возраст маркера — ориентир, а не доказательство целостности backup.");
            break;
        case SAMBU_FINDING_BACKUP_MISSING:
            copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                      L"DuckFix: создай резервную копию");
            copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                      L"Подключение Apple обнаружено. Для защиты данных создай локальную резервную копию штатным приложением Apple Devices или iTunes. ToolzNG не создаёт и не изменяет backup самостоятельно.");
            break;
        case SAMBU_FINDING_HEALTHY:
            copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                      L"DuckFix: базовые проверки пройдены");
            copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                      L"Windows видит Apple USB, служба доступна, а структура резервной копии выглядит ожидаемой. Это не тест скорости iOS и не проверка содержимого backup; продолжай пользоваться штатными средствами Apple.");
            break;
        case SAMBU_FINDING_NONE:
        default:
            copy_text(g_app.coach_title, ARRAYSIZE(g_app.coach_title),
                      L"DuckFix: недостаточно данных");
            copy_text(g_app.coach_body, ARRAYSIZE(g_app.coach_body),
                      L"Повтори диагностику, чтобы получить проверяемые сигналы.");
            break;
    }

    if (g_app.sambu_assessment.quality_flags != 0u) {
        WCHAR quality_key[64];
        WCHAR note[240];
        copy_ascii_to_wide(quality_key, ARRAYSIZE(quality_key),
            sambu_quality_key(g_app.sambu_assessment.quality_flags));
        if (swprintf_s(note, ARRAYSIZE(note),
                L"\r\nSAMBU Quality Gate: %ls. Повтори проверку; неподтверждённые сигналы не учитываются в оценке.",
                quality_key) > 0) {
            (void)wcscat_s(g_app.coach_body, ARRAYSIZE(g_app.coach_body), note);
        }
    }

    if (g_app.sambu_assessment.finding != SAMBU_FINDING_NONE &&
        g_app.sambu_assessment.finding != SAMBU_FINDING_HEALTHY) {
        const uint32_t repeats = sambu_ledger_recent_occurrences(
            &g_app.sambu_ledger,
            (uint32_t)g_app.sambu_assessment.finding,
            GetTickCount64());
        if (repeats >= 2u) {
            WCHAR note[144];
            if (swprintf_s(note, ARRAYSIZE(note),
                    L"\r\nПовторяемость: %u случаев за 10 минут. Проверь первопричину.",
                    (unsigned)repeats) > 0) {
                (void)wcscat_s(g_app.coach_body,
                               ARRAYSIZE(g_app.coach_body), note);
            }
        }
    }
}

static void refresh_diagnostics(void)
{
    scan_apple_devices();
    read_apple_service_status();
    update_linkdna_profile();
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



static BOOL write_sha256_seal(const WCHAR *path)
{
    HANDLE file = INVALID_HANDLE_VALUE;
    LARGE_INTEGER size;
    BYTE *data = NULL;
    BYTE digest[32];
    DWORD got = 0, digest_size = sizeof(digest);
    HCRYPTPROV provider = 0;
    HCRYPTHASH hash = 0;
    WCHAR sidecar[MAX_PATH * 2], output[160];
    static const WCHAR hex[] = L"0123456789abcdef";
    BOOL ok = FALSE;

    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    if (!GetFileSizeEx(file, &size) || size.QuadPart < 0 || size.QuadPart > 8 * 1024 * 1024) {
        CloseHandle(file);
        return FALSE;
    }
    data = (BYTE *)malloc((size_t)(size.QuadPart ? size.QuadPart : 1));
    if (!data) { CloseHandle(file); return FALSE; }
    if (size.QuadPart && (!ReadFile(file, data, (DWORD)size.QuadPart, &got, NULL) ||
                          got != (DWORD)size.QuadPart)) {
        free(data); CloseHandle(file); return FALSE;
    }
    CloseHandle(file);
    if (!CryptAcquireContextW(&provider, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
        goto cleanup;
    if (!CryptCreateHash(provider, CALG_SHA_256, 0, 0, &hash))
        goto cleanup;
    if (size.QuadPart && !CryptHashData(hash, data, (DWORD)size.QuadPart, 0))
        goto cleanup;
    if (!CryptGetHashParam(hash, HP_HASHVAL, digest, &digest_size, 0) || digest_size != 32)
        goto cleanup;

    for (DWORD i = 0; i < 32; ++i) {
        output[i * 2] = hex[digest[i] >> 4];
        output[i * 2 + 1] = hex[digest[i] & 0x0F];
    }
    output[64] = L' '; output[65] = L' '; output[66] = L' ';
    output[67] = L'T'; output[68] = L'o'; output[69] = L'o';
    output[70] = L'l'; output[71] = L'z'; output[72] = L'N';
    output[73] = L'G'; output[74] = L'\r'; output[75] = L'\n';
    output[76] = L'\0';
    copy_text(sidecar, ARRAYSIZE(sidecar), path);
    wcscat_s(sidecar, ARRAYSIZE(sidecar), L".sha256");
    ok = write_utf8_file(sidecar, output);

cleanup:
    if (hash) CryptDestroyHash(hash);
    if (provider) CryptReleaseContext(provider, 0);
    if (data) free(data);
    return ok;
}

static void save_duckfix_route(void)
{
    SYSTEMTIME now;
    WCHAR path[MAX_PATH * 2], filename[160], contents[5000];
    WCHAR engine_version[32], report_schema[64];
    if (!sambu_authorize(SAMBU_OP_WRITE_PRIVATE_REPORT, SAMBU_ORIGIN_USER)) {
        set_status(L"Сохранение маршрута требует явного действия пользователя.");
        return;
    }
    if (!g_app.reports_dir[0]) {
        set_status(L"Локальное хранилище отчётов недоступно.");
        return;
    }
    copy_ascii_to_wide(engine_version, ARRAYSIZE(engine_version), sambu_engine_version());
    copy_ascii_to_wide(report_schema, ARRAYSIZE(report_schema), sambu_report_schema());
    GetLocalTime(&now);
    swprintf_s(filename, ARRAYSIZE(filename),
        L"DuckFix-%04u%02u%02u-%02u%02u%02u-%03u.txt",
        now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute,
        now.wSecond, now.wMilliseconds);
    append_path(path, ARRAYSIZE(path), g_app.reports_dir, filename);
    swprintf_s(contents, ARRAYSIZE(contents),
        L"ToolzNG — маршрут DuckFix\r\n================================\r\n"
        L"SAMBU engine: %ls · diagnostic schema: %ls\r\n"
        L"SAMBU finding code: %u · confidence: %u%% · quality flags: 0x%08lX\r\n"
        L"Состояние: %ls\r\nDuck Readiness: %d/100\r\nUSB interfaces: %u\r\n"
        L"Apple service: %ls\r\nBackup Sentinel: %ls\r\n"
        L"USB events lifetime: %u arrivals / %u removals\r\n\r\n"
        L"ПЕРСОНАЛЬНЫЙ СЛЕДУЮЩИЙ ШАГ\r\n%ls\r\n%ls\r\n\r\n"
        L"БЕЗОПАСНЫЙ ПОРЯДОК\r\n"
        L"1. Проверь кабель передачи данных, USB-порт, разблокировку и запрос доверия.\r\n"
        L"2. Проверь Apple Devices и состояние Apple Mobile Device Service.\r\n"
        L"3. Если backup нужен, создай свежую копию штатным приложением Apple.\r\n"
        L"4. Повтори сканирование ToolzNG и сравни показатели.\r\n"
        L"5. Не удаляй папки backup и не прошивай устройство ради ошибки подключения.\r\n\r\n"
        L"Это план действий, а не автоматический ремонт. ToolzNG не меняет iOS,\r\n"
        L"не удаляет пользовательские данные и не обходит блокировки.\r\n",
        engine_version, report_schema,
        (unsigned)g_app.sambu_assessment.finding,
        g_app.sambu_assessment.confidence_percent,
        (unsigned long)g_app.sambu_assessment.quality_flags,
        g_app.status, g_app.readiness_score, g_app.device_count,
        g_app.service_status, g_app.backup_audit,
        g_app.usb_arrivals_total, g_app.usb_removals_total,
        g_app.coach_title, g_app.coach_body);
    if (write_utf8_file(path, contents))
        set_status(L"Маршрут DuckFix сохранён во внутреннем архиве приложения.");
    else
        set_status(L"Не удалось сохранить маршрут DuckFix в локальное хранилище.");
}

static BOOL read_utf8_file(const WCHAR *path, WCHAR *output, size_t output_count)
{
    HANDLE file;
    LARGE_INTEGER size;
    BYTE *bytes;
    DWORD got = 0;
    int chars;
    if (!path || !output || output_count < 2) return FALSE;
    output[0] = L'\0';
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    if (!GetFileSizeEx(file, &size) || size.QuadPart < 0 ||
        size.QuadPart > (LONGLONG)(output_count * 4)) {
        CloseHandle(file);
        return FALSE;
    }
    bytes = (BYTE *)malloc((size_t)size.QuadPart + 1);
    if (!bytes) { CloseHandle(file); return FALSE; }
    if (size.QuadPart && (!ReadFile(file, bytes, (DWORD)size.QuadPart, &got, NULL) ||
                          got != (DWORD)size.QuadPart)) {
        free(bytes); CloseHandle(file); return FALSE;
    }
    CloseHandle(file);
    bytes[got] = 0;
    chars = got ? MultiByteToWideChar(CP_UTF8, 0, (const char *)bytes,
                                      (int)got, output, (int)output_count - 1) : 0;
    free(bytes);
    if (chars <= 0 && got) { output[0] = L'\0'; return FALSE; }
    output[chars] = L'\0';
    return TRUE;
}

static BOOL load_report_file(int index)
{
    if (index < 0 || index >= g_app.report_count) return FALSE;
    if (!read_utf8_file(g_app.reports[index].path, g_app.report_content,
                        ARRAYSIZE(g_app.report_content))) {
        copy_text(g_app.report_content, ARRAYSIZE(g_app.report_content),
                  L"Не удалось прочитать выбранный отчёт. Файл мог быть удалён или повреждён.");
        return FALSE;
    }
    copy_text(g_app.last_report_path, ARRAYSIZE(g_app.last_report_path),
              g_app.reports[index].path);
    g_app.selected_report = index;
    g_app.report_scroll = 0;
    g_app.report_content_height = 0;
    return TRUE;
}

static void refresh_report_history(void)
{
    WCHAR pattern[MAX_PATH * 2];
    WIN32_FIND_DATAW data;
    HANDLE find;
    int i, j;
    g_app.report_count = 0;
    g_app.report_list_scroll = 0;
    if (!g_app.reports_dir[0]) return;
    append_path(pattern, ARRAYSIZE(pattern), g_app.reports_dir,
                L"ToolzNG-report-*.txt");
    find = FindFirstFileW(pattern, &data);
    if (find != INVALID_HANDLE_VALUE) {
        do {
            ReportEntry *entry;
            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            if (g_app.report_count >= MAX_REPORTS) break;
            entry = &g_app.reports[g_app.report_count++];
            copy_text(entry->name, ARRAYSIZE(entry->name), data.cFileName);
            append_path(entry->path, ARRAYSIZE(entry->path),
                        g_app.reports_dir, data.cFileName);
        } while (FindNextFileW(find, &data));
        FindClose(find);
    }
    for (i = 0; i < g_app.report_count; ++i) {
        for (j = i + 1; j < g_app.report_count; ++j) {
            if (wcscmp(g_app.reports[i].name, g_app.reports[j].name) < 0) {
                ReportEntry swap = g_app.reports[i];
                g_app.reports[i] = g_app.reports[j];
                g_app.reports[j] = swap;
            }
        }
    }
    if (g_app.report_count > 0) {
        (void)load_report_file(0);
    } else {
        g_app.last_report_path[0] = L'\0';
        copy_text(g_app.report_content, ARRAYSIZE(g_app.report_content),
                  L"Отчётов пока нет.\r\n\r\nНажми «Создать отчёт» на главном экране. Приватный отчёт будет сохранён локально и сразу появится здесь.");
        g_app.report_scroll = 0;
    }
}

static BOOL copy_report_to_clipboard(void)
{
    SIZE_T bytes;
    HGLOBAL memory;
    WCHAR *target;
    size_t chars;
    if (!sambu_authorize(SAMBU_OP_CLIPBOARD, SAMBU_ORIGIN_USER)) return FALSE;
    chars = wcslen(g_app.report_content) + 1;
    bytes = chars * sizeof(WCHAR);
    if (!OpenClipboard(g_app.hwnd)) return FALSE;
    if (!EmptyClipboard()) { CloseClipboard(); return FALSE; }
    memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!memory) { CloseClipboard(); return FALSE; }
    target = (WCHAR *)GlobalLock(memory);
    if (!target) { GlobalFree(memory); CloseClipboard(); return FALSE; }
    memcpy(target, g_app.report_content, bytes);
    GlobalUnlock(memory);
    if (!SetClipboardData(CF_UNICODETEXT, memory)) {
        GlobalFree(memory);
        CloseClipboard();
        return FALSE;
    }
    CloseClipboard();
    return TRUE;
}

static void export_report_copy(void)
{
    OPENFILENAMEW dialog;
    WCHAR target[MAX_PATH * 2] = L"ToolzNG-report.txt";
    if (!sambu_authorize(SAMBU_OP_EXPORT, SAMBU_ORIGIN_USER)) {
        set_status(L"Экспорт доступен только по явному запросу пользователя.");
        return;
    }
    ZeroMemory(&dialog, sizeof(dialog));
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = g_app.hwnd;
    dialog.lpstrFilter = L"Текстовый отчёт (*.txt)\0*.txt\0Все файлы (*.*)\0*.*\0\0";
    dialog.lpstrFile = target;
    dialog.nMaxFile = ARRAYSIZE(target);
    dialog.lpstrDefExt = L"txt";
    dialog.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetSaveFileNameW(&dialog)) {
        if (write_utf8_file(target, g_app.report_content)) {
            (void)write_sha256_seal(target);
            set_status(L"Копия отчёта экспортирована по выбранному пути.");
        } else {
            set_status(L"Не удалось экспортировать TXT-копию отчёта.");
        }
    }
}

static void save_report(void)
{
    WCHAR report_path[MAX_PATH * 2];
    WCHAR filename[160];
    WCHAR finding_key[64];
    WCHAR engine_version[32];
    WCHAR report_schema[64];
    WCHAR pattern_key[32];
    WCHAR quality_key[64];
    WCHAR backup_age_text[64] = L"нет данных";
    WCHAR report[REPORT_CHARS];
    SYSTEMTIME now;
    int count;
    UINT i;
    size_t used = 0;
    UINT recent_arrivals = 0, recent_removals = 0;

    if (!sambu_authorize(SAMBU_OP_WRITE_PRIVATE_REPORT, SAMBU_ORIGIN_USER)) {
        set_status(L"Создание отчёта требует явного действия пользователя.");
        return;
    }
    if (!g_app.scan_complete) refresh_diagnostics();
    if (!g_app.reports_dir[0]) {
        set_status(L"Не удалось открыть локальную папку отчётов.");
        return;
    }
    get_recent_usb_counts(&recent_arrivals, &recent_removals);
    copy_ascii_to_wide(finding_key, ARRAYSIZE(finding_key),
                       sambu_finding_key(g_app.sambu_assessment.finding));
    copy_ascii_to_wide(engine_version, ARRAYSIZE(engine_version), sambu_engine_version());
    copy_ascii_to_wide(report_schema, ARRAYSIZE(report_schema), sambu_report_schema());
    copy_ascii_to_wide(pattern_key, ARRAYSIZE(pattern_key),
        sambu_pattern_key(sambu_ledger_pattern(&g_app.sambu_ledger,
            (uint32_t)g_app.sambu_assessment.finding, GetTickCount64())));
    copy_ascii_to_wide(quality_key, ARRAYSIZE(quality_key),
        sambu_quality_key(g_app.sambu_assessment.quality_flags));
    if (g_app.backup_age_days != ~(ULONGLONG)0) {
        swprintf_s(backup_age_text, ARRAYSIZE(backup_age_text),
            L"%llu дней", (unsigned long long)g_app.backup_age_days);
    }
    GetLocalTime(&now);
    swprintf_s(filename, ARRAYSIZE(filename),
        L"ToolzNG-report-%04u%02u%02u-%02u%02u%02u-%03u.txt",
        now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute,
        now.wSecond, now.wMilliseconds);
    append_path(report_path, ARRAYSIZE(report_path), g_app.reports_dir, filename);

    count = swprintf_s(report, ARRAYSIZE(report),
        L"ToolzNG — приватный диагностический отчёт\r\n"
        L"==========================================\r\n"
        L"Дата: %04u-%02u-%02u %02u:%02u:%02u\r\n"
        L"Автор: ToolzNG (локальная генерация)\r\n"
        L"Платформа: Windows\r\n"
        L"SAMBU engine: %ls\r\n"
        L"Diagnostic schema: %ls\r\n"
        L"SAMBU finding code: %u\r\n"
        L"SAMBU finding key: %ls\r\n"
        L"SAMBU confidence: %u%%\r\n"
        L"SAMBU readiness: %d/100\r\n"
        L"SAMBU evidence mask: 0x%08lX\r\n"
        L"SAMBU evidence quality: %ls (0x%08lX)\r\n"
        L"SAMBU memory: %u unique findings; current state observed %u time(s)\r\n"
        L"SAMBU recurrence (10 min): %u; pattern: %ls\r\n\r\n"
        L"USB-устройства Apple (VID_05AC): %u\r\n",
        now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute,
        now.wSecond, engine_version, report_schema,
        (unsigned)g_app.sambu_assessment.finding, finding_key,
        g_app.sambu_assessment.confidence_percent,
        g_app.sambu_assessment.readiness_score,
        (unsigned long)g_app.sambu_assessment.evidence_mask,
        quality_key,
        (unsigned long)g_app.sambu_assessment.quality_flags,
        (unsigned)sambu_ledger_unique_count(&g_app.sambu_ledger),
        (unsigned)sambu_ledger_occurrences(&g_app.sambu_ledger,
            (uint32_t)g_app.sambu_assessment.finding),
        (unsigned)sambu_ledger_recent_occurrences(&g_app.sambu_ledger,
            (uint32_t)g_app.sambu_assessment.finding, GetTickCount64()),
        pattern_key,
        g_app.device_count);
    if (count < 0) {
        set_status(L"Не удалось сформировать отчёт: ошибка форматирования.");
        return;
    }
    used = (size_t)count;
    for (i = 0; i < g_app.device_count && used < ARRAYSIZE(report) - 512; ++i) {
        count = swprintf_s(report + used, ARRAYSIZE(report) - used,
            L"\r\n[%u] %ls\r\n    Производитель: %ls\r\n",
            i + 1, g_app.devices[i].name,
            g_app.devices[i].manufacturer[0] ? g_app.devices[i].manufacturer : L"не указан");
        if (count < 0) break;
        used += (size_t)count;
    }
    count = swprintf_s(report + used, ARRAYSIZE(report) - used,
        L"\r\nСлужба Apple: %ls\r\n"
        L"Папка резервных копий: %ls\r\n"
        L"Backup Sentinel: %ls\r\n"
        L"Backup folders: %lu; marker-valid: %lu\r\n"
        L"Возраст новейшего Info.plist: %ls (оценка по времени изменения)\r\n"
        L"Duck Readiness: %d/100 (оценка Windows-side сигналов)\r\n"
        L"USB Pulse за 10 минут: %u подключений / %u отключений\r\n"
        L"LinkDNA baseline: %ls\r\n"
        L"LinkDNA изменился: %ls\r\n\r\n"
        L"Приватность и границы\r\n"
        L"---------------------\r\n"
        L"Hardware ID и точный путь backup в приватный отчёт не включаются.\r\n"
        L"Отчёт читает только сведения Windows PnP, статус службы и метаданные папок.\r\n"
        L"Он не читает личные данные iPhone/iPad, не проверяет содержимое backup,\r\n"
        L"не изменяет прошивку, не удаляет файлы и не выполняет jailbreak.\r\n",
        g_app.service_status,
        g_app.backup_found ? L"[локальный backup обнаружен; путь скрыт]" : L"не найдена",
        g_app.backup_audit,
        (unsigned long)g_app.backup_folder_count,
        (unsigned long)g_app.backup_valid_count,
        backup_age_text,
        g_app.readiness_score, recent_arrivals, recent_removals,
        g_app.link_baseline_known ? L"инициализирован" : L"не создан",
        g_app.link_profile_changed ? L"да" : L"нет");
    if (count < 0) {
        set_status(L"Не удалось завершить формирование отчёта.");
        return;
    }
    if (!write_utf8_file(report_path, report)) {
        set_status(L"Ошибка записи отчёта во внутреннее хранилище.");
        return;
    }
    (void)write_sha256_seal(report_path);
    refresh_report_history();
    g_app.page = PAGE_REPORTS;
    set_status(L"Отчёт создан и сохранён внутри ToolzNG. Предпросмотр открыт.");
    InvalidateRect(g_app.hwnd, NULL, FALSE);
}

static void draw_header(HDC dc, int width)
{
    RECT header = { 0, 0, width, 88 };
    RECT logo = { 28, 22, 66, 60 };
    RECT status = { width - 220, 28, width - 28, 60 };
    g_app.zoom_minus_button = (RECT){ width - 360, 28, width - 322, 60 };
    g_app.zoom_plus_button = (RECT){ width - 316, 28, width - 278, 60 };
    HBRUSH brush = CreateSolidBrush(COL_HEADER);
    FillRect(dc, &header, brush);
    DeleteObject(brush);

    draw_round_rect(dc, logo, COL_ACCENT_DARK, COL_ACCENT, 11);
    draw_text(dc, L"T", logo, g_app.font_brand, COL_TEXT,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"ToolzNG", (RECT){ 80, 20, 260, 55 },
              g_app.font_title, COL_TEXT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"APPLE DEVICE DIAGNOSTICS  ·  v0.6.1", (RECT){ 82, 54, 390, 74 },
              g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    draw_button(dc, g_app.zoom_minus_button, L"A−", FALSE);
    draw_button(dc, g_app.zoom_plus_button, L"A+", FALSE);
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
    const WCHAR *labels[PAGE_COUNT] = {
        L"Обзор", L"USB-устройства", L"Оптимизация",
        L"О программе", L"Отчёты", L"Справка"
    };
    const WCHAR *hints[PAGE_COUNT] = {
        L"Панель состояния", L"Поиск Apple VID_05AC",
        L"Безопасные проверки", L"Версия и ограничения",
        L"История и предпросмотр", L"Руководство офлайн"
    };

    FillRect(dc, &side, brush);
    DeleteObject(brush);

    draw_text(dc, L"РАЗДЕЛЫ", (RECT){ 26, 112, 190, 136 },
              g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    for (i = 0; i < PAGE_COUNT; ++i) {
        int top = 148 + i * 56;
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

    draw_text(dc, L"TOOLZNG  0.6.1  /  C17", (RECT){ 24, g_app.client_height - 48, 210,
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

static void format_quality_summary(WCHAR *buffer, size_t buffer_count)
{
    const WCHAR *state = g_app.sambu_assessment.quality_flags == 0u
        ? L"OK — сигналы согласованы"
        : L"ВНИМАНИЕ — см. DuckFix";
    swprintf_s(buffer, buffer_count,
        L"SAMBU Quality Gate · %ls · confidence %u%%",
        state, g_app.sambu_assessment.confidence_percent);
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
    WCHAR quality_line[220];
    int card_y = 416;

    draw_page_heading(dc, L"Центр управления", L"Проверяй подключение и обслуживай устройства Apple без рискованных вмешательств.");
    draw_round_rect(dc, hero, COL_PANEL, COL_BORDER, 16);

    draw_text(dc, L"СОСТОЯНИЕ ПОДКЛЮЧЕНИЯ", (RECT){ x + 24, 208, right - 24, 232 },
              g_app.font_small, COL_ACCENT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    {
        WCHAR score_line[128];
        swprintf_s(score_line, ARRAYSIZE(score_line),
            L"DUCK READINESS  ·  %d/100  ·  confidence %u%%",
            g_app.sambu_assessment.readiness_score,
            g_app.sambu_assessment.confidence_percent);
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
    draw_text(dc, L"Рекомендации iOS", (RECT){ g_app.care_button.left + 16,
              card_y + 53, g_app.care_button.right - 12, card_y + 82 },
              g_app.font_button, COL_TEXT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"Практические советы по настройкам и обслуживанию iOS — без ложных измерений.",
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
    format_quality_summary(quality_line, ARRAYSIZE(quality_line));
    draw_text(dc, quality_line, (RECT){ x + 2, 669, right, 691 },
        g_app.font_small,
        g_app.sambu_assessment.quality_flags == 0u ? COL_GREEN : COL_AMBER,
        DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
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

    {
        WCHAR quality_line[220];
        WCHAR backup_line[160];
        format_quality_summary(quality_line, ARRAYSIZE(quality_line));
        swprintf_s(backup_line, ARRAYSIZE(backup_line),
            L"Backup markers: %lu / %lu folders verified",
            (unsigned long)g_app.backup_valid_count,
            (unsigned long)g_app.backup_folder_count);
        draw_round_rect(dc, (RECT){ x, 262, right, 314 }, COL_CARD, COL_BORDER, 10);
        draw_text(dc, quality_line, (RECT){ x + 16, 268, right - 16, 288 },
            g_app.font_small,
            g_app.sambu_assessment.quality_flags == 0u ? COL_GREEN : COL_AMBER,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        draw_text(dc, backup_line, (RECT){ x + 16, 290, right - 16, 308 },
            g_app.font_small, COL_MUTED,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    }

    if (g_app.device_count == 0) {
        draw_round_rect(dc, (RECT){ x, 326, right, 426 }, COL_CARD, COL_BORDER, 12);
        draw_text(dc, g_app.scan_complete ? L"Устройства Apple не найдены" : L"Нажми «Повторить поиск»",
                  (RECT){ x + 20, 344, right - 20, 378 },
                  g_app.font_title, COL_TEXT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        draw_text(dc, L"Проверь разблокировку iPhone/iPad, кабель с передачей данных и драйвер Apple Devices.",
                  (RECT){ x + 20, 383, right - 20, 412 },
                  g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    } else {
        for (i = 0; i < (int)g_app.device_count && i < 4; ++i) {
            int top = 326 + i * 74;
            RECT item = { x, top, right, top + 68 };
            WCHAR id_line[600];
            draw_round_rect(dc, item, COL_CARD, COL_BORDER, 10);
            draw_text(dc, g_app.devices[i].name,
                      (RECT){ x + 18, top + 6, right - 18, top + 31 },
                      g_app.font_button, COL_TEXT, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            swprintf_s(id_line, ARRAYSIZE(id_line), L"%ls  |  %ls",
                g_app.devices[i].manufacturer[0] ? g_app.devices[i].manufacturer : L"Apple",
                g_app.devices[i].hardware_id);
            draw_text(dc, id_line,
                      (RECT){ x + 18, top + 34, right - 18, top + 61 },
                      g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        }
        if (g_app.device_count > 4) {
            WCHAR more[96];
            swprintf_s(more, ARRAYSIZE(more), L"Дополнительных записей: %u",
                       g_app.device_count - 4);
            draw_text(dc, more, (RECT){ x + 2, 620, right, 642 },
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
    g_app.playbook_button = (RECT){ x + 250, 651, x + 490, 691 };
    draw_button(dc, g_app.support_button, L"Локальная справка", FALSE);
    draw_button(dc, g_app.playbook_button, L"Сохранить маршрут", TRUE);
}

static void draw_reports(HDC dc)
{
    int x = 250, right = g_app.client_width - 28;
    int list_right = x + 196;
    RECT list_panel = { x, 188, list_right, g_app.client_height - 24 };
    RECT preview_panel = { list_right + 14, 188, right, g_app.client_height - 24 };
    RECT content_rect;
    int i, preview_top = 246;
    int preview_bottom = g_app.client_height - 46;

    draw_page_heading(dc, L"Внутренние отчёты",
        L"История хранится локально. Создание и просмотр не запускают сторонние приложения.");
    draw_round_rect(dc, list_panel, COL_PANEL, COL_BORDER, 12);
    draw_round_rect(dc, preview_panel, COL_PANEL, COL_BORDER, 12);
    draw_text(dc, L"ИСТОРИЯ", (RECT){ x + 14, 202, list_right - 12, 228 },
              g_app.font_small, COL_ACCENT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    {
        WCHAR summary[64];
        swprintf_s(summary, ARRAYSIZE(summary), L"%d отчётов", g_app.report_count);
        draw_text(dc, summary, (RECT){ x + 14, 228, list_right - 12, 250 },
                  g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
    for (i = 0; i < REPORT_ROWS; ++i) {
        int index = i + g_app.report_list_scroll;
        int top = 260 + i * 48;
        if (index < g_app.report_count) {
            g_app.report_row_index[i] = index;
            g_app.report_rows[i] = (RECT){ x + 9, top, list_right - 9, top + 42 };
            if (index == g_app.selected_report)
                draw_round_rect(dc, g_app.report_rows[i], COL_CARD_HOVER, COL_ACCENT, 7);
            else
                draw_round_rect(dc, g_app.report_rows[i], COL_CARD, COL_BORDER, 7);
            draw_text(dc, g_app.reports[index].name,
                (RECT){ x + 17, top + 3, list_right - 16, top + 39 },
                g_app.font_small, index == g_app.selected_report ? COL_ACCENT : COL_TEXT,
                DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        } else {
            g_app.report_row_index[i] = -1;
            g_app.report_rows[i] = (RECT){ 0, 0, 0, 0 };
        }
    }

    draw_text(dc, L"ПРЕДПРОСМОТР", (RECT){ list_right + 30, 202, right - 180, 228 },
              g_app.font_small, COL_ACCENT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    g_app.report_refresh_button = (RECT){ right - 154, 198, right - 20, 232 };
    draw_button(dc, g_app.report_refresh_button, L"Обновить", FALSE);
    draw_text(dc, g_app.last_report_path[0] ? g_app.last_report_path : L"Отчёт не выбран",
              (RECT){ list_right + 30, 228, right - 18, 246 },
              g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    content_rect = (RECT){ list_right + 30, preview_top - g_app.report_scroll,
                           right - 18, preview_top + 6000 };
    {
        HGDIOBJ old_font = SelectObject(dc, g_app.font_small);
        RECT measured = { 0, 0, (preview_panel.right - preview_panel.left) - 48, 6000 };
        DrawTextW(dc, g_app.report_content, -1, &measured,
                  DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);
        g_app.report_content_height = measured.bottom - measured.top;
        SelectObject(dc, old_font);
    }
    {
        int saved = SaveDC(dc);
        IntersectClipRect(dc, preview_panel.left + 12, preview_top,
                          preview_panel.right - 12, preview_bottom);
        draw_text(dc, g_app.report_content, content_rect, g_app.font_small,
                  COL_TEXT, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);
        RestoreDC(dc, saved);
    }
    g_app.report_copy_button = (RECT){ right - 170, g_app.client_height - 66,
                                       right - 20, g_app.client_height - 30 };
    g_app.report_export_button = (RECT){ right - 342, g_app.client_height - 66,
                                         right - 182, g_app.client_height - 30 };
    draw_button(dc, g_app.report_export_button, L"Экспорт TXT", FALSE);
    draw_button(dc, g_app.report_copy_button, L"Копировать", TRUE);
    draw_text(dc, L"Колёсико мыши / PgUp / PgDn — прокрутка",
        (RECT){ list_right + 30, g_app.client_height - 66, right - 352,
                g_app.client_height - 30 }, g_app.font_small, COL_MUTED,
        DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
}

static void draw_help(HDC dc)
{
    int x = 250, right = g_app.client_width - 28;
    RECT panel = { x, 188, right, g_app.client_height - 24 };
    draw_page_heading(dc, L"Справка ToolzNG",
        L"Локальное руководство — доступно без браузера и подключения к сети.");
    draw_round_rect(dc, panel, COL_PANEL, COL_BORDER, 14);
    draw_bullet(dc, 210, L"Горячие клавиши и масштаб",
        L"F5 запускает повторное USB-сканирование, Ctrl + R создаёт локальный отчёт, F1 открывает эту справку. Кнопки A− / A+ и Ctrl + плюс / минус меняют масштаб; Ctrl + 0 возвращает 100%.");
    draw_bullet(dc, 282, L"Отчёты и история",
        L"Создай отчёт на главном экране. Приватная версия автоматически сохранится в %LOCALAPPDATA%\\\\ToolzNG\\\\Reports и откроется во вкладке «Отчёты». История сохраняет последние 32 файла.");
    draw_bullet(dc, 354, L"Предпросмотр и копирование",
        L"Выбери отчёт слева. Прокручивай содержимое колесом мыши или Page Up / Page Down. Кнопка «Копировать» помещает текст в буфер обмена; экспорт в файл остаётся необязательным.");
    draw_bullet(dc, 426, L"Навигация без браузера",
        L"Разделы «Справка», «Отчёты» и «О программе» встроены в окно. ToolzNG не запускает внешний браузер для ежедневной работы. Открытие папки backup по-прежнему использует Проводник Windows.");
    draw_bullet(dc, 498, L"Приватность и хранилище",
        L"Настройки масштаба, история USB и отчёты остаются в локальном профиле Windows. Приватный отчёт скрывает Hardware ID и точный путь резервной копии. Удаление отчётов вручную не затрагивает настройки или данные устройства.");
    draw_bullet(dc, 570, L"Границы диагностики",
        L"Приложение проверяет видимые Windows USB/PnP-сигналы и метаданные backup. Оно не разгоняет iOS, не читает личные файлы, не удаляет данные, не выполняет jailbreak и не меняет прошивку.");
}

static void draw_about(HDC dc)
{
    int x = 250;
    int right = g_app.client_width - 28;
    RECT panel = { x, 188, right, 612 };

    draw_page_heading(dc, L"О ToolzNG", L"Нативная Windows-утилита на C17 и Win32 API.");
    draw_round_rect(dc, panel, COL_PANEL, COL_BORDER, 14);
    draw_text(dc, L"ToolzNG  0.6.1", (RECT){ x + 24, 216, right - 24, 254 },
              g_app.font_title, COL_TEXT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"SAMBU Engine 1.2.0 · Quality Gate · Recurrence Radar · 10 min",
              (RECT){ x + 24, 258, right - 24, 276 },
              g_app.font_small, COL_MUTED, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"Технологии", (RECT){ x + 24, 280, right - 24, 306 },
              g_app.font_button, COL_ACCENT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"C17 · Win32 API · GDI 2D · SetupAPI · Windows Service Control Manager · CMake",
              (RECT){ x + 24, 310, right - 24, 344 },
              g_app.font_body, COL_TEXT, DT_LEFT | DT_VCENTER | DT_WORDBREAK);
    draw_text(dc, L"Что программа делает", (RECT){ x + 24, 374, right - 24, 402 },
              g_app.font_button, COL_ACCENT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    draw_text(dc, L"• Duck Readiness: объяснимый балл готовности Windows-side подключения.\n"
                  L"• USB Pulse + PulseVault: события в реальном времени и локальная JSONL-история.\n"
                  L"• LinkDNA: локальное сравнение профиля Apple USB/службы без хранения серийных номеров.\n"
                  L"• Backup Sentinel + Freshness Radar: маркеры структуры и возраст Info.plist.\n"
                  L"• DuckFix Route: персональный план действий; Privacy Shield + Evidence Seal защищают экспорт.",
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
    {
        int physical_width = client.right - client.left;
        int physical_height = client.bottom - client.top;
        g_app.effective_dpi = effective_dpi();
        g_app.client_width = MulDiv(physical_width, 96, (int)g_app.effective_dpi);
        g_app.client_height = MulDiv(physical_height, 96, (int)g_app.effective_dpi);
        buffer = CreateCompatibleDC(target);
        bitmap = CreateCompatibleBitmap(target, physical_width, physical_height);
        old_bitmap = SelectObject(buffer, bitmap);
        background = CreateSolidBrush(COL_BG);
        FillRect(buffer, &client, background);
        DeleteObject(background);
        SetMapMode(buffer, MM_ANISOTROPIC);
        SetWindowExtEx(buffer, 96, 96, NULL);
        SetViewportExtEx(buffer, (int)g_app.effective_dpi,
                         (int)g_app.effective_dpi, NULL);
    }

    draw_header(buffer, g_app.client_width);
    draw_sidebar(buffer);

    switch (g_app.page) {
        case PAGE_DEVICES: draw_devices(buffer); break;
        case PAGE_CARE:    draw_care(buffer); break;
        case PAGE_ABOUT:   draw_about(buffer); break;
        case PAGE_REPORTS: draw_reports(buffer); break;
        case PAGE_HELP:    draw_help(buffer); break;
        case PAGE_OVERVIEW:
        default:           draw_overview(buffer); break;
    }

    SetMapMode(buffer, MM_TEXT);
    BitBlt(target, 0, 0, client.right - client.left, client.bottom - client.top,
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
            if (!sambu_authorize(SAMBU_OP_SCAN, SAMBU_ORIGIN_USER)) {
                set_status(L"Сканирование отклонено политикой безопасности.");
                break;
            }
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
        case ACTION_HELP:
            g_app.page = PAGE_HELP;
            InvalidateRect(g_app.hwnd, NULL, FALSE);
            break;
        case ACTION_PLAYBOOK:
            save_duckfix_route();
            break;
        default:
            break;
    }
}

static void handle_click(POINT point)
{
    int i;
    if (point_in_rect(point, g_app.zoom_minus_button)) { adjust_zoom(-10); return; }
    if (point_in_rect(point, g_app.zoom_plus_button)) { adjust_zoom(10); return; }
    for (i = 0; i < PAGE_COUNT; ++i) {
        if (point_in_rect(point, g_app.nav[i])) {
            g_app.page = i;
            if (g_app.page == PAGE_REPORTS) refresh_report_history();
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
        if (point_in_rect(point, g_app.support_button)) handle_action(ACTION_HELP);
        else if (point_in_rect(point, g_app.playbook_button)) handle_action(ACTION_PLAYBOOK);
    } else if (g_app.page == PAGE_REPORTS) {
        int row;
        for (row = 0; row < REPORT_ROWS; ++row) {
            int selected_index = g_app.report_row_index[row];
            if (selected_index >= 0 && point_in_rect(point, g_app.report_rows[row])) {
                (void)load_report_file(selected_index);
                InvalidateRect(g_app.hwnd, NULL, FALSE);
                return;
            }
        }
        if (point_in_rect(point, g_app.report_refresh_button)) {
            refresh_report_history();
            InvalidateRect(g_app.hwnd, NULL, FALSE);
        } else if (point_in_rect(point, g_app.report_copy_button)) {
            set_status(copy_report_to_clipboard()
                ? L"Содержимое отчёта скопировано в буфер обмена."
                : L"Не удалось скопировать отчёт в буфер обмена.");
        } else if (point_in_rect(point, g_app.report_export_button)) {
            export_report_copy();
        }
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
            sambu_ledger_init(&g_app.sambu_ledger);
            g_app.page = PAGE_OVERVIEW;
            g_app.scan_complete = FALSE;
            g_app.app_started_at = GetTickCount64();
            g_app.dpi = GetDpiForWindow(hwnd);
            if (!g_app.dpi) g_app.dpi = 96;
            g_app.effective_dpi = effective_dpi();
            create_fonts();
            initialize_local_store();
            load_display_settings();
            migrate_legacy_reports();
            update_high_contrast();
            g_app.effective_dpi = effective_dpi();
            register_usb_watch(hwnd);
            refresh_report_history();
            SetTimer(hwnd, TIMER_READINESS, (UINT)USB_STABILITY_WARMUP_MS, NULL);
            copy_text(g_app.status, ARRAYSIZE(g_app.status),
                      L"Готово к проверке. Подключи iPhone/iPad через USB.");
            refresh_diagnostics();
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
            update_high_contrast();
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;

        case WM_PAINT:
            paint_window(hwnd);
            return 0;

        case WM_SIZE:
            g_app.effective_dpi = effective_dpi();
            g_app.client_width = MulDiv(LOWORD(lparam), 96, (int)g_app.effective_dpi);
            g_app.client_height = MulDiv(HIWORD(lparam), 96, (int)g_app.effective_dpi);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;

        case WM_DPICHANGED: {
            const RECT *suggested = (const RECT *)lparam;
            g_app.dpi = HIWORD(wparam);
            if (!g_app.dpi) g_app.dpi = 96;
            g_app.effective_dpi = effective_dpi();
            delete_fonts();
            create_fonts();
            SetWindowPos(hwnd, NULL, suggested->left, suggested->top,
                suggested->right - suggested->left,
                suggested->bottom - suggested->top,
                SWP_NOZORDER | SWP_NOACTIVATE);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_MOUSEWHEEL:
            if (g_app.page == PAGE_REPORTS) {
                POINT cursor;
                int direction = GET_WHEEL_DELTA_WPARAM(wparam);
                GetCursorPos(&cursor);
                ScreenToClient(hwnd, &cursor);
                cursor.x = MulDiv(cursor.x, 96, (int)effective_dpi());
                cursor.y = MulDiv(cursor.y, 96, (int)effective_dpi());
                if (cursor.x < 455) {
                    g_app.report_list_scroll += direction < 0 ? 1 : -1;
                    if (g_app.report_list_scroll < 0) g_app.report_list_scroll = 0;
                    if (g_app.report_list_scroll > g_app.report_count - REPORT_ROWS)
                        g_app.report_list_scroll = g_app.report_count > REPORT_ROWS
                            ? g_app.report_count - REPORT_ROWS : 0;
                } else {
                    g_app.report_scroll -= direction > 0 ? 96 : -96;
                    if (g_app.report_scroll < 0) g_app.report_scroll = 0;
                    if (g_app.report_scroll > g_app.report_content_height)
                        g_app.report_scroll = g_app.report_content_height;
                }
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
            return DefWindowProcW(hwnd, message, wparam, lparam);

        case WM_KEYDOWN:
            if (wparam == VK_F1) {
                g_app.page = PAGE_HELP;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
            if (wparam == VK_F5) {
                handle_action(ACTION_SCAN);
                return 0;
            }
            if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
                if (wparam == L'R') { handle_action(ACTION_REPORT); return 0; }
                if (wparam == VK_OEM_PLUS || wparam == VK_ADD) { adjust_zoom(10); return 0; }
                if (wparam == VK_OEM_MINUS || wparam == VK_SUBTRACT) { adjust_zoom(-10); return 0; }
                if (wparam == L'0') {
                    g_app.zoom_percent = 100;
                    g_app.effective_dpi = effective_dpi();
                    save_display_settings();
                    delete_fonts(); create_fonts();
                    InvalidateRect(hwnd, NULL, FALSE);
                    return 0;
                }
            }
            if (g_app.page == PAGE_REPORTS && (wparam == VK_PRIOR || wparam == VK_NEXT)) {
                if ((GetKeyState(VK_SHIFT) & 0x8000) != 0) {
                    g_app.report_list_scroll += wparam == VK_PRIOR ? -REPORT_ROWS : REPORT_ROWS;
                    if (g_app.report_list_scroll < 0) g_app.report_list_scroll = 0;
                    if (g_app.report_list_scroll > g_app.report_count - REPORT_ROWS)
                        g_app.report_list_scroll = g_app.report_count > REPORT_ROWS
                            ? g_app.report_count - REPORT_ROWS : 0;
                } else {
                    g_app.report_scroll += wparam == VK_PRIOR ? -320 : 320;
                    if (g_app.report_scroll < 0) g_app.report_scroll = 0;
                    if (g_app.report_scroll > g_app.report_content_height)
                        g_app.report_scroll = g_app.report_content_height;
                }
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
            return DefWindowProcW(hwnd, message, wparam, lparam);

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
            g_app.effective_dpi = effective_dpi();
            point.x = MulDiv(GET_X_LPARAM(lparam), 96, (int)g_app.effective_dpi);
            point.y = MulDiv(GET_Y_LPARAM(lparam), 96, (int)g_app.effective_dpi);
            handle_click(point);
            return 0;
        }

        case WM_GETMINMAXINFO: {
            MINMAXINFO *info = (MINMAXINFO *)lparam;
            g_app.effective_dpi = effective_dpi();
            info->ptMinTrackSize.x = MulDiv(840, (int)g_app.effective_dpi, 96);
            info->ptMinTrackSize.y = MulDiv(720, (int)g_app.effective_dpi, 96);
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
    if (!sambu_architecture_validate()) {
        MessageBoxW(NULL, L"Проверка архитектуры SAMBU не пройдена. Запуск остановлен.",
                    APP_TITLE, MB_OK | MB_ICONERROR);
        return 2;
    }
    (void)SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    g_app.dpi = GetDpiForSystem();
    if (!g_app.dpi) g_app.dpi = 96;
    initialize_local_store();
    load_display_settings();
    migrate_legacy_reports();
    update_high_contrast();
    g_app.effective_dpi = effective_dpi();
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
        MulDiv(1120, (int)g_app.effective_dpi, 96),
        MulDiv(790, (int)g_app.effective_dpi, 96),
        NULL, NULL, instance, NULL);
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