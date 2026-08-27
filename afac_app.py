import os
import sys
import random
import shutil
import subprocess
import platform
from pathlib import Path
from datetime import datetime
from PIL import Image, ImageEnhance

from PyQt6.QtCore import Qt, QThread, pyqtSignal, QSize
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QPushButton, QLabel, QProgressBar, QTextEdit, QCheckBox,
    QFileDialog, QGroupBox, QSplitter, QTableWidget, QTableWidgetItem,
    QHeaderView, QDialog, QComboBox, QFormLayout, QMenuBar, QMenu,
    QMessageBox, QAbstractItemView
)
from PyQt6.QtGui import QFont, QAction

TRANSLATIONS = {
    "en": {
        "app_title": "AFAC - Anti-Fingerprint Asset Cleaner",
        "menu_file": "File",
        "menu_settings": "Settings",
        "menu_help": "Help",
        "action_add_files": "Add Files...",
        "action_add_folder": "Add Folder...",
        "action_clear": "Clear File List",
        "action_exit": "Exit",
        "action_preferences": "Preferences...",
        "action_about": "About AFAC",
        "drop_zone_hint": "Drag and drop media files or folders into the table below:",
        "col_name": "File Name",
        "col_size": "Size",
        "col_type": "Type",
        "col_status": "Status",
        "status_ready": "Pending",
        "status_processing": "Processing",
        "status_done": "Completed",
        "status_error": "Error",
        "btn_add_files": "Add Files",
        "btn_add_folder": "Add Folder",
        "btn_clear": "Clear List",
        "group_options": "Processing Options",
        "opt_strip_meta": "Strip metadata (EXIF, IPTC, XMP, stream tags)",
        "opt_rand_hash": "Randomize hash (MD5 / SHA-256) via micro-shift",
        "opt_rand_names": "Generate random output filenames",
        "opt_open_folder": "Open output directory upon completion",
        "log_title": "Execution Log:",
        "btn_start": "START PROCESSING",
        "btn_stop": "STOP",
        "settings_title": "Preferences",
        "settings_language": "Interface Language:",
        "settings_theme": "Visual Theme:",
        "settings_save": "Save",
        "settings_cancel": "Cancel",
        "log_start": "Process initiated. Total files to process: {count}",
        "log_ffmpeg_missing": "FFmpeg was not detected in PATH or application directory. Video processing will fail.",
        "log_ffmpeg_found": "FFmpeg detected at: {path}",
        "log_processed_img": "Image processed successfully: {src} -> {dst}",
        "log_processed_vid": "Video processed successfully: {src} -> {dst}",
        "log_skipped_unsupported": "File skipped due to unsupported extension: {file}",
        "log_skipped_no_ffmpeg": "Video skipped because FFmpeg is missing: {file}",
        "log_error": "Error processing {file}: {error}",
        "log_canceled": "Process stopped by user.",
        "log_finished": "Task completed. Successfully processed: {success}/{total}. Output path: {path}",
        "log_empty_list": "The file list is empty. Add files before starting.",
        "about_title": "About AFAC",
        "about_text": "AFAC (Anti-Fingerprint Asset Cleaner)\n\nUtility for batch metadata removal and unique content generation for image and video assets.\n\nVersion: 2.1.0",
        "theme_dark": "Dark Standard",
        "theme_deep": "Deep Dark (OLED)",
        "theme_light": "Light Standard"
    },
    "ru": {
        "app_title": "AFAC - Anti-Fingerprint Asset Cleaner",
        "menu_file": "Файл",
        "menu_settings": "Настройки",
        "menu_help": "Справка",
        "action_add_files": "Добавить файлы...",
        "action_add_folder": "Добавить папку...",
        "action_clear": "Очистить список файлов",
        "action_exit": "Выход",
        "action_preferences": "Параметры...",
        "action_about": "О программе",
        "drop_zone_hint": "Перетащите файлы или папки в таблицу ниже:",
        "col_name": "Имя файла",
        "col_size": "Размер",
        "col_type": "Тип",
        "col_status": "Статус",
        "status_ready": "В очереди",
        "status_processing": "Обработка",
        "status_done": "Завершено",
        "status_error": "Ошибка",
        "btn_add_files": "Добавить файлы",
        "btn_add_folder": "Добавить папку",
        "btn_clear": "Очистить список",
        "group_options": "Параметры обработки",
        "opt_strip_meta": "Удалить метаданные (EXIF, IPTC, XMP, теги потоков)",
        "opt_rand_hash": "Рандомизировать хэш (MD5 / SHA-256) микро-сдвигом",
        "opt_rand_names": "Генерировать случайные имена файлов",
        "opt_open_folder": "Открыть папку с результатом после завершения",
        "log_title": "Журнал выполнения:",
        "btn_start": "НАЧАТЬ ОБРАБОТКУ",
        "btn_stop": "ОСТАНОВИТЬ",
        "settings_title": "Параметры",
        "settings_language": "Язык интерфейса:",
        "settings_theme": "Тема оформления:",
        "settings_save": "Сохранить",
        "settings_cancel": "Отмена",
        "log_start": "Процесс запущен. Всего файлов для обработки: {count}",
        "log_ffmpeg_missing": "FFmpeg не обнаружен в PATH или каталоге программы. Обработка видео невозможна.",
        "log_ffmpeg_found": "FFmpeg обнаружен по пути: {path}",
        "log_processed_img": "Изображение обработано: {src} -> {dst}",
        "log_processed_vid": "Видео обработано: {src} -> {dst}",
        "log_skipped_unsupported": "Файл пропущен (неподдерживаемый формат): {file}",
        "log_skipped_no_ffmpeg": "Видео пропущено из-за отсутствия FFmpeg: {file}",
        "log_error": "Ошибка обработки {file}: {error}",
        "log_canceled": "Процесс остановлен пользователем.",
        "log_finished": "Задача завершена. Успешно обработано: {success}/{total}. Каталог: {path}",
        "log_empty_list": "Список файлов пуст. Добавьте файлы перед запуском.",
        "about_title": "О программе",
        "about_text": "AFAC (Anti-Fingerprint Asset Cleaner)\n\nУтилита для пакетной очистки метаданных и уникализации медиафайлов (изображений и видео).\n\nВерсия: 2.1.0",
        "theme_dark": "Темная стандартная",
        "theme_deep": "Глубокая темная (OLED)",
        "theme_light": "Светлая стандартная"
    },
    "et": {
        "app_title": "AFAC - Anti-Fingerprint Asset Cleaner",
        "menu_file": "Fail",
        "menu_settings": "Seaded",
        "menu_help": "Abi",
        "action_add_files": "Lisa failid...",
        "action_add_folder": "Lisa kaust...",
        "action_clear": "Tühjenda loend",
        "action_exit": "Välju",
        "action_preferences": "Eelistused...",
        "action_about": "Programmist",
        "drop_zone_hint": "Lohistage failid või kaustad allolevasse tabelisse:",
        "col_name": "Faili nimi",
        "col_size": "Suurus",
        "col_type": "Tüüp",
        "col_status": "Olek",
        "status_ready": "Ootel",
        "status_processing": "Töötlemine",
        "status_done": "Lõpetatud",
        "status_error": "Viga",
        "btn_add_files": "Lisa failid",
        "btn_add_folder": "Lisa kaust",
        "btn_clear": "Tühjenda",
        "group_options": "Töötlemise valikud",
        "opt_strip_meta": "Eemalda metaandmed (EXIF, IPTC, XMP, vootähised)",
        "opt_rand_hash": "Muuda faili räsi (MD5 / SHA-256) mikronihkega",
        "opt_rand_names": "Loo juhuslikud failinimed",
        "opt_open_folder": "Ava väljundkaust pärast lõpetamist",
        "log_title": "Töölogi:",
        "btn_start": "ALUSTA TÖÖTLEMIST",
        "btn_stop": "PEATAGE",
        "settings_title": "Eelistused",
        "settings_language": "Kasutajaliidese keel:",
        "settings_theme": "Teema:",
        "settings_save": "Salvesta",
        "settings_cancel": "Loobu",
        "log_start": "Protsess käivitatud. Failide koguarv: {count}",
        "log_ffmpeg_missing": "FFmpeg programm ei leitud PATH-ist ega rakenduse kaustast. Video töötlemine ebaõnnestub.",
        "log_ffmpeg_found": "FFmpeg leitud asukohast: {path}",
        "log_processed_img": "Pilt edukalt töödeldud: {src} -> {dst}",
        "log_processed_vid": "Video edukalt töödeldud: {src} -> {dst}",
        "log_skipped_unsupported": "Fail vahele jäetud (toetamata formaat): {file}",
        "log_skipped_no_ffmpeg": "Video vahele jäetud FFmpegi puudumise tõttu: {file}",
        "log_error": "Viga faili {file} töötlemisel: {error}",
        "log_canceled": "Kasutaja peatas protsessi.",
        "log_finished": "Töö lõpetatud. Edukalt töödeldud: {success}/{total}. Väljundkaust: {path}",
        "log_empty_list": "Failide loend on tühi. Lisage failid enne alustamist.",
        "about_title": "Programmist",
        "about_text": "AFAC (Anti-Fingerprint Asset Cleaner)\n\nUtiliit pildi- ja videofailide metaandmete puhastamiseks ja unikaalsuse tagamiseks.\n\nVersioon: 2.1.0",
        "theme_dark": "Tume standardne",
        "theme_deep": "Sügav tume (OLED)",
        "theme_light": "Hele standardne"
    }
}

