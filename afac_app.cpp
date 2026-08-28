#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <gdiplus.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <thread>
#include <atomic>
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "advapi32.lib")

#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

namespace fs = std::filesystem;

#define WM_APP_LOG           (WM_APP + 1)
#define WM_APP_ROW_STATUS    (WM_APP + 2)
#define WM_APP_PROGRESS      (WM_APP + 3)
#define WM_APP_TASK_FINISHED (WM_APP + 4)

enum ControlIDs {
    ID_MENU_ADD_FILES = 1001,
    ID_MENU_ADD_FOLDER,
    ID_MENU_CLEAR,
    ID_MENU_EXIT,
    ID_MENU_PREFS,
    ID_MENU_ABOUT,

    IDC_FILE_LIST = 2001,
    IDC_BTN_ADD_FILES,
    IDC_BTN_ADD_FOLDER,
    IDC_BTN_CLEAR,

    IDC_GRP_OPTIONS,
    IDC_CHK_STRIP_META,
    IDC_CHK_RAND_HASH,
    IDC_CHK_RAND_NAMES,
    IDC_CHK_OPEN_FOLDER,

    IDC_TXT_LOG,
    IDC_PROGRESS_BAR,
    IDC_BTN_START,
    IDC_BTN_STOP,

    IDC_LABEL_DROP_HINT,
    IDC_LABEL_LOG
};

struct Translation {
    std::wstring app_title;
    std::wstring menu_file;
    std::wstring menu_settings;
    std::wstring menu_help;
    std::wstring action_add_files;
    std::wstring action_add_folder;
    std::wstring action_clear;
    std::wstring action_exit;
    std::wstring action_preferences;
    std::wstring action_about;
    std::wstring drop_zone_hint;
    std::wstring col_name;
    std::wstring col_size;
    std::wstring col_type;
    std::wstring col_status;
    std::wstring status_ready;
    std::wstring status_processing;
    std::wstring status_done;
    std::wstring status_error;
    std::wstring btn_add_files;
    std::wstring btn_add_folder;
    std::wstring btn_clear;
    std::wstring group_options;
    std::wstring opt_strip_meta;
    std::wstring opt_rand_hash;
    std::wstring opt_rand_names;
    std::wstring opt_open_folder;
    std::wstring log_title;
    std::wstring btn_start;
    std::wstring btn_stop;
    std::wstring settings_title;
    std::wstring settings_language;
    std::wstring settings_save;
    std::wstring settings_cancel;
    std::wstring log_start;
    std::wstring log_ffmpeg_missing;
    std::wstring log_ffmpeg_found;
    std::wstring log_processed_img;
    std::wstring log_processed_vid;
    std::wstring log_skipped_unsupported;
    std::wstring log_skipped_no_ffmpeg;
    std::wstring log_error;
    std::wstring log_canceled;
    std::wstring log_finished;
    std::wstring log_empty_list;
    std::wstring about_title;
    std::wstring about_text;
};

