# BluFixer 1.6.0

## What's New

- **Theme selector** — System / Light / Dark, persisted in `~/.config/blufixer/theme`
- **Update button** — appears in the header bar when a new GitHub release is detected
- **Website link** — in the app menu, pointing to the GitHub project page
- **Modular codebase** — monolithic `main.c` split into 9 modules under `src/` (i18n, dialogs, fixes, scan, version, platform, ui_helpers, header)
- **Legal notice** — About dialog now includes an AI assistance disclosure
- **Binary tarball** — `make dist` now ships a pre-compiled executable (extract & run)
- **Unified packaging** — all packages land in `build/BluFixer-{version}-{arch}.{ext}`; RPM and DEB output directly to `build/`
- **i18n updates** — new keys for Theme, Legal Notice, Website, Update button; unused keys removed

## Changes since v1.5.5

- Theme selector with persistence (no restart needed)
- Update button in header bar on new release
- Modular source tree (9 files, single responsibility)
- Binary distribution tarball (no compilation required)
- RPM copied to `build/` automatically after `rpmbuild`
- `.spec`: added `BuildRequires: make`, `rpmbuild` availability check
- Unified package naming: `BluFixer-{version}-{arch}.{ext}`
- New i18n entries; stale keys dropped

## Install

```bash
# Binary tarball (recommended)
tar -xzf BluFixer-1.6.0-x86_64.tar.gz && cd BluFixer-1.6.0 && ./blufixer

# RPM (Fedora 43+)
sudo dnf install build/BluFixer-1.6.0-x86_64.rpm

# DEB (Debian 12+ / Ubuntu 24.04+)
sudo dpkg -i build/BluFixer-1.6.0-amd64.deb && sudo apt install -f

# From source
make && sudo make install
```

**License:** MIT — **Author:** [Renan Mayrinck](https://www.renanmayrinck.com) — **Donate:** [LiberaPay](https://liberapay.com/RenanMayrinck/)
