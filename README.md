# AFAC (Anti-Fingerprint Asset Cleaner)

Ultra-fast desktop application built with C++ and Win32 API for batch metadata removal and media asset randomization (images and videos).

## Features
<img width="923" height="686" alt="2026-08-28_07-28-26" src="https://github.com/user-attachments/assets/8b705ec6-d71e-44ef-ab12-839d34ff4b73" />

- **Images (JPG, PNG, WEBP):**
  - Strips all EXIF, XMP, IPTC, and geotag metadata.
  - Applies micro-adjustments to contrast and brightness (±0.4%) to guarantee hash changes without visible quality loss.
  - Performs 1-pixel boundary adjustments to alter the underlying pixel matrix.
  - Modifies MD5 and SHA-256 file hashes.


- **Videos (MP4, MOV):**
  - Strips container and stream metadata (`-map_metadata -1`).
  - Re-encodes with subtle pixel noise and micro-audio adjustments to generate unique file hashes.

- **Interface:**
  - Native Win32 Drag & Drop support for files and folders.
  - Multi-threaded processing pool for zero UI freezes.
  - Real-time logging and progress tracking.
  - Automatic output directory opening upon completion.

[Download AFAC v3.0.0](https://github.com/eloyssync/AFAC/releases/tag/3.0.0)

## Requirements

- Windows 10 / 11 (x64)
- MSVC / Visual Studio 2022 (C++20)
- FFmpeg (required for video processing)

## Installation & Build

1. **Clone the repository:**
```bash
git clone https://github.com/eloyssync/AFAC.git
cd AFAC
Build:

Open AFAC.sln in Visual Studio.

Select Release mode (x64).

Build Solution (Ctrl + Shift + B).

FFmpeg Setup:

Place ffmpeg.exe into the application directory or ensure it is accessible via system PATH.

Usage
Launch AFAC.exe.

Drag and drop media files or directories into the window.

Select your processing options.

Click START PROCESSING.

Processed files are saved to the output/ directory without overwriting originals.

Tech Stack
C++20

Native Win32 API

FFmpeg Integration

License
This project is licensed under the MIT License. See the LICENSE file for details.
