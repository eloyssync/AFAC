# AFAC (Anti-Fingerprint Asset Cleaner)

Desktop application built with Python and PyQt6 for batch metadata removal and media asset randomization (images and videos).

## Features

- Images (JPG, PNG, WEBP):
  - Strips all EXIF, XMP, IPTC, and geotag metadata.
  - Applies micro-adjustments to contrast and brightness (+-0.4%) to guarantee hash changes without visible quality loss.
  - Performs 1-pixel boundary adjustments to alter the underlying pixel matrix.
  - Modifies MD5 and SHA256 file hashes.

- Videos (MP4, MOV):
  - Strips container and stream metadata (-map_metadata -1).
  - Re-encodes with subtle pixel noise and a 10ms audio delay to generate unique file hashes.

- Interface:
  - Drag and drop support for files and folders.
  - Multi-threaded processing (QThread) to ensure non-blocking UI.
  - Real-time logging and progress tracking.
  - Automatic output directory opening upon completion.

## Requirements

- Python 3.10 or higher
- FFmpeg (required for video processing)

## Installation

1. Clone the repository:
```bash
git clone https://github.com/your-username/afac.git
cd afac
Create and activate a virtual environment:
code
Bash
# Windows
python -m venv venv
venv\Scripts\activate

# macOS / Linux
python3 -m venv venv
source venv/bin/activate
Install dependencies:
code
Bash
pip install -r requirements.txt
Install FFmpeg:
Windows: Place ffmpeg.exe into the project root or add it to your system PATH.
macOS: brew install ffmpeg
Linux: sudo apt update && sudo apt install ffmpeg
Usage
Run the application:
code
Bash
python main.py
Drag and drop media files or directories into the file list.
Select your processing options.
Click "START CLEANING & UNIQUE".
Processed files are saved to the output/ directory without overwriting originals.
Tech Stack
Python 3.10+
PyQt6 (GUI)
Pillow (Image processing)
FFmpeg (Video processing via subprocess)
License
This project is licensed under the MIT License. See the LICENSE file for details.