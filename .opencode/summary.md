# Summary

## Goal
Maintain and enhance BluFixer, a GTK4/libadwaita Bluetooth troubleshooting utility, with reliable state detection, robust error reporting, proper branding, and cross-distro package support.

## Constraints & Preferences
- All system fixes (CSR, ERTM, Realtek, Legacy, Broadcom) must detect their own applied state after command execution via file/content checks, not `system()` exit code.
- Button text must show "Aplicar"/"Instalar" when fix is inactive and "Reverter"/"Remover"/"Instalado" when active.
- Cross-distro detection (`pkexec`/`sudo`, `wget`/`curl`, `systemctl`/`service`, firmware path, package managers) must be preserved.
- Action section order: Descongestionar antena, Adicionar permissões ao usuário atual, Limpar cache de dispositivos, Reiniciar Serviço Bluetooth.
- Error toasts must be visually distinct (red button, HIGH priority) and include a "Ver detalhes" button.
- Logo must appear in the "Sobre" dialog and executable, but NOT in the header bar (GNOME HIG compliance).
- Manufacturer detection must use USB vendor ID, not description string matching.
- "Genérico" replaced with "Generic" for universal understanding.

## Progress
### Done
- Async command infrastructure: `CommandContext` holds `is_active` function pointer and `expect_active` flag; `on_command_finished` verifies state and shows toast accordingly.
- Four verification helpers: `csr_is_active()` (file `/etc/modprobe.d/btusb.conf`), `ertm_is_active()` (file `/etc/modprobe.d/bluetooth-ertm.conf`), `realtek_is_active()` (firmware file at fw_path), `legacy_is_active()` (grep `Security=legacy`), `broadcom_fw_is_active()` (directory `/lib/firmware/b43`).
- Legacy apply command fixed: now deletes any `Security=` line and appends `Security=legacy` instead of fragile sed replace.
- Error toasts: "Ver detalhes" button via `adw_toast_set_button_label`, HIGH priority, red CSS via `toast button:not(.image-button)`, dialog with command and exit code via `on_show_error_detail` action.
- Logo integration: `hicolor/` icon theme directory created with SVG and PNGs (24x24, 48x48, 128x128), search path registered in `activate()` via `gtk_icon_theme_add_search_path`, about dialog uses `org.renanmayrinck.blufixer` icon name, header bar logo removed per HIG.
- `.desktop` file: `org.renanmayrinck.blufixer.desktop` created with `Categories=Utility;` and `Icon=org.renanmayrinck.blufixer`.
- Manufacturer detection rewritten: `detect_manufacturer(vendor, product, desc)` maps vendor IDs to brands (0bda→Realtek, 8087→Intel, 0cf3→Qualcomm/Atheros, 0a5c→Broadcom/Cypress, 0e8d→MediaTek, 148f→Ralink, 0a12→CSR or Generic if product=0001 or "Barrot"), stored as GObject data on each device button.
- Brand badges with distinct hex colors: Realtek `#00f`, Intel `#127CC1`, Qualcomm/Atheros `#2e52dd`, Broadcom/Cypress `#e00`, MediaTek `#e66000`, Ralink `#2ECC71`, CSR `#8E44AD`, Generic `#888`.
- Scan loader: `on_scan_clicked` clears list, shows `AdwActionRow` with `GtkSpinner` ("Escaneando dispositivos..."), schedules `delayed_scan` via `g_timeout_add(500, ...)`. Toast removed.
- CSR fix row always visible (not restricted to CSR vendor).
- Bluetooth version field: `row_tech_version` added to tech specs, `query_bluetooth_version()` tries `bluetoothctl show` then `hciconfig -a`, fallsback to "Desconhecida".
- Broadcom/Cypress fix row: installs `b43-fwcutter` (or `firmware-b43-installer` on Debian, `b43-openfwwf` on Fedora) via detected package manager (`dnf`/`apt-get`/`zypper`/`pacman`), reloads `b43` module; info dialog explains the firmware dependency. Button shows "Instalar" when firmware missing, "Instalado" (flat) when present.
- Fake test entries removed.

### In Progress
- (none)

### Blocked
- (none)

## Key Decisions
- `gtk_icon_theme_add_search_path` receives the project root (parent of `hicolor/`), not `hicolor/` directly, and an `index.theme` file is required for icon theme resolution.
- Manufacturer detection migrated from description substring matching to vendor ID parsing, with CSR exception for product `0001` or "Barrot" → Generic.
- Toast close button excluded from red styling via `:not(.image-button)` selector (icon-only buttons have that class).
- Bluetooth version queried at device selection via synchronous `popen` (fast path via `bluetoothctl show`, fallback `hciconfig -a`).
- Broadcom fix uses cross-distro package manager detection inside the callback, not a global variable; applies only to devices with vendor `0a5c`. One-shot install (no revert) with "Instalado" flat button state.

## Relevant Files
- `/mnt/BACKUP/GithubRepos/org.renanmayrinck.blufixer/main.c`: all application code; single-file GTK4+libadwaita project with ~1065 lines.
- `/mnt/BACKUP/GithubRepos/org.renanmayrinck.blufixer/logo.svg`: application icon source.
- `/mnt/BACKUP/GithubRepos/org.renanmayrinck.blufixer/hicolor/`: icon theme directory (SVG + PNGs + index.theme).
- `/mnt/BACKUP/GithubRepos/org.renanmayrinck.blufixer/org.renanmayrinck.blufixer.desktop`: desktop entry.
- `/mnt/BACKUP/GithubRepos/org.renanmayrinck.blufixer/README.md`: project documentation.