THEMES = {
    "dark": """
        QWidget {
            background-color: #2b2d30;
            color: #dfe1e5;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Arial, sans-serif;
            font-size: 13px;
        }
        QMenuBar {
            background-color: #1e1f22;
            color: #dfe1e5;
            border-bottom: 1px solid #393b40;
        }
        QMenuBar::item:selected {
            background-color: #3574f0;
            color: #ffffff;
        }
        QMenu {
            background-color: #2b2d30;
            color: #dfe1e5;
            border: 1px solid #393b40;
        }
        QMenu::item:selected {
            background-color: #3574f0;
            color: #ffffff;
        }
        QGroupBox {
            border: 1px solid #393b40;
            border-radius: 4px;
            margin-top: 10px;
            padding-top: 10px;
            font-weight: 600;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 4px;
        }
        QTableWidget {
            background-color: #1e1f22;
            border: 1px solid #393b40;
            border-radius: 4px;
            gridline-color: #2b2d30;
            color: #dfe1e5;
        }
        QHeaderView::section {
            background-color: #2b2d30;
            color: #8c9099;
            border: 1px solid #393b40;
            padding: 4px;
            font-weight: 600;
        }
        QTextEdit {
            background-color: #1e1f22;
            border: 1px solid #393b40;
            border-radius: 4px;
            font-family: "Consolas", "Courier New", monospace;
            font-size: 12px;
            color: #bcbec4;
        }
        QPushButton {
            background-color: #393b40;
            color: #dfe1e5;
            border: 1px solid #4e5157;
            border-radius: 4px;
            padding: 6px 14px;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: #43454a;
        }
        QPushButton:pressed {
            background-color: #2b2d30;
        }
        QPushButton#start_btn {
            background-color: #3574f0;
            color: #ffffff;
            border: 1px solid #3574f0;
            font-weight: 600;
            padding: 9px;
        }
        QPushButton#start_btn:hover {
            background-color: #3068d9;
        }
        QPushButton#stop_btn {
            background-color: #e5484d;
            color: #ffffff;
            border: 1px solid #e5484d;
            font-weight: 600;
            padding: 9px;
        }
        QPushButton#stop_btn:hover {
            background-color: #c93b40;
        }
        QProgressBar {
            border: 1px solid #393b40;
            border-radius: 4px;
            background-color: #1e1f22;
            text-align: center;
            color: #dfe1e5;
            height: 20px;
        }
        QProgressBar::chunk {
            background-color: #3574f0;
        }
        QCheckBox {
            spacing: 6px;
        }
        QComboBox {
            background-color: #1e1f22;
            border: 1px solid #393b40;
            border-radius: 4px;
            padding: 4px 8px;
            color: #dfe1e5;
        }
    """,
    "deep": """
        QWidget {
            background-color: #121212;
            color: #e0e0e0;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Arial, sans-serif;
            font-size: 13px;
        }
        QMenuBar {
            background-color: #080808;
            color: #e0e0e0;
            border-bottom: 1px solid #242424;
        }
        QMenuBar::item:selected {
            background-color: #262626;
            color: #ffffff;
        }
        QMenu {
            background-color: #121212;
            color: #e0e0e0;
            border: 1px solid #242424;
        }
        QMenu::item:selected {
            background-color: #262626;
            color: #ffffff;
        }
        QGroupBox {
            border: 1px solid #242424;
            border-radius: 4px;
            margin-top: 10px;
            padding-top: 10px;
            font-weight: 600;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 4px;
        }
        QTableWidget {
            background-color: #000000;
            border: 1px solid #242424;
            border-radius: 4px;
            gridline-color: #1a1a1a;
            color: #e0e0e0;
        }
        QHeaderView::section {
            background-color: #181818;
            color: #888888;
            border: 1px solid #242424;
            padding: 4px;
            font-weight: 600;
        }
        QTextEdit {
            background-color: #000000;
            border: 1px solid #242424;
            border-radius: 4px;
            font-family: "Consolas", "Courier New", monospace;
            font-size: 12px;
            color: #aaaaaa;
        }
        QPushButton {
            background-color: #1e1e1e;
            color: #e0e0e0;
            border: 1px solid #333333;
            border-radius: 4px;
            padding: 6px 14px;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: #2a2a2a;
        }
        QPushButton#start_btn {
            background-color: #2e7d32;
            color: #ffffff;
            border: 1px solid #2e7d32;
            font-weight: 600;
            padding: 9px;
        }
        QPushButton#start_btn:hover {
            background-color: #388e3c;
        }
        QPushButton#stop_btn {
            background-color: #c62828;
            color: #ffffff;
            border: 1px solid #c62828;
            font-weight: 600;
            padding: 9px;
        }
        QPushButton#stop_btn:hover {
            background-color: #d32f2f;
        }
        QProgressBar {
            border: 1px solid #242424;
            border-radius: 4px;
            background-color: #000000;
            text-align: center;
            color: #e0e0e0;
            height: 20px;
        }
        QProgressBar::chunk {
            background-color: #2e7d32;
        }
        QCheckBox {
            spacing: 6px;
        }
        QComboBox {
            background-color: #1a1a1a;
            border: 1px solid #333333;
            border-radius: 4px;
            padding: 4px 8px;
            color: #e0e0e0;
        }
    """,
    "light": """
        QWidget {
            background-color: #f5f5f7;
            color: #1d1d1f;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Arial, sans-serif;
            font-size: 13px;
        }
        QMenuBar {
            background-color: #e8e8ed;
            color: #1d1d1f;
            border-bottom: 1px solid #d2d2d7;
        }
        QMenuBar::item:selected {
            background-color: #0071e3;
            color: #ffffff;
        }
        QMenu {
            background-color: #ffffff;
            color: #1d1d1f;
            border: 1px solid #d2d2d7;
        }
        QMenu::item:selected {
            background-color: #0071e3;
            color: #ffffff;
        }
        QGroupBox {
            border: 1px solid #d2d2d7;
            border-radius: 4px;
            margin-top: 10px;
            padding-top: 10px;
            font-weight: 600;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 4px;
        }
        QTableWidget {
            background-color: #ffffff;
            border: 1px solid #d2d2d7;
            border-radius: 4px;
            gridline-color: #f0f0f2;
            color: #1d1d1f;
        }
        QHeaderView::section {
            background-color: #ebebee;
            color: #515154;
            border: 1px solid #d2d2d7;
            padding: 4px;
            font-weight: 600;
        }
        QTextEdit {
            background-color: #ffffff;
            border: 1px solid #d2d2d7;
            border-radius: 4px;
            font-family: "Consolas", "Courier New", monospace;
            font-size: 12px;
            color: #1d1d1f;
        }
        QPushButton {
            background-color: #ffffff;
            color: #1d1d1f;
            border: 1px solid #d2d2d7;
            border-radius: 4px;
            padding: 6px 14px;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: #f0f0f2;
        }
        QPushButton#start_btn {
            background-color: #0071e3;
            color: #ffffff;
            border: 1px solid #0071e3;
            font-weight: 600;
            padding: 9px;
        }
        QPushButton#start_btn:hover {
            background-color: #0077ed;
        }
        QPushButton#stop_btn {
            background-color: #df2020;
            color: #ffffff;
            border: 1px solid #df2020;
            font-weight: 600;
            padding: 9px;
        }
        QPushButton#stop_btn:hover {
            background-color: #e53935;
        }
        QProgressBar {
            border: 1px solid #d2d2d7;
            border-radius: 4px;
            background-color: #ffffff;
            text-align: center;
            color: #1d1d1f;
            height: 20px;
        }
        QProgressBar::chunk {
            background-color: #0071e3;
        }
        QCheckBox {
            spacing: 6px;
        }
        QComboBox {
            background-color: #ffffff;
            border: 1px solid #d2d2d7;
            border-radius: 4px;
            padding: 4px 8px;
            color: #1d1d1f;
        }
    """
}

IMAGE_EXTENSIONS = {".jpg", ".jpeg", ".png", ".webp", ".bmp", ".tiff"}
VIDEO_EXTENSIONS = {".mp4", ".mov", ".mkv", ".avi"}

def resolve_ffmpeg_binary() -> str | None:
    binary_name = "ffmpeg.exe" if platform.system() == "Windows" else "ffmpeg"
    local_path = Path(__file__).parent / binary_name
    if local_path.is_file():
        return str(local_path)
    return shutil.which("ffmpeg")

def format_file_size(size_in_bytes: int) -> str:
    if size_in_bytes < 1024:
        return f"{size_in_bytes} B"
    elif size_in_bytes < 1024 * 1024:
        return f"{size_in_bytes / 1024:.1f} KB"
    else:
        return f"{size_in_bytes / (1024 * 1024):.1f} MB"

class FileTableWidget(QTableWidget):
    files_dropped = pyqtSignal(list)

    def __init__(self, parent=None):
        super().__init__(0, 4, parent)
        self.setAcceptDrops(True)
        self.setSelectionBehavior(QAbstractItemView.SelectionBehavior.SelectRows)
        self.setSelectionMode(QAbstractItemView.SelectionMode.ExtendedSelection)
        self.horizontalHeader().setSectionResizeMode(0, QHeaderView.ResizeMode.Stretch)
        self.horizontalHeader().setSectionResizeMode(1, QHeaderView.ResizeMode.ResizeToContents)
        self.horizontalHeader().setSectionResizeMode(2, QHeaderView.ResizeMode.ResizeToContents)
        self.horizontalHeader().setSectionResizeMode(3, QHeaderView.ResizeMode.ResizeToContents)
        self.verticalHeader().setVisible(False)

    def dragEnterEvent(self, event):
        if event.mimeData().hasUrls():
            event.acceptProposedAction()
        else:
            event.ignore()

    def dragMoveEvent(self, event):
        if event.mimeData().hasUrls():
            event.acceptProposedAction()
        else:
            event.ignore()

    def dropEvent(self, event):
        paths = []
        for url in event.mimeData().urls():
            local_path = url.toLocalFile()
            if os.path.exists(local_path):
                paths.append(local_path)
        if paths:
            self.files_dropped.emit(paths)
        event.acceptProposedAction()

class AssetProcessorThread(QThread):
    progress_updated = pyqtSignal(int, int)
    row_status_updated = pyqtSignal(int, str)
    log_emitted = pyqtSignal(str)
    processing_completed = pyqtSignal(str)

    def __init__(self, file_items: list[tuple[int, str]], options: dict, lang: str, parent=None):
        super().__init__(parent)
        self.file_items = file_items
        self.options = options
        self.lang = lang
        self._is_interrupted = False

    def request_interruption(self):
        self._is_interrupted = True

    def run(self):
        tr = TRANSLATIONS.get(self.lang, TRANSLATIONS["en"])
        total_items = len(self.file_items)
        successful_count = 0

        output_path = Path.cwd() / "output"
        output_path.mkdir(exist_ok=True)

        ffmpeg_bin = resolve_ffmpeg_binary()

        self.log_emitted.emit(tr["log_start"].format(count=total_items))
        if ffmpeg_bin:
            self.log_emitted.emit(tr["log_ffmpeg_found"].format(path=ffmpeg_bin))
        else:
            self.log_emitted.emit(tr["log_ffmpeg_missing"])

        for idx, (row_index, file_path_str) in enumerate(self.file_items, start=1):
            if self._is_interrupted:
                self.log_emitted.emit(tr["log_canceled"])
                break

            file_path = Path(file_path_str)
            extension = file_path.suffix.lower()

            self.row_status_updated.emit(row_index, tr["status_processing"])

            try:

                if self.options.get("random_names", False):
                    prefix = "VID" if extension in VIDEO_EXTENSIONS else "IMG"
                    rand_id = random.randint(10000, 99999)
                    destination_name = f"{prefix}_{rand_id}{extension}"
                else:
                    destination_name = f"{file_path.stem}_cleaned{extension}"

                destination_path = output_path / destination_name

                if extension in IMAGE_EXTENSIONS:
                    self._process_image(file_path, destination_path)
                    successful_count += 1
                    self.row_status_updated.emit(row_index, tr["status_done"])
                    self.log_emitted.emit(tr["log_processed_img"].format(src=file_path.name, dst=destination_name))

                elif extension in VIDEO_EXTENSIONS:
                    if not ffmpeg_bin:
                        self.row_status_updated.emit(row_index, tr["status_error"])
                        self.log_emitted.emit(tr["log_skipped_no_ffmpeg"].format(file=file_path.name))
                    else:
                        self._process_video(ffmpeg_bin, file_path, destination_path)
                        successful_count += 1
                        self.row_status_updated.emit(row_index, tr["status_done"])
                        self.log_emitted.emit(tr["log_processed_vid"].format(src=file_path.name, dst=destination_name))

                else:
                    self.row_status_updated.emit(row_index, tr["status_error"])
                    self.log_emitted.emit(tr["log_skipped_unsupported"].format(file=file_path.name))

            except Exception as exc:
                self.row_status_updated.emit(row_index, tr["status_error"])
                self.log_emitted.emit(tr["log_error"].format(file=file_path.name, error=str(exc)))

            self.progress_updated.emit(idx, total_items)

        self.log_emitted.emit(tr["log_finished"].format(
            success=successful_count,
            total=total_items,
            path=str(output_path)
        ))
        self.processing_completed.emit(str(output_path))

    def _process_image(self, source: Path, destination: Path):
        with Image.open(source) as img:

            if destination.suffix.lower() in [".jpg", ".jpeg"] and img.mode in ("RGBA", "P"):
                transformed_img = img.convert("RGB")
            else:
                transformed_img = img.copy()

            if self.options.get("randomize_hash", False):
                contrast_delta = 1.0 + random.uniform(-0.004, 0.004)
                transformed_img = ImageEnhance.Contrast(transformed_img).enhance(contrast_delta)

                brightness_delta = 1.0 + random.uniform(-0.004, 0.004)
                transformed_img = ImageEnhance.Brightness(transformed_img).enhance(brightness_delta)

                if transformed_img.width > 30 and transformed_img.height > 30:
                    crop_x = random.choice([0, 1])
                    crop_y = random.choice([0, 1])
                    if crop_x > 0 or crop_y > 0:
                        transformed_img = transformed_img.crop((
                            0, 0,
                            transformed_img.width - crop_x,
                            transformed_img.height - crop_y
                        ))

            save_parameters = {}
            if destination.suffix.lower() in [".jpg", ".jpeg"]:
                save_parameters["quality"] = random.randint(94, 97) if self.options.get("randomize_hash") else 95
                save_parameters["optimize"] = True

            transformed_img.save(destination, **save_parameters)

    def _process_video(self, ffmpeg_bin: str, source: Path, destination: Path):
        command = [ffmpeg_bin, "-y", "-i", str(source)]

        if self.options.get("strip_meta", True):
            command.extend(["-map_metadata", "-1", "-map_chapters", "-1"])

        if self.options.get("randomize_hash", False):
            command.extend([
                "-vf", "noise=alls=1:allf=t",
                "-af", "adelay=10|10",
                "-c:v", "libx264",
                "-crf", str(random.randint(18, 21)),
                "-preset", "fast",
                "-c:a", "aac",
                "-b:a", "192k"
            ])
        else:
            command.extend(["-c", "copy"])

        command.append(str(destination))

        startup_info = None
        if platform.system() == "Windows":
            startup_info = subprocess.STARTUPINFO()
            startup_info.dwFlags |= subprocess.STARTF_USESHOWWINDOW

        execution = subprocess.run(
            command,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            startupinfo=startup_info
        )

        if execution.returncode != 0:
            error_details = execution.stderr.decode("utf-8", errors="ignore")
            last_line = error_details.splitlines()[-1] if error_details else "Command execution failed"
            raise RuntimeError(f"FFmpeg returned code {execution.returncode}: {last_line}")