std::unordered_map<std::wstring, Translation> g_translations = {
    {
        L"en", {
            L"AFAC - Anti-Fingerprint Asset Cleaner", L"File", L"Settings", L"Help",
            L"Add Files...", L"Add Folder...", L"Clear File List", L"Exit", L"Preferences...", L"About AFAC",
            L"Drag and drop media files or folders into the table below:",
            L"File Name", L"Size", L"Type", L"Status",
            L"Pending", L"Processing", L"Completed", L"Error",
            L"Add Files", L"Add Folder", L"Clear List",
            L"Processing Options",
            L"Strip metadata (EXIF, IPTC, XMP, stream tags)",
            L"Randomize hash (MD5 / SHA-256) via micro-shift",
            L"Generate random output filenames",
            L"Open output directory upon completion",
            L"Execution Log:",
            L"START PROCESSING", L"STOP",
            L"Preferences", L"Interface Language:", L"Save", L"Cancel",
            L"Process initiated. Total files to process: ",
            L"FFmpeg was not detected in PATH or application directory. Video processing will fail.",
            L"FFmpeg detected at: ",
            L"Image processed successfully: ",
            L"Video processed successfully: ",
            L"File skipped due to unsupported extension: ",
            L"Video skipped because FFmpeg is missing: ",
            L"Error processing ",
            L"Process stopped by user.",
            L"Task completed. Successfully processed: ",
            L"The file list is empty. Add files before starting.",
            L"About AFAC",
            L"AFAC (Anti-Fingerprint Asset Cleaner)\n\nUtility for batch metadata removal and unique content generation for image and video assets.\n\nVersion: 2.1.0"
        }
    },
    {
        L"ru", {
            L"AFAC - Anti-Fingerprint Asset Cleaner", L"Файл", L"Настройки", L"Справка",
            L"Добавить файлы...", L"Добавить папку...", L"Очистить список файлов", L"Выход", L"Параметры...", L"О программе",
            L"Перетащите файлы или папки в таблицу ниже:",
            L"Имя файла", L"Размер", L"Тип", L"Статус",
            L"В очереди", L"Обработка", L"Завершено", L"Ошибка",
            L"Добавить файлы", L"Добавить папку", L"Очистить список",
            L"Параметры обработки",
            L"Удалить метаданные (EXIF, IPTC, XMP, теги потоков)",
            L"Рандомизировать хэш (MD5 / SHA-256) микро-сдвигом",
            L"Генерировать случайные имена файлов",
            L"Открыть папку с результатом после завершения",
            L"Журнал выполнения:",
            L"НАЧАТЬ ОБРАБОТКУ", L"ОСТАНОВИТЬ",
            L"Параметры", L"Язык интерфейса:", L"Сохранить", L"Отмена",
            L"Процесс запущен. Всего файлов для обработки: ",
            L"FFmpeg не обнаружен в PATH или каталоге программы. Обработка видео невозможна.",
            L"FFmpeg обнаружен по пути: ",
            L"Изображение обработано: ",
            L"Видео обработано: ",
            L"Файл пропущен (неподдерживаемый формат): ",
            L"Видео пропущено из-за отсутствия FFmpeg: ",
            L"Ошибка обработки ",
            L"Процесс остановлен пользователем.",
            L"Задача завершена. Успешно обработано: ",
            L"Список файлов пуст. Добавьте файлы перед запуском.",
            L"О программе",
            L"AFAC (Anti-Fingerprint Asset Cleaner)\n\nУтилита для пакетной очистки метаданных и уникализации медиафайлов (изображений и видео).\n\nВерсия: 2.1.0"
        }
    },
    {
        L"et", {
            L"AFAC - Anti-Fingerprint Asset Cleaner", L"Fail", L"Seaded", L"Abi",
            L"Lisa failid...", L"Lisa kaust...", L"Tühjenda loend", L"Välju", L"Eelistused...", L"Programmist",
            L"Lohistage failid või kaustad allolevasse tabelisse:",
            L"Faili nimi", L"Suurus", L"Tüüp", L"Olek",
            L"Ootel", L"Töötlemine", L"Lõpetatud", L"Viga",
            L"Lisa failid", L"Lisa kaust", L"Tühjenda",
            L"Töötlemise valikud",
            L"Eemalda metaandmed (EXIF, IPTC, XMP, vootähised)",
            L"Muuda faili räsi (MD5 / SHA-256) mikronihkega",
            L"Loo juhuslikud failinimed",
            L"Ava väljundkaust pärast lõpetamist",
            L"Töölogi:",
            L"ALUSTA TÖÖTLEMIST", L"PEATAGE",
            L"Eelistused", L"Kasutajaliidese keel:", L"Salvesta", L"Loobu",
            L"Protsess käivitatud. Failide koguarv: ",
            L"FFmpeg programm ei leitud PATH-ist ega rakenduse kaustast. Video töötlemine ebaõnnestub.",
            L"FFmpeg leitud asukohast: ",
            L"Pilt edukalt töödeldud: ",
            L"Video edukalt töödeldud: ",
            L"Fail vahele jäetud (toetamata formaat): ",
            L"Video vahele jäetud FFmpegi puudumise tõttu: ",
            L"Viga faili töötlemisel: ",
            L"Kasutaja peatas protsessi.",
            L"Töö lõpetatud. Edukalt töödeldud: ",
            L"Failide loend on tühi. Lisage failid enne alustamist.",
            L"Programmist",
            L"AFAC (Anti-Fingerprint Asset Cleaner)\n\nUtiliit pildi- ja videofailide metaandmete puhastamiseks ja unikaalsuse tagamiseks.\n\nVersioon: 2.1.0"
        }
    }
};

const std::unordered_set<std::wstring> IMAGE_EXTENSIONS = { L".jpg", L".jpeg", L".png", L".webp", L".bmp", L".tiff" };
const std::unordered_set<std::wstring> VIDEO_EXTENSIONS = { L".mp4", L".mov", L".mkv", L".avi" };

HWND g_hWnd = NULL;
HWND g_hList = NULL;
HWND g_hBtnAddFiles = NULL, g_hBtnAddFolder = NULL, g_hBtnClear = NULL;
HWND g_hGrpOptions = NULL;
HWND g_hChkStripMeta = NULL, g_hChkRandHash = NULL, g_hChkRandNames = NULL, g_hChkOpenFolder = NULL;
HWND g_hTxtLog = NULL;
HWND g_hProgressBar = NULL;
HWND g_hBtnStart = NULL, g_hBtnStop = NULL;
HWND g_hLblDropHint = NULL, g_hLblLog = NULL;
HMENU g_hMenu = NULL;
HFONT g_hFont = NULL, g_hLogFont = NULL;

std::wstring g_currentLang = L"en";
std::vector<std::wstring> g_registeredPaths;
std::atomic<bool> g_isInterrupted(false);
std::thread g_workerThread;

void RetranslateUI();

std::wstring ToLower(std::wstring str) {
    for (auto& c : str) c = towlower(c);
    return str;
}

std::wstring FormatFileSize(uintmax_t bytes) {
    std::wstringstream ss;
    if (bytes < 1024) {
        ss << bytes << L" B";
    }
    else if (bytes < 1024 * 1024) {
        ss << std::fixed << std::setprecision(1) << (bytes / 1024.0) << L" KB";
    }
    else {
        ss << std::fixed << std::setprecision(1) << (bytes / (1024.0 * 1024.0)) << L" MB";
    }
    return ss.str();
}

std::wstring ResolveFFmpegBinary() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    fs::path appDir = fs::path(exePath).parent_path();
    fs::path localFfmpeg = appDir / L"ffmpeg.exe";
    if (fs::exists(localFfmpeg)) {
        return localFfmpeg.wstring();
    }
    wchar_t foundPath[MAX_PATH];
    if (SearchPathW(NULL, L"ffmpeg.exe", NULL, MAX_PATH, foundPath, NULL) > 0) {
        return std::wstring(foundPath);
    }
    return L"";
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0, size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;

    std::vector<BYTE> buffer(size);
    Gdiplus::ImageCodecInfo* pImageCodecInfo = reinterpret_cast<Gdiplus::ImageCodecInfo*>(buffer.data());
    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            return j;
        }
    }
    return -1;
}

void AppendLog(const std::wstring& text) {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    struct tm buf;
    localtime_s(&buf, &in_time_t);

    std::wstringstream ss;
    ss << L"[" << std::setfill(L'0') << std::setw(2) << buf.tm_hour << L":"
        << std::setfill(L'0') << std::setw(2) << buf.tm_min << L":"
        << std::setfill(L'0') << std::setw(2) << buf.tm_sec << L"] "
        << text << L"\r\n";

    std::wstring line = ss.str();
    int len = GetWindowTextLengthW(g_hTxtLog);
    SendMessageW(g_hTxtLog, EM_SETSEL, (WPARAM)len, (LPARAM)len);
    SendMessageW(g_hTxtLog, EM_REPLACESEL, FALSE, (LPARAM)line.c_str());
}

void ProcessImage(const fs::path& source, const fs::path& dest, bool randomizeHash) {
    std::mt19937 rng(std::random_device{}());

    Gdiplus::Bitmap* original = Gdiplus::Bitmap::FromFile(source.wstring().c_str());
    if (!original || original->GetLastStatus() != Gdiplus::Ok) {
        delete original;
        throw std::runtime_error("Failed to load image via GDI+");
    }

    UINT width = original->GetWidth();
    UINT height = original->GetHeight();
    UINT cropX = 0, cropY = 0;

    if (randomizeHash && width > 30 && height > 30) {
        std::uniform_int_distribution<int> cropDist(0, 1);
        cropX = cropDist(rng);
        cropY = cropDist(rng);
    }

    UINT newWidth = width - cropX;
    UINT newHeight = height - cropY;

    Gdiplus::Bitmap processed(newWidth, newHeight, PixelFormat32bppARGB);
    Gdiplus::Graphics g(&processed);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

    Gdiplus::ImageAttributes imgAttr;
    if (randomizeHash) {
        std::uniform_real_distribution<float> deltaDist(-0.004f, 0.004f);
        float contrast = 1.0f + deltaDist(rng);
        float brightness = deltaDist(rng);

        Gdiplus::ColorMatrix colorMatrix = {
            contrast, 0.0f,     0.0f,     0.0f, 0.0f,
            0.0f,     contrast, 0.0f,     0.0f, 0.0f,
            0.0f,     0.0f,     contrast, 0.0f, 0.0f,
            0.0f,     0.0f,     0.0f,     1.0f, 0.0f,
            brightness, brightness, brightness, 0.0f, 1.0f
        };
        imgAttr.SetColorMatrix(&colorMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);
    }

    Gdiplus::Rect destRect(0, 0, newWidth, newHeight);
    g.DrawImage(original, destRect, 0, 0, newWidth, newHeight, Gdiplus::UnitPixel, randomizeHash ? &imgAttr : NULL);
    delete original;

    std::wstring ext = ToLower(dest.extension().wstring());
    CLSID encoderClsid;
    Gdiplus::EncoderParameters encoderParams;
    ULONG quality = 95;

    if (ext == L".jpg" || ext == L".jpeg") {
        GetEncoderClsid(L"image/jpeg", &encoderClsid);
        if (randomizeHash) {
            std::uniform_int_distribution<ULONG> qDist(94, 97);
            quality = qDist(rng);
        }
        encoderParams.Count = 1;
        encoderParams.Parameter[0].Guid = Gdiplus::EncoderQuality;
        encoderParams.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
        encoderParams.Parameter[0].NumberOfValues = 1;
        encoderParams.Parameter[0].Value = &quality;

        processed.Save(dest.wstring().c_str(), &encoderClsid, &encoderParams);
    }
    else if (ext == L".png") {
        GetEncoderClsid(L"image/png", &encoderClsid);
        processed.Save(dest.wstring().c_str(), &encoderClsid, NULL);
    }
    else if (ext == L".bmp") {
        GetEncoderClsid(L"image/bmp", &encoderClsid);
        processed.Save(dest.wstring().c_str(), &encoderClsid, NULL);
    }
    else if (ext == L".tiff") {
        GetEncoderClsid(L"image/tiff", &encoderClsid);
        processed.Save(dest.wstring().c_str(), &encoderClsid, NULL);
    }
    else {
        GetEncoderClsid(L"image/jpeg", &encoderClsid);
        processed.Save(dest.wstring().c_str(), &encoderClsid, NULL);
    }
}

void ProcessVideo(const std::wstring& ffmpegBin, const fs::path& source, const fs::path& dest, bool stripMeta, bool randomizeHash) {
    std::wstringstream cmd;
    cmd << L"\"" << ffmpegBin << L"\" -y -i \"" << source.wstring() << L"\"";

    if (stripMeta) {
        cmd << L" -map_metadata -1 -map_chapters -1";
    }

    if (randomizeHash) {
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> crfDist(18, 21);
        cmd << L" -vf noise=alls=1:allf=t -af adelay=10|10 -c:v libx264 -crf " << crfDist(rng)
            << L" -preset fast -c:a aac -b:a 192k";
    }
    else {
        cmd << L" -c copy";
    }

    cmd << L" \"" << dest.wstring() << L"\"";

    STARTUPINFOW si = { sizeof(STARTUPINFOW) };
    PROCESS_INFORMATION pi = { 0 };
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    std::wstring cmdStr = cmd.str();
    std::vector<wchar_t> cmdBuf(cmdStr.begin(), cmdStr.end());
    cmdBuf.push_back(L'\0');

    if (!CreateProcessW(NULL, cmdBuf.data(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        throw std::runtime_error("Failed to start FFmpeg process.");
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (exitCode != 0) {
        throw std::runtime_error("FFmpeg exited with error code " + std::to_string(exitCode));
    }
}

struct ProcessOptions {
    bool stripMeta;
    bool randomizeHash;
    bool randomNames;
    bool openFolder;
};

void WorkerThreadFunc(std::vector<std::wstring> filePaths, ProcessOptions opts, std::wstring lang) {
    const Translation& tr = g_translations[lang];
    size_t total = filePaths.size();
    size_t successCount = 0;

    fs::path outDir = fs::current_path() / L"output";
    std::error_code ec;
    fs::create_directories(outDir, ec);

    std::wstring ffmpeg = ResolveFFmpegBinary();

    PostMessageW(g_hWnd, WM_APP_LOG, 0, (LPARAM)new std::wstring(tr.log_start + std::to_wstring(total)));
    if (!ffmpeg.empty()) {
        PostMessageW(g_hWnd, WM_APP_LOG, 0, (LPARAM)new std::wstring(tr.log_ffmpeg_found + ffmpeg));
    }
    else {
        PostMessageW(g_hWnd, WM_APP_LOG, 0, (LPARAM)new std::wstring(tr.log_ffmpeg_missing));
    }

    std::mt19937 rng(std::random_device{}());

    for (size_t i = 0; i < total; ++i) {
        if (g_isInterrupted.load()) {
            PostMessageW(g_hWnd, WM_APP_LOG, 0, (LPARAM)new std::wstring(tr.log_canceled));
            break;
        }

        fs::path src(filePaths[i]);
        std::wstring ext = ToLower(src.extension().wstring());

        PostMessageW(g_hWnd, WM_APP_ROW_STATUS, (WPARAM)i, (LPARAM)new std::wstring(tr.status_processing));

        try {
            std::wstring destName;
            if (opts.randomNames) {
                std::uniform_int_distribution<int> idDist(10000, 99999);
                std::wstring prefix = (VIDEO_EXTENSIONS.find(ext) != VIDEO_EXTENSIONS.end()) ? L"VID_" : L"IMG_";
                destName = prefix + std::to_wstring(idDist(rng)) + ext;
            }
            else {
                destName = src.stem().wstring() + L"_cleaned" + ext;
            }

            fs::path dst = outDir / destName;

            if (IMAGE_EXTENSIONS.find(ext) != IMAGE_EXTENSIONS.end()) {
                ProcessImage(src, dst, opts.randomizeHash);
                successCount++;
                PostMessageW(g_hWnd, WM_APP_ROW_STATUS, (WPARAM)i, (LPARAM)new std::wstring(tr.status_done));
                PostMessageW(g_hWnd, WM_APP_LOG, 0, (LPARAM)new std::wstring(tr.log_processed_img + src.filename().wstring() + L" -> " + destName));
            }
            else if (VIDEO_EXTENSIONS.find(ext) != VIDEO_EXTENSIONS.end()) {
                if (ffmpeg.empty()) {
                    PostMessageW(g_hWnd, WM_APP_ROW_STATUS, (WPARAM)i, (LPARAM)new std::wstring(tr.status_error));
                    PostMessageW(g_hWnd, WM_APP_LOG, 0, (LPARAM)new std::wstring(tr.log_skipped_no_ffmpeg + src.filename().wstring()));
                }
                else {
                    ProcessVideo(ffmpeg, src, dst, opts.stripMeta, opts.randomizeHash);
                    successCount++;
                    PostMessageW(g_hWnd, WM_APP_ROW_STATUS, (WPARAM)i, (LPARAM)new std::wstring(tr.status_done));
                    PostMessageW(g_hWnd, WM_APP_LOG, 0, (LPARAM)new std::wstring(tr.log_processed_vid + src.filename().wstring() + L" -> " + destName));
                }
            }
            else {
                PostMessageW(g_hWnd, WM_APP_ROW_STATUS, (WPARAM)i, (LPARAM)new std::wstring(tr.status_error));
                PostMessageW(g_hWnd, WM_APP_LOG, 0, (LPARAM)new std::wstring(tr.log_skipped_unsupported + src.filename().wstring()));
            }
        }
        catch (const std::exception& ex) {
            std::string errStr(ex.what());
            std::wstring wErr(errStr.begin(), errStr.end());
            PostMessageW(g_hWnd, WM_APP_ROW_STATUS, (WPARAM)i, (LPARAM)new std::wstring(tr.status_error));
            PostMessageW(g_hWnd, WM_APP_LOG, 0, (LPARAM)new std::wstring(tr.log_error + src.filename().wstring() + L": " + wErr));
        }

        PostMessageW(g_hWnd, WM_APP_PROGRESS, (WPARAM)(i + 1), (LPARAM)total);
    }

    std::wstring finishMsg = tr.log_finished + std::to_wstring(successCount) + L"/" + std::to_wstring(total) + L". Output: " + outDir.wstring();
    PostMessageW(g_hWnd, WM_APP_LOG, 0, (LPARAM)new std::wstring(finishMsg));
    PostMessageW(g_hWnd, WM_APP_TASK_FINISHED, (WPARAM)(opts.openFolder ? 1 : 0), (LPARAM)new std::wstring(outDir.wstring()));
}

void AddFileToTable(const fs::path& p) {
    std::wstring pathStr = p.wstring();
    for (const auto& reg : g_registeredPaths) {
        if (reg == pathStr) return;
    }

    std::wstring ext = ToLower(p.extension().wstring());
    bool isImg = (IMAGE_EXTENSIONS.find(ext) != IMAGE_EXTENSIONS.end());
    bool isVid = (VIDEO_EXTENSIONS.find(ext) != VIDEO_EXTENSIONS.end());
    if (!isImg && !isVid) return;

    g_registeredPaths.push_back(pathStr);
    int row = (int)g_registeredPaths.size() - 1;

    LVITEMW item = { 0 };
    item.mask = LVIF_TEXT;
    item.iItem = row;
    item.iSubItem = 0;
    std::wstring name = p.filename().wstring();
    item.pszText = const_cast<LPWSTR>(name.c_str());
    ListView_InsertItem(g_hList, &item);

    std::error_code ec;
    uintmax_t size = fs::file_size(p, ec);
    std::wstring sizeStr = FormatFileSize(size);
    ListView_SetItemText(g_hList, row, 1, const_cast<LPWSTR>(sizeStr.c_str()));

    std::wstring typeStr = isImg ? L"Image" : L"Video";
    ListView_SetItemText(g_hList, row, 2, const_cast<LPWSTR>(typeStr.c_str()));

    const Translation& tr = g_translations[g_currentLang];
    ListView_SetItemText(g_hList, row, 3, const_cast<LPWSTR>(tr.status_ready.c_str()));
}

void RetranslateUI() {
    const Translation& tr = g_translations[g_currentLang];

    SetWindowTextW(g_hWnd, tr.app_title.c_str());
    SetWindowTextW(g_hLblDropHint, tr.drop_zone_hint.c_str());
    SetWindowTextW(g_hBtnAddFiles, tr.btn_add_files.c_str());
    SetWindowTextW(g_hBtnAddFolder, tr.btn_add_folder.c_str());
    SetWindowTextW(g_hBtnClear, tr.btn_clear.c_str());

    SetWindowTextW(g_hGrpOptions, tr.group_options.c_str());
    SetWindowTextW(g_hChkStripMeta, tr.opt_strip_meta.c_str());
    SetWindowTextW(g_hChkRandHash, tr.opt_rand_hash.c_str());
    SetWindowTextW(g_hChkRandNames, tr.opt_rand_names.c_str());
    SetWindowTextW(g_hChkOpenFolder, tr.opt_open_folder.c_str());

    SetWindowTextW(g_hLblLog, tr.log_title.c_str());
    SetWindowTextW(g_hBtnStart, tr.btn_start.c_str());
    SetWindowTextW(g_hBtnStop, tr.btn_stop.c_str());

    LVCOLUMNW lvc = { 0 };
    lvc.mask = LVCF_TEXT;

    lvc.pszText = const_cast<LPWSTR>(tr.col_name.c_str());
    ListView_SetColumn(g_hList, 0, &lvc);
    lvc.pszText = const_cast<LPWSTR>(tr.col_size.c_str());
    ListView_SetColumn(g_hList, 1, &lvc);
    lvc.pszText = const_cast<LPWSTR>(tr.col_type.c_str());
    ListView_SetColumn(g_hList, 2, &lvc);
    lvc.pszText = const_cast<LPWSTR>(tr.col_status.c_str());
    ListView_SetColumn(g_hList, 3, &lvc);

    HMENU hMenu = CreateMenu();
    HMENU hFile = CreatePopupMenu();
    HMENU hSettings = CreatePopupMenu();
    HMENU hHelp = CreatePopupMenu();

    AppendMenuW(hFile, MF_STRING, ID_MENU_ADD_FILES, tr.action_add_files.c_str());
    AppendMenuW(hFile, MF_STRING, ID_MENU_ADD_FOLDER, tr.action_add_folder.c_str());
    AppendMenuW(hFile, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hFile, MF_STRING, ID_MENU_CLEAR, tr.action_clear.c_str());
    AppendMenuW(hFile, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hFile, MF_STRING, ID_MENU_EXIT, tr.action_exit.c_str());

    AppendMenuW(hSettings, MF_STRING, ID_MENU_PREFS, tr.action_preferences.c_str());
    AppendMenuW(hHelp, MF_STRING, ID_MENU_ABOUT, tr.action_about.c_str());

    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFile, tr.menu_file.c_str());
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hSettings, tr.menu_settings.c_str());
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hHelp, tr.menu_help.c_str());

    SetMenu(g_hWnd, hMenu);
    if (g_hMenu) DestroyMenu(g_hMenu);
    g_hMenu = hMenu;
}

