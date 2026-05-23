# Testing

## Build

```bash
make clean && make         # clean rebuild
make clean && make MOCK=1  # rebuild with 18 mock devices for UI testing
```

The binary is placed at `./blufixer`. Run it from a terminal inside a desktop
session (GNOME/KDE — the app needs `$DISPLAY` or `$WAYLAND_DISPLAY`).

## Smoke test (headless)

```bash
make test
```

This runs `./blufixer` under `xvfb-run` for 2 seconds to verify the binary
loads and initialises without crashing. Useful after refactoring.

## Debug build

Build with debug flags and run with verbose logging:

```bash
make clean && make MOCK=1 CFLAGS="-O0 -g -Wall -Wextra"
G_MESSAGES_DEBUG=all ./blufixer
```

All `g_warning()` calls will print to stderr, including thread spawn failures,
popen/pclose errors, and file unlink warnings.

## Crash analysis (GDB)

```bash
G_MESSAGES_DEBUG=all gdb --args ./blufixer
```

Inside GDB:

```
(gdb) run
# reproduce the crash
(gdb) bt full
(gdb) info locals
(gdb) quit
```

## Test specific features

### Keyboard shortcuts

| Shortcut | Action |
|----------|--------|
| `F1` | Keyboard shortcuts dialog |
| `F5` | Refresh device list |
| `F12` | Toggle theme (Light / Dark) |
| `Ctrl+Shift+C` | Copy tech sheet to clipboard |
| `Ctrl+P` | Open Preferences (language, theme) |

### Mock devices (18 entries)

With `MOCK=1`, the device list shows adapters for every supported
manufacturer: CSR, Realtek, Intel, Qualcomm, Broadcom, MediaTek,
Ralink, Barrot/Generic, Sony (DualSense, CECHYA-0080, Guitar Hero Live,
Rock Band Keyboard, DualSense PS5, DualShock 4 G1/G2, PS4 USB Adaptor),
and a Generic mouse.

Select each mock device to verify:
- Tech sheet columns (name, vendor, hardware ID, Bluetooth version)
- Category icons (headset, gamepad, mouse, or none)
- Manufacturer badge colours
- PlayStation licensed row visibility and subtitle
- Per-manufacturer exclusive fixes (Realtek, Broadcom, GH Live)

### System fixes

The 4 system fixes should be tested on each manufacturer mock:

1. **Energy fix** — apply, verify `btusb.conf` created at
   `/etc/modprobe.d/btusb.conf`, revert, verify removed
2. **Enable Bluetooth dual mode** — apply, verify
   `ControllerMode = dual` in `/etc/bluetooth/main.conf`, revert
3. **Force legacy pairing mode** — apply, verify
   `Security=legacy` in `/etc/bluetooth/main.conf`, revert
4. **Disable ERTM for gamepads** — apply, verify
   `/etc/modprobe.d/bluetooth-ertm.conf` created, revert

The Dual Mode row visibility depends on the Energy fix being active.
When Energy is applied, Dual Mode appears; when reverted, it hides.

### Exclusive fixes

Select Sony `054c:0100` (GH Live) mock:
- An "Exclusive Fixes" group appears below System Fixes
- The "Fix read permission failure" entry is a device-specific udev rule
- Other Sony mocks should have no exclusive fixes

Select the `0bda` mock (Realtek):
- Exclusive Fixes shows "Install Realtek firmware (RTL8761B)"

### Error toast / details

To trigger the error detail dialog:
1. Run a fix that requires elevation (`pkexec`/`sudo`)
2. Cancel the password prompt
3. An error toast appears with "View details"
4. Clicking it opens an `AdwAlertDialog` with the command, exit code
   and a description

### Preferences dialog (`Ctrl+P`)

- Switch language, verify all 5 UI translations reload
  (restart required — a toast warns)
- Switch theme, verify colours change immediately with no restart
- The `~/.config/blufixer/{language,theme}` config files are written
  on change and read at next start

### Update check

The app fetches the latest release tag from GitHub on startup. If the
remote tag version is greater than `APP_VERSION`, a blue update button
appears in the header bar. Clicking it opens the releases page.

To test offline: disconnect the network — no update button should appear.
No crash should occur on fetch failure (the check runs in a worker thread).

### Translation coverage

The i18n table in `src/i18n.c` covers 5 languages. When adding a new
translatable string, add an entry for all 5 languages and add the key
to the `lookup` function. The `_("string")` macro resolves the key.
A missing key falls through to the system language or English.

To verify a translation:
```bash
LANGUAGE=pt_BR ./blufixer    # Brazilian Portuguese
LANGUAGE=es ./blufixer       # Spanish
LANGUAGE=ru ./blufixer       # Russian
LANGUAGE=zh_CN ./blufixer    # Chinese
```

### Bluetooth version detection

The tech sheet queries three tools in order; the first that succeeds wins:

1. `bluetoothctl show` — parses `Controller Version`
2. `btmgmt info` — detects `0x`-prefixed hex version
3. `hciconfig -a` — extracts hex version from `HCI Version:`

If none are available (e.g. no Bluetooth hardware), the subtitle shows
`—` (em dash) and the row tooltip explains the fallback chain.

### Race condition guards

- The **scan button** is disabled while a scan is in progress via
  `g_atomic_int_get(&scanning)`
- **Fix buttons** are disabled during command execution via
  `gtk_widget_set_sensitive(button, FALSE)` and re-enabled by
  `on_command_finished`
- **Double-launch** is prevented by checking sensitivity at the top
  of `launch_async_action`
- **Window destroyed**: `on_command_finished` and `on_scan_finished`
  check `gtk_widget_get_realized(app_data.window)` before touching
  any widget

### Packaging verification

```bash
make dist      # binary tarball in build/
make deb       # .deb package in build/
make rpm       # RPM + SRPM in ~/rpmbuild/
make rpm-mock  # RPM via mock for Fedora 43
```

Test each package by installing on a clean system or container and
launching the app.
