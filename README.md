[![Build for Windows using MSVC](https://github.com/P-O-P-E/geiss/actions/workflows/windows-msvc.yml/badge.svg)](https://github.com/P-O-P-E/geiss/actions/workflows/windows-msvc.yml)


# Geiss Standalone, Screensaver, and Winamp plug-in

This repository holds the latest code of Geiss. It now includes a standalone,
64-bit Windows application in addition to the legacy screensaver and Winamp plug-in.
for general information about Geiss please see:

- [At a glance: Geiss](https://www.geisswerks.com/about_geiss.html)
- [Official Homepage of the Geiss Screensaver and Winamp plug-in](https://www.geisswerks.com/geiss/)
- [Screenshots from Geiss](https://www.geisswerks.com/geiss/shots.html)
- [How 'Geiss' Worked](https://www.geisswerks.com/geiss/secrets.html)
- [Geiss @ WinampHeritage.com](https://winampheritage.com/visualizations/geiss)
- [Geisswerks — home page of Ryan M. Geiss](https://www.geisswerks.com/)

For a YouTube video showing Geiss in action, please click:

[![Video "Windows 95 + Winamp + Geiss = Aesthetic"](https://img.youtube.com/vi/MfFcRsUZCxw/hqdefault.jpg)](https://youtu.be/MfFcRsUZCxw?t=15)


# Standalone app for Windows 11

The standalone build runs directly as `Geiss.exe`; Winamp is not required and the
file does not need to be installed as a screensaver. It uses WASAPI to capture system output or the default microphone/recording device.
System output is the default and does not require Stereo Mix. The startup settings
and the **Tab** menu select the source; changing it during playback takes effect
immediately and saves the choice. Select the same source again to retry capture
or pick up a changed Windows default device. Use Windows Sound settings to select
the default playback/recording endpoint. Microphone capture needs Windows microphone
access enabled for desktop apps.

Standalone commands:

| Command | Action |
| --- | --- |
| Tab | Audio source, audio on/off, help, and exit menu |
| O | Toggle audio reaction |
| H / F | Toggle help / frame-rate display |
| Esc | Cancel an unfinished command; otherwise exit |
| N | Random visual mode |
| W / P | Next waveform / palette |
| L / T | Lock visual mode / palette |
| I | Toggle shifting |
| J / K | Scale waveform down / up |
| Q / E / U / G / D / A / Y | Toggle visual effects (use uppercase A and Y) |
| Two digits | Select visual mode 01–25 |
| < / > | Decrease / increase visual mode rating |

Mouse clicks do not exit. CD playback shortcuts belong to the legacy screensaver
and are disabled in the standalone app. Capture failures produce an on-screen
message; use Tab to retry after checking the Windows default device and access.


The x64 renderer replaces the original self-modifying x86/MMX routines with
portable C++ and uses the Windows Concurrency Runtime to process independent
scanlines across the system's worker pool. The executable is large-address-aware,
DEP/ASLR enabled, and Per-Monitor-V2 DPI aware.

Build with Visual Studio 2022:

```console
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DGEISS_BUILD_LEGACY=OFF
cmake --build build --config RelWithDebInfo --target geiss
```

The executable is written to `build/RelWithDebInfo/Geiss.exe`.

# Legacy builds

To compile the Geiss Winamp plug-in and/or Geiss Screensaver,
you need:

- A Git clone or snapshot of this repository
- CMake >=3.15
- Microsoft Visual Studio >=9/2008 (due to the current use of MSVC style assembly)

Then, the code can be built using:

```console
cmake -S . -B build-x86 -G "Visual Studio 17 2022" -A Win32 -DGEISS_BUILD_LEGACY=ON
cmake --build build-x86 --config RelWithDebInfo --target geiss_scr vis_geis
```