void OnAddFiles() {
    std::vector<wchar_t> buffer(65536, 0);
    OPENFILENAMEW ofn = { sizeof(OPENFILENAMEW) };
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFilter = L"Media Files (*.jpg;*.jpeg;*.png;*.webp;*.bmp;*.tiff;*.mp4;*.mov;*.mkv;*.avi)\0*.jpg;*.jpeg;*.png;*.webp;*.bmp;*.tiff;*.mp4;*.mov;*.mkv;*.avi\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = buffer.data();
    ofn.nMaxFile = (DWORD)buffer.size();
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;

    if (GetOpenFileNameW(&ofn)) {
        wchar_t* ptr = ofn.lpstrFile;
        std::wstring dir = ptr;
        ptr += dir.length() + 1;

        if (*ptr == L'\0') {
            AddFileToTable(fs::path(dir));
        }
        else {
            while (*ptr != L'\0') {
                std::wstring filename = ptr;
                AddFileToTable(fs::path(dir) / filename);
                ptr += filename.length() + 1;
            }
        }
    }
}

void OnAddFolder() {
    BROWSEINFOW bi = { 0 };
    bi.hwndOwner = g_hWnd;
    bi.lpszTitle = L"Select Folder with Media Assets";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    PIDLIST_ABSOLUTE pidl = SHBrowseForFolderW(&bi);
    if (pidl) {
        wchar_t folderPath[MAX_PATH];
        if (SHGetPathFromIDListW(pidl, folderPath)) {
            for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
                if (entry.is_regular_file()) {
                    AddFileToTable(entry.path());
                }
            }
        }
        CoTaskMemFree(pidl);
    }
}