class PreferencesDialog(QDialog):
    def __init__(self, current_lang: str, current_theme: str, parent=None):
        super().__init__(parent)
        self.selected_lang = current_lang
        self.selected_theme = current_theme
        self.resize(360, 160)
        self._build_ui()

    def _build_ui(self):
        tr = TRANSLATIONS.get(self.selected_lang, TRANSLATIONS["en"])
        self.setWindowTitle(tr["settings_title"])

        layout = QVBoxLayout(self)
        form_layout = QFormLayout()

        self.lang_combo = QComboBox()
        self.lang_combo.addItem("English", "en")
        self.lang_combo.addItem("Русский", "ru")
        self.lang_combo.addItem("Eesti", "et")

        index_lang = self.lang_combo.findData(self.selected_lang)
        if index_lang >= 0:
            self.lang_combo.setCurrentIndex(index_lang)

        self.theme_combo = QComboBox()
        self.theme_combo.addItem(tr["theme_dark"], "dark")
        self.theme_combo.addItem(tr["theme_deep"], "deep")
        self.theme_combo.addItem(tr["theme_light"], "light")

        index_theme = self.theme_combo.findData(self.selected_theme)
        if index_theme >= 0:
            self.theme_combo.setCurrentIndex(index_theme)

        form_layout.addRow(tr["settings_language"], self.lang_combo)
        form_layout.addRow(tr["settings_theme"], self.theme_combo)
        layout.addLayout(form_layout)

        button_layout = QHBoxLayout()
        self.save_btn = QPushButton(tr["settings_save"])
        self.cancel_btn = QPushButton(tr["settings_cancel"])

        self.save_btn.clicked.connect(self._accept_changes)
        self.cancel_btn.clicked.connect(self.reject)

        button_layout.addStretch()
        button_layout.addWidget(self.save_btn)
        button_layout.addWidget(self.cancel_btn)
        layout.addLayout(button_layout)

    def _accept_changes(self):
        self.selected_lang = self.lang_combo.currentData()
        self.selected_theme = self.theme_combo.currentData()
        self.accept()

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.current_lang = "en"
        self.current_theme = "dark"
        self.processor_thread = None
        self.registered_paths = []

        self.resize(920, 680)
        self.setMinimumSize(780, 520)

        self._setup_ui()
        self.apply_theme(self.current_theme)
        self.retranslate_ui()

    def _setup_ui(self):

        self.menu_bar = QMenuBar(self)
        self.setMenuBar(self.menu_bar)

        self.menu_file = QMenu(self)
        self.menu_settings = QMenu(self)
        self.menu_help = QMenu(self)

        self.menu_bar.addMenu(self.menu_file)
        self.menu_bar.addMenu(self.menu_settings)
        self.menu_bar.addMenu(self.menu_help)

        self.act_add_files = QAction(self)
        self.act_add_files.triggered.connect(self._open_file_dialog)
        self.act_add_folder = QAction(self)
        self.act_add_folder.triggered.connect(self._open_folder_dialog)
        self.act_clear = QAction(self)
        self.act_clear.triggered.connect(self._clear_file_table)
        self.act_exit = QAction(self)
        self.act_exit.triggered.connect(self.close)

        self.menu_file.addAction(self.act_add_files)
        self.menu_file.addAction(self.act_add_folder)
        self.menu_file.addSeparator()
        self.menu_file.addAction(self.act_clear)
        self.menu_file.addSeparator()
        self.menu_file.addAction(self.act_exit)

        self.act_preferences = QAction(self)
        self.act_preferences.triggered.connect(self._show_preferences)
        self.menu_settings.addAction(self.act_preferences)

        self.act_about = QAction(self)
        self.act_about.triggered.connect(self._show_about)
        self.menu_help.addAction(self.act_about)

        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        main_layout.setContentsMargins(14, 14, 14, 14)
        main_layout.setSpacing(10)

        splitter = QSplitter(Qt.Orientation.Horizontal)

        left_container = QWidget()
        left_layout = QVBoxLayout(left_container)
        left_layout.setContentsMargins(0, 0, 0, 0)
        left_layout.setSpacing(8)

        self.drop_label = QLabel()
        left_layout.addWidget(self.drop_label)

        self.file_table = FileTableWidget()
        self.file_table.files_dropped.connect(self.add_paths_to_table)
        left_layout.addWidget(self.file_table)

        btn_row_layout = QHBoxLayout()
        self.btn_add_files = QPushButton()
        self.btn_add_files.clicked.connect(self._open_file_dialog)
        self.btn_add_folder = QPushButton()
        self.btn_add_folder.clicked.connect(self._open_folder_dialog)
        self.btn_clear = QPushButton()
        self.btn_clear.clicked.connect(self._clear_file_table)

        btn_row_layout.addWidget(self.btn_add_files)
        btn_row_layout.addWidget(self.btn_add_folder)
        btn_row_layout.addWidget(self.btn_clear)
        left_layout.addLayout(btn_row_layout)

        splitter.addWidget(left_container)

        right_container = QWidget()
        right_layout = QVBoxLayout(right_container)
        right_layout.setContentsMargins(0, 0, 0, 0)
        right_layout.setSpacing(8)

        self.options_group = QGroupBox()
        options_vbox = QVBoxLayout(self.options_group)
        options_vbox.setSpacing(6)

        self.chk_strip_meta = QCheckBox()
        self.chk_strip_meta.setChecked(True)
        self.chk_rand_hash = QCheckBox()
        self.chk_rand_hash.setChecked(True)
        self.chk_rand_names = QCheckBox()
        self.chk_rand_names.setChecked(False)
        self.chk_open_folder = QCheckBox()
        self.chk_open_folder.setChecked(True)

        options_vbox.addWidget(self.chk_strip_meta)
        options_vbox.addWidget(self.chk_rand_hash)
        options_vbox.addWidget(self.chk_rand_names)
        options_vbox.addWidget(self.chk_open_folder)
        right_layout.addWidget(self.options_group)

        self.log_label = QLabel()
        right_layout.addWidget(self.log_label)

        self.log_text_box = QTextEdit()
        self.log_text_box.setReadOnly(True)
        right_layout.addWidget(self.log_text_box)

        splitter.addWidget(right_container)
        splitter.setStretchFactor(0, 3)
        splitter.setStretchFactor(1, 2)
        main_layout.addWidget(splitter)

        bottom_layout = QVBoxLayout()
        bottom_layout.setSpacing(6)

        self.progress_bar = QProgressBar()
        self.progress_bar.setValue(0)
        bottom_layout.addWidget(self.progress_bar)

        btn_action_layout = QHBoxLayout()
        self.btn_start = QPushButton()
        self.btn_start.setObjectName("start_btn")
        self.btn_start.clicked.connect(self._start_task)

        self.btn_stop = QPushButton()
        self.btn_stop.setObjectName("stop_btn")
        self.btn_stop.setEnabled(False)
        self.btn_stop.clicked.connect(self._stop_task)

        btn_action_layout.addWidget(self.btn_start, stretch=4)
        btn_action_layout.addWidget(self.btn_stop, stretch=1)
        bottom_layout.addLayout(btn_action_layout)

        main_layout.addLayout(bottom_layout)

    def apply_theme(self, theme_key: str):
        self.current_theme = theme_key
        qss_content = THEMES.get(theme_key, THEMES["dark"])
        QApplication.instance().setStyleSheet(qss_content)

    def retranslate_ui(self):
        tr = TRANSLATIONS.get(self.current_lang, TRANSLATIONS["en"])

        self.setWindowTitle(tr["app_title"])
        self.menu_file.setTitle(tr["menu_file"])
        self.menu_settings.setTitle(tr["menu_settings"])
        self.menu_help.setTitle(tr["menu_help"])

        self.act_add_files.setText(tr["action_add_files"])
        self.act_add_folder.setText(tr["action_add_folder"])
        self.act_clear.setText(tr["action_clear"])
        self.act_exit.setText(tr["action_exit"])
        self.act_preferences.setText(tr["action_preferences"])
        self.act_about.setText(tr["action_about"])

        self.drop_label.setText(tr["drop_zone_hint"])
        self.file_table.setHorizontalHeaderLabels([
            tr["col_name"],
            tr["col_size"],
            tr["col_type"],
            tr["col_status"]
        ])

        self.btn_add_files.setText(tr["btn_add_files"])
        self.btn_add_folder.setText(tr["btn_add_folder"])
        self.btn_clear.setText(tr["btn_clear"])

        self.options_group.setTitle(tr["group_options"])
        self.chk_strip_meta.setText(tr["opt_strip_meta"])
        self.chk_rand_hash.setText(tr["opt_rand_hash"])
        self.chk_rand_names.setText(tr["opt_rand_names"])
        self.chk_open_folder.setText(tr["opt_open_folder"])

        self.log_label.setText(tr["log_title"])
        self.btn_start.setText(tr["btn_start"])
        self.btn_stop.setText(tr["btn_stop"])

    def _open_file_dialog(self):
        selected_files, _ = QFileDialog.getOpenFileNames(
            self,
            "Select Media Files",
            "",
            "Media Files (*.jpg *.jpeg *.png *.webp *.bmp *.tiff *.mp4 *.mov *.mkv *.avi);;All Files (*.*)"
        )
        if selected_files:
            self.add_paths_to_table(selected_files)

    def _open_folder_dialog(self):
        selected_directory = QFileDialog.getExistingDirectory(self, "Select Folder")
        if selected_directory:
            self.add_paths_to_table([selected_directory])

    def add_paths_to_table(self, paths: list[str]):
        allowed_extensions = IMAGE_EXTENSIONS | VIDEO_EXTENSIONS
        tr = TRANSLATIONS.get(self.current_lang, TRANSLATIONS["en"])

        for path_entry in paths:
            entry_path = Path(path_entry)
            if entry_path.is_dir():
                for root, _, filenames in os.walk(entry_path):
                    for name in filenames:
                        file_obj = Path(root) / name
                        if file_obj.suffix.lower() in allowed_extensions:
                            self._insert_table_row(file_obj, tr)
            elif entry_path.is_file():
                if entry_path.suffix.lower() in allowed_extensions:
                    self._insert_table_row(entry_path, tr)

    def _insert_table_row(self, file_path: Path, tr: dict):
        str_path = str(file_path)
        if str_path in self.registered_paths:
            return

        self.registered_paths.append(str_path)
        row_pos = self.file_table.rowCount()
        self.file_table.insertRow(row_pos)

        ext = file_path.suffix.lower()
        kind_label = "Image" if ext in IMAGE_EXTENSIONS else "Video"
        size_label = format_file_size(file_path.stat().st_size) if file_path.exists() else "0 B"

        name_item = QTableWidgetItem(file_path.name)
        name_item.setToolTip(str_path)
        size_item = QTableWidgetItem(size_label)
        type_item = QTableWidgetItem(kind_label)
        status_item = QTableWidgetItem(tr["status_ready"])

        name_item.setFlags(name_item.flags() ^ Qt.ItemFlag.ItemIsEditable)
        size_item.setFlags(size_item.flags() ^ Qt.ItemFlag.ItemIsEditable)
        type_item.setFlags(type_item.flags() ^ Qt.ItemFlag.ItemIsEditable)
        status_item.setFlags(status_item.flags() ^ Qt.ItemFlag.ItemIsEditable)

        self.file_table.setItem(row_pos, 0, name_item)
        self.file_table.setItem(row_pos, 1, size_item)
        self.file_table.setItem(row_pos, 2, type_item)
        self.file_table.setItem(row_pos, 3, status_item)

    def _clear_file_table(self):
        self.file_table.setRowCount(0)
        self.registered_paths.clear()
        self.progress_bar.setValue(0)

    def _append_log(self, text: str):
        timestamp = datetime.now().strftime("[%H:%M:%S]")
        self.log_text_box.append(f"{timestamp} {text}")
        scrollbar = self.log_text_box.verticalScrollBar()
        scrollbar.setValue(scrollbar.maximum())

    def _update_row_status(self, row: int, status_text: str):
        if row < self.file_table.rowCount():
            item = self.file_table.item(row, 3)
            if item:
                item.setText(status_text)

    def _update_progress(self, current: int, total: int):
        if total > 0:
            percentage = int((current / total) * 100)
            self.progress_bar.setValue(percentage)

    def _start_task(self):
        tr = TRANSLATIONS.get(self.current_lang, TRANSLATIONS["en"])

        if not self.registered_paths:
            self._append_log(tr["log_empty_list"])
            return

        file_items = []
        for i in range(self.file_table.rowCount()):
            file_items.append((i, self.registered_paths[i]))

        options = {
            "strip_meta": self.chk_strip_meta.isChecked(),
            "randomize_hash": self.chk_rand_hash.isChecked(),
            "random_names": self.chk_rand_names.isChecked(),
            "open_folder": self.chk_open_folder.isChecked()
        }

        self.btn_start.setEnabled(False)
        self.btn_stop.setEnabled(True)
        self.btn_add_files.setEnabled(False)
        self.btn_add_folder.setEnabled(False)
        self.btn_clear.setEnabled(False)
        self.progress_bar.setValue(0)

        self.processor_thread = AssetProcessorThread(file_items, options, self.current_lang)
        self.processor_thread.log_emitted.connect(self._append_log)
        self.processor_thread.row_status_updated.connect(self._update_row_status)
        self.processor_thread.progress_updated.connect(self._update_progress)
        self.processor_thread.processing_completed.connect(self._on_task_finished)
        self.processor_thread.start()

    def _stop_task(self):
        if self.processor_thread and self.processor_thread.isRunning():
            self.processor_thread.request_interruption()
            self.btn_stop.setEnabled(False)

    def _on_task_finished(self, output_path_str: str):
        self.btn_start.setEnabled(True)
        self.btn_stop.setEnabled(False)
        self.btn_add_files.setEnabled(True)
        self.btn_add_folder.setEnabled(True)
        self.btn_clear.setEnabled(True)

        if self.chk_open_folder.isChecked() and os.path.exists(output_path_str):
            if platform.system() == "Windows":
                os.startfile(output_path_str)
            elif platform.system() == "Darwin":
                subprocess.Popen(["open", output_path_str])
            else:
                subprocess.Popen(["xdg-open", output_path_str])

    def _show_preferences(self):
        dialog = PreferencesDialog(self.current_lang, self.current_theme, self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            self.current_lang = dialog.selected_lang
            self.apply_theme(dialog.selected_theme)
            self.retranslate_ui()

    def _show_about(self):
        tr = TRANSLATIONS.get(self.current_lang, TRANSLATIONS["en"])
        QMessageBox.information(self, tr["about_title"], tr["about_text"])

def main():
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())

if __name__ == "__main__":
    main()
