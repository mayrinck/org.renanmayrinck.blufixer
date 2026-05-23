# BluFixer

A Linux GUI tool to detect and fix Bluetooth and Wireless hardware issues using **GTK4** and **libadwaita**.

## Features

- **Device scanning** — detects Bluetooth USB adapters via `lsusb`
- **Tech sheet** — shows device name, manufacturer, and hardware ID
- **Theme picker** — Light, Dark, or System (persists between sessions)
- **Update button** — appears in the header when a new release is found on GitHub
- **System fixes** (revertible):
  - Power fix for generic dongles, like CSR/Barrot 
  - Disable ERTM for gamepads and joysticks
  - Legacy pairing mode (PIN) for old devices
  - Realtek RTL8761B firmware install
  - Broadcom/Cypress b43 firmware install
- **Immediate actions**:
  - Unblock antenna (rfkill)
  - Add user to lp group
  - Clear device cache
  - Restart Bluetooth service
- **Multi-platform detection** — auto-detects: `pkexec`/`sudo`, `wget`/`curl`, `systemctl`/`service`, package manager, and firmware path
- **5 languages** — English, Portuguese, Spanish, Russian, Chinese
- **Modular code** — 9 modules under `src/`

## Build

### Dependencies

- `gtk4`, `libadwaita`, `glib` development packages
- `pkg-config`

### Quick start

```bash
make                # compile
sudo make install   # install to /usr/local
sudo make uninstall # remove
```

### Packages

```bash
make dist           # binary tar.gz (no compilation needed on target)
make deb            # .deb package (Debian/Ubuntu)
make rpm            # RPM package (Fedora/RHEL)
make rpm-mock       # RPM via mock for Fedora 43
make flatpak        # Flatpak build
make flatpak-bundle # .flatpak bundle for offline distribution
```

### Binary tarball usage

```bash
tar -xzf BluFixer-1.6.7-x86_64.tar.gz
cd BluFixer-1.6.7
./blufixer
```

System-wide install from tarball:

```bash
sudo install -m 0755 blufixer /usr/local/bin/
sudo install -m 0644 share/applications/*.desktop /usr/local/share/applications/
sudo install -m 0644 share/polkit-1/actions/*.policy /usr/local/share/polkit-1/actions/
sudo install -m 0644 share/icons/hicolor/*/apps/* /usr/local/share/icons/hicolor/*/apps/
gtk-update-icon-cache -f -t /usr/local/share/icons/hicolor 2>/dev/null || true
```

## License

MIT License. Copyright © 2026 Renan Mayrinck.

## Author

**Renan Mayrinck** — [renanmayrinck.com](https://www.renanmayrinck.com) — [Support via LiberaPay](https://liberapay.com/RenanMayrinck/)

### AI notice

This project was built with **Opencode**, an AI coding assistant. The code is fully functional, but may not follow the best practices adopted by professional developers. Contributions and suggestions are welcome via [pull requests or issues](https://github.com/mayrinck/blufixer/issues).