void OnClearList() {
    ListView_DeleteAllItems(g_hList);
    g_registeredPaths.clear();
    SendMessageW(g_hProgressBar, PBM_SETPOS, 0, 0);
}

void OnStartTask() {
    const Translation& tr = g_translations[g_currentLang];
    if (g_registeredPaths.empty()) {
        AppendLog(tr.log_empty_list);
        return;
    }

    ProcessOptions opts;
    opts.stripMeta = (SendMessageW(g_hChkStripMeta, BM_GETCHECK, 0, 0) == BST_CHECKED);
    opts.randomizeHash = (SendMessageW(g_hChkRandHash, BM_GETCHECK, 0, 0) == BST_CHECKED);
    opts.randomNames = (SendMessageW(g_hChkRandNames, BM_GETCHECK, 0, 0) == BST_CHECKED);
    opts.openFolder = (SendMessageW(g_hChkOpenFolder, BM_GETCHECK, 0, 0) == BST_CHECKED);

    EnableWindow(g_hBtnStart, FALSE);
    EnableWindow(g_hBtnStop, TRUE);
    EnableWindow(g_hBtnAddFiles, FALSE);
    EnableWindow(g_hBtnAddFolder, FALSE);
    EnableWindow(g_hBtnClear, FALSE);
    SendMessageW(g_hProgressBar, PBM_SETPOS, 0, 0);

    g_isInterrupted.store(false);

    if (g_workerThread.joinable()) {
        g_workerThread.join();
    }
    g_workerThread = std::thread(WorkerThreadFunc, g_registeredPaths, opts, g_currentLang);
}

void OnStopTask() {
    g_isInterrupted.store(true);
    EnableWindow(g_hBtnStop, FALSE);
}

INT_PTR CALLBACK PrefsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM) {
    switch (message) {
    case WM_INITDIALOG: {
        const Translation& tr = g_translations[g_currentLang];
        SetWindowTextW(hDlg, tr.settings_title.c_str());

        CreateWindowExW(0, L"STATIC", tr.settings_language.c_str(), WS_CHILD | WS_VISIBLE, 15, 18, 120, 20, hDlg, NULL, NULL, NULL);
        HWND hCombo = CreateWindowExW(0, L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_TABSTOP, 140, 15, 120, 100, hDlg, (HMENU)102, NULL, NULL);
        SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"English");
        SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Русский");
        SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Eesti");

        if (g_currentLang == L"en") SendMessageW(hCombo, CB_SETCURSEL, 0, 0);
        else if (g_currentLang == L"ru") SendMessageW(hCombo, CB_SETCURSEL, 1, 0);
        else if (g_currentLang == L"et") SendMessageW(hCombo, CB_SETCURSEL, 2, 0);

        CreateWindowExW(0, L"BUTTON", tr.settings_save.c_str(), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP, 100, 60, 80, 26, hDlg, (HMENU)IDOK, NULL, NULL);
        CreateWindowExW(0, L"BUTTON", tr.settings_cancel.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 190, 60, 80, 26, hDlg, (HMENU)IDCANCEL, NULL, NULL);

        EnumChildWindows(hDlg, [](HWND hChild, LPARAM) -> BOOL {
            SendMessageW(hChild, WM_SETFONT, (WPARAM)g_hFont, TRUE);
            return TRUE;
            }, 0);
        return TRUE;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            HWND hCombo = GetDlgItem(hDlg, 102);
            int sel = (int)SendMessageW(hCombo, CB_GETCURSEL, 0, 0);
            if (sel == 0) g_currentLang = L"en";
            else if (sel == 1) g_currentLang = L"ru";
            else if (sel == 2) g_currentLang = L"et";

            RetranslateUI();
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void ShowPreferences() {
#pragma pack(push, 4)
    struct DLGTEMPLATEEX {
        WORD dlgVer;
        WORD signature;
        DWORD helpID;
        DWORD exStyle;
        DWORD style;
        WORD cDlgItems;
        short x, y, cx, cy;
    } dt = { 1, 0xFFFF, 0, 0, WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_CENTER, 4, 0, 0, 190, 85 };
#pragma pack(pop)

    std::vector<BYTE> dialogBuf(1024, 0);
    memcpy(dialogBuf.data(), &dt, sizeof(dt));

    DialogBoxIndirectParamW(GetModuleHandle(NULL), (LPCDLGTEMPLATEW)dialogBuf.data(), g_hWnd, PrefsDlgProc, 0);
}

void RelayoutControls(int width, int height) {
    int margin = 14;
    int spacing = 10;

    int bottomH = 70;
    int clientW = width - margin * 2;
    int clientH = height - margin * 2 - bottomH;

    int leftW = (int)(clientW * 0.58);
    int rightW = clientW - leftW - spacing;

    SetWindowPos(g_hLblDropHint, NULL, margin, margin, leftW, 20, SWP_NOZORDER);

    int btnRowH = 30;
    int listH = clientH - 25 - btnRowH - spacing;
    SetWindowPos(g_hList, NULL, margin, margin + 25, leftW, listH, SWP_NOZORDER);

    int btnW = (leftW - spacing * 2) / 3;
    SetWindowPos(g_hBtnAddFiles, NULL, margin, margin + 25 + listH + spacing, btnW, btnRowH, SWP_NOZORDER);
    SetWindowPos(g_hBtnAddFolder, NULL, margin + btnW + spacing, margin + 25 + listH + spacing, btnW, btnRowH, SWP_NOZORDER);
    SetWindowPos(g_hBtnClear, NULL, margin + (btnW + spacing) * 2, margin + 25 + listH + spacing, leftW - (btnW + spacing) * 2, btnRowH, SWP_NOZORDER);

    int rightX = margin + leftW + spacing;
    int grpOptionsH = 135;
    SetWindowPos(g_hGrpOptions, NULL, rightX, margin, rightW, grpOptionsH, SWP_NOZORDER);

    int chkY = margin + 22;
    int chkH = 22;
    SetWindowPos(g_hChkStripMeta, NULL, rightX + 12, chkY, rightW - 24, chkH, SWP_NOZORDER);
    SetWindowPos(g_hChkRandHash, NULL, rightX + 12, chkY + chkH + 4, rightW - 24, chkH, SWP_NOZORDER);
    SetWindowPos(g_hChkRandNames, NULL, rightX + 12, chkY + (chkH + 4) * 2, rightW - 24, chkH, SWP_NOZORDER);
    SetWindowPos(g_hChkOpenFolder, NULL, rightX + 12, chkY + (chkH + 4) * 3, rightW - 24, chkH, SWP_NOZORDER);

    int logLblY = margin + grpOptionsH + spacing;
    SetWindowPos(g_hLblLog, NULL, rightX, logLblY, rightW, 20, SWP_NOZORDER);

    int logTextY = logLblY + 22;
    int logTextH = clientH - (logTextY - margin);
    SetWindowPos(g_hTxtLog, NULL, rightX, logTextY, rightW, logTextH, SWP_NOZORDER);

    int bottomY = margin + clientH + spacing;
    SetWindowPos(g_hProgressBar, NULL, margin, bottomY, clientW, 20, SWP_NOZORDER);

    int startBtnW = (int)(clientW * 0.78);
    int stopBtnW = clientW - startBtnW - spacing;
    SetWindowPos(g_hBtnStart, NULL, margin, bottomY + 26, startBtnW, 34, SWP_NOZORDER);
    SetWindowPos(g_hBtnStop, NULL, margin + startBtnW + spacing, bottomY + 26, stopBtnW, 34, SWP_NOZORDER);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        NONCLIENTMETRICSW ncm = { sizeof(NONCLIENTMETRICSW) };
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), &ncm, 0);
        g_hFont = CreateFontIndirectW(&ncm.lfMessageFont);
        g_hLogFont = CreateFontW(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, L"Consolas");

        g_hLblDropHint = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)IDC_LABEL_DROP_HINT, NULL, NULL);

        g_hList = CreateWindowExW(0, WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
            0, 0, 0, 0, hWnd, (HMENU)IDC_FILE_LIST, NULL, NULL);
        ListView_SetExtendedListViewStyle(g_hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

        LVCOLUMNW lvc = { 0 };
        lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        lvc.fmt = LVCFMT_LEFT;

        lvc.cx = 240; lvc.pszText = (LPWSTR)L"Name"; ListView_InsertColumn(g_hList, 0, &lvc);
        lvc.cx = 85;  lvc.pszText = (LPWSTR)L"Size"; ListView_InsertColumn(g_hList, 1, &lvc);
        lvc.cx = 75;  lvc.pszText = (LPWSTR)L"Type"; ListView_InsertColumn(g_hList, 2, &lvc);
        lvc.cx = 105; lvc.pszText = (LPWSTR)L"Status"; ListView_InsertColumn(g_hList, 3, &lvc);

        g_hBtnAddFiles = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, hWnd, (HMENU)IDC_BTN_ADD_FILES, NULL, NULL);
        g_hBtnAddFolder = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, hWnd, (HMENU)IDC_BTN_ADD_FOLDER, NULL, NULL);
        g_hBtnClear = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 0, 0, hWnd, (HMENU)IDC_BTN_CLEAR, NULL, NULL);

        g_hGrpOptions = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 0, 0, 0, 0, hWnd, (HMENU)IDC_GRP_OPTIONS, NULL, NULL);
        g_hChkStripMeta = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 0, 0, 0, 0, hWnd, (HMENU)IDC_CHK_STRIP_META, NULL, NULL);
        g_hChkRandHash = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 0, 0, 0, 0, hWnd, (HMENU)IDC_CHK_RAND_HASH, NULL, NULL);
        g_hChkRandNames = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 0, 0, 0, 0, hWnd, (HMENU)IDC_CHK_RAND_NAMES, NULL, NULL);
        g_hChkOpenFolder = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 0, 0, 0, 0, hWnd, (HMENU)IDC_CHK_OPEN_FOLDER, NULL, NULL);

        SendMessageW(g_hChkStripMeta, BM_SETCHECK, BST_CHECKED, 0);
        SendMessageW(g_hChkRandHash, BM_SETCHECK, BST_CHECKED, 0);
        SendMessageW(g_hChkRandNames, BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessageW(g_hChkOpenFolder, BM_SETCHECK, BST_CHECKED, 0);

        g_hLblLog = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)IDC_LABEL_LOG, NULL, NULL);
        g_hTxtLog = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            0, 0, 0, 0, hWnd, (HMENU)IDC_TXT_LOG, NULL, NULL);

        g_hProgressBar = CreateWindowExW(0, PROGRESS_CLASSW, L"", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)IDC_PROGRESS_BAR, NULL, NULL);
        SendMessageW(g_hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));

        g_hBtnStart = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 0, 0, 0, 0, hWnd, (HMENU)IDC_BTN_START, NULL, NULL);
        g_hBtnStop = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED, 0, 0, 0, 0, hWnd, (HMENU)IDC_BTN_STOP, NULL, NULL);

        EnumChildWindows(hWnd, [](HWND hChild, LPARAM) -> BOOL {
            if (hChild == g_hTxtLog) {
                SendMessageW(hChild, WM_SETFONT, (WPARAM)g_hLogFont, TRUE);
            }
            else {
                SendMessageW(hChild, WM_SETFONT, (WPARAM)g_hFont, TRUE);
            }
            return TRUE;
            }, 0);

        DragAcceptFiles(hWnd, TRUE);
        RetranslateUI();
        break;
    }

    case WM_SIZE: {
        RelayoutControls(LOWORD(lParam), HIWORD(lParam));
        break;
    }

    case WM_GETMINMAXINFO: {
        MINMAXINFO* mmi = (MINMAXINFO*)lParam;
        mmi->ptMinTrackSize.x = 800;
        mmi->ptMinTrackSize.y = 560;
        break;
    }

    case WM_DROPFILES: {
        HDROP hDrop = (HDROP)wParam;
        UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
        wchar_t filePath[MAX_PATH];
        for (UINT i = 0; i < count; ++i) {
            if (DragQueryFileW(hDrop, i, filePath, MAX_PATH)) {
                fs::path p(filePath);
                if (fs::is_directory(p)) {
                    for (const auto& entry : fs::recursive_directory_iterator(p)) {
                        if (entry.is_regular_file()) {
                            AddFileToTable(entry.path());
                        }
                    }
                }
                else if (fs::is_regular_file(p)) {
                    AddFileToTable(p);
                }
            }
        }
        DragFinish(hDrop);
        break;
    }

    case WM_COMMAND: {
        int id = LOWORD(wParam);
        switch (id) {
        case ID_MENU_ADD_FILES:
        case IDC_BTN_ADD_FILES:
            OnAddFiles();
            break;
        case ID_MENU_ADD_FOLDER:
        case IDC_BTN_ADD_FOLDER:
            OnAddFolder();
            break;
        case ID_MENU_CLEAR:
        case IDC_BTN_CLEAR:
            OnClearList();
            break;
        case ID_MENU_EXIT:
            PostMessageW(hWnd, WM_CLOSE, 0, 0);
            break;
        case ID_MENU_PREFS:
            ShowPreferences();
            break;
        case ID_MENU_ABOUT: {
            const Translation& tr = g_translations[g_currentLang];
            MessageBoxW(hWnd, tr.about_text.c_str(), tr.about_title.c_str(), MB_OK | MB_ICONINFORMATION);
            break;
        }
        case IDC_BTN_START:
            OnStartTask();
            break;
        case IDC_BTN_STOP:
            OnStopTask();
            break;
        }
        break;
    }

    case WM_APP_LOG: {
        std::wstring* pText = reinterpret_cast<std::wstring*>(lParam);
        if (pText) {
            AppendLog(*pText);
            delete pText;
        }
        break;
    }

    case WM_APP_ROW_STATUS: {
        int row = (int)wParam;
        std::wstring* pStatus = reinterpret_cast<std::wstring*>(lParam);
        if (pStatus) {
            ListView_SetItemText(g_hList, row, 3, const_cast<LPWSTR>(pStatus->c_str()));
            delete pStatus;
        }
        break;
    }

    case WM_APP_PROGRESS: {
        int current = (int)wParam;
        int total = (int)lParam;
        if (total > 0) {
            int pct = (int)(((double)current / total) * 100.0);
            SendMessageW(g_hProgressBar, PBM_SETPOS, pct, 0);
        }
        break;
    }

    case WM_APP_TASK_FINISHED: {
        bool openFolder = (wParam == 1);
        std::wstring* pOutDir = reinterpret_cast<std::wstring*>(lParam);

        EnableWindow(g_hBtnStart, TRUE);
        EnableWindow(g_hBtnStop, FALSE);
        EnableWindow(g_hBtnAddFiles, TRUE);
        EnableWindow(g_hBtnAddFolder, TRUE);
        EnableWindow(g_hBtnClear, TRUE);

        if (openFolder && pOutDir && fs::exists(*pOutDir)) {
            ShellExecuteW(NULL, L"open", pOutDir->c_str(), NULL, NULL, SW_SHOWNORMAL);
        }
        if (pOutDir) delete pOutDir;
        break;
    }

    case WM_DESTROY: {
        g_isInterrupted.store(true);
        if (g_workerThread.joinable()) {
            g_workerThread.join();
        }
        if (g_hFont) DeleteObject(g_hFont);
        if (g_hLogFont) DeleteObject(g_hLogFont);
        PostQuitMessage(0);
        break;
    }

    default:
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES | ICC_PROGRESS_CLASS | ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"AFAC_MainWindow_Class";

    RegisterClassExW(&wc);

    g_hWnd = CreateWindowExW(
        WS_EX_ACCEPTFILES,
        wc.lpszClassName,
        L"AFAC - Anti-Fingerprint Asset Cleaner",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 940, 700,
        NULL, NULL, hInstance, NULL
    );

    if (!g_hWnd) {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return 0;
    }

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}