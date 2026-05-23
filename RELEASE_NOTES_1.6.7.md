# BluFixer 1.6.7

## Architecture
- Monolithic `main.c` split into 9 modular files under `src/`
- Root `main.c` deleted (dead code, never compiled)
- `src/main.c` renamed to `src/app.c`; `AppData` now a named `typedef struct` with `extern`

## Stability & Safety
- Race condition on `scanning` flag fixed via `g_atomic_int`
- Double-launch guard in `launch_async_action()` (checks widget sensitivity)
- Dynamic `error_detail` per `CommandContext` (no global buffer, thread-safe by construction)
- `last_error_detail` changed to `char *` with proper `g_free()` — memory leak fixed
- NULL guards added: error detail dialog, spinner, window/button in callbacks, manufacturer field
- `atexit(cleanup_temp_files)` registered once via static guard
- `system()` replaced with `g_spawn_command_line_sync()` in Flatpak path
- Temp file `unlink()` failures logged as warnings instead of silently cast
- Cache fix checks `/var/lib/bluetooth` exists before `rm -rf`

## System Fixes
- **Exclusive Fixes section** — device-specific fixes group (Realtek, Broadcom, GH Live) hidden by default
- **Barrot/Generic Dual Mode fix** — manages `ControllerMode=dual` in `main.conf` + restart; gated behind Energy fix
- **GH Live fix** — creates udev rules file for Guitar Hero Live controller (`12ba:0100`)
- System Fixes reordered: Energy → Dual Mode → Legacy pairing → ERTM
- All fix titles normalized to sentence case across all 5 languages
- Dead code (`g_autofree char *fw_check`) removed in Realtek fix

## Device Database & Categories
- **Sony `054c` vendor** — DualSense PS5 (`0ce6`), DualSense Edge (`0df2`), DualShock 4 Gen 1/2 (`05c4`/`09cc`), PS4 USB Adaptor (`0ba0`)
- **Sony `12ba` expanded** — Guitar Hero Live (`0100`), Rock Band Keyboard (`0210`)
- **CECHYA-0080** (`12ba:0030`) — Headset category, Sony manufacturer, PS3 licensed
- **Mouse category** — `1ea7:0064` → `"Mouse"` with `input-mouse-symbolic`
- **Category icon system** — `lookup_category` table with 8 entries, displayed at 14px in device list
- **Device name sanitisation** — `&` replaced with `-e-` in scan post-processing loop
- **Vendor ID expansion** — Intel (`8086`+`8087`), MediaTek (`0e8d`+`14c3`), Broadcom/Cypress (`0a5c`+`1000`), Sony (`12ba`+`054c`)
- PS licensed detection covers PS3, PS4, and PS5 by hardware ID

## UI & UX
- **Preferences dialog** (Ctrl+P) — Theme and Language via `AdwActionRow` + `GtkDropDown`
- **Consistent 34px height** — CSS `min-height: 34px` on all buttons and dropdowns
- **Sony badge** — black (`#000`) background, white (`#fff`) text
- **Keyboard shortcuts** — F1 (help), F5 (rescan), F12 (debug), Ctrl+Shift+C (copy tech sheet), Ctrl+P (prefs)
- **Shortcuts dialog** rewritten as `AdwAlertDialog` with `AdwPreferencesPage` layout
- **Copy format** changed to `{title}: {value}` instead of bare values
- **About dialog** uses native `AdwAboutDialog`
- Bluetooth HCI version parsed across 3 tools (bluetoothctl, btmgmt, hciconfig)
- Update button only shown when release tag > app version
- Dark mode compatible — `.device-sub` uses `alpha(@theme_fg_color, 0.55)`
- Toast error buttons made transparent
- Language change shows restart toast; theme change applies immediately

## i18n
- 5 languages: English, Portuguese, Spanish, Russian, Chinese
- Custom i18n table (no gettext dependency)
- 15 missing translation keys added, including `/var/lib/bluetooth not found`

## Build & Packaging
- **Makefile overhaul** — modular compilation, dependency tracking (`.d` files), `-DVERSION`, MOCK flag
- `make dist` — binary tarball
- `make deb` — Debian package
- `make rpm` — RPM package via rpmbuild
- `make test` — smoke test via `xvfb-run`
- All artifacts produced in `build/`
- `debian/control` fixed (Package field first for dpkg-deb compat)
- `.spec` updated with `BuildRequires: make`, changelog for 1.6.7
- Version bumped from 1.6.0 to 1.6.7 in all files
- Comprehensive `TESTING.md` (165 lines, 14 sections)
- `README.md` translated to simplified English

## Install

```bash
# Binary tarball
tar -xzf BluFixer-1.6.7-x86_64.tar.gz && cd BluFixer-1.6.7 && ./blufixer

# RPM (Fedora 43+)
sudo dnf install BluFixer-1.6.7-x86_64.rpm

# DEB (Debian 12+ / Ubuntu 24.04+)
sudo dpkg -i BluFixer-1.6.7-amd64.deb && sudo apt install -f

# From source
make && sudo make install
```

**License:** MIT — **Author:** Renan Mayrinck — **Donate:** https://liberapay.com/RenanMayrinck/
