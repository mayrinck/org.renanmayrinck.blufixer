%global app_id org.renanmayrinck.blufixer

Name:       blufixer
Version:    1.6.0
Release:    1%{?dist}
Summary:    Bluetooth troubleshooting utility for Linux

License:    MIT
URL:        https://github.com/mayrinck/blufixer
Source0:    %{app_id}-%{version}.tar.gz

BuildRequires: gcc
BuildRequires: make
BuildRequires: pkgconfig
BuildRequires: pkgconfig(gtk4)
BuildRequires: pkgconfig(libadwaita-1)
BuildRequires: desktop-file-utils

Requires:   gtk4
Requires:   libadwaita
Requires:   hicolor-icon-theme
Requires:   polkit

%description
BluFixer is a graphical GTK4/libadwaita utility that diagnoses and
fixes common Bluetooth issues on Linux. It detects USB Bluetooth
adapters by manufacturer and offers targeted fixes including energy
correction for CSR dongles, ERTM disable, legacy pairing, Realtek
and Broadcom firmware installation, rfkill unblock, cache cleanup,
and service restart.

%prep
%autosetup -n %{app_id}-%{version}

%build
export CFLAGS="%{optflags} -fno-lto"
%make_build

%install
%make_install PREFIX=%{_prefix}

%post
%desktop_database_post
%icon_cache_post

%postun
%desktop_database_postun
%icon_cache_postun

%files
%license LICENSE
%doc README.md

%{_bindir}/blufixer

%{_datadir}/applications/%{app_id}.desktop
%{_datadir}/polkit-1/actions/%{app_id}.policy

%{_datadir}/icons/hicolor/scalable/apps/%{app_id}.svg
%{_datadir}/icons/hicolor/24x24/apps/%{app_id}.png
%{_datadir}/icons/hicolor/48x48/apps/%{app_id}.png
%{_datadir}/icons/hicolor/128x128/apps/%{app_id}.png
%ghost %{_datadir}/icons/hicolor/icon-theme.cache

%changelog
* Thu May 21 2026 Renan Mayrinck <renan@renanmayrinck.com> - 1.6.0-1
- Theme selector (System/Light/Dark) with persistence
- Update button in header bar for new GitHub releases
- Website link in app menu
- Legal notice in About dialog
- Modular refactoring: monolithic main.c split into 9 src/ modules
- Binary distribution tarball with pre-compiled executable
- All packages output to build/ with unified naming scheme
- RPM binary copied to build/ automatically
- BuildRequires: make added explicitly
- rpmbuild availability check before building
- New i18n keys: Theme, Legal Notice, Website, Update button
- Unused translation keys removed, update strings simplified

* Wed May 20 2026 Renan Mayrinck <renan@renanmayrinck.com> - 1.5.5-1
- Async popen migration for Bluetooth version query and device scan
- Fix command injection vulnerability via g_shell_quote on usermod
- Proper application restart via g_spawn_asyn c + g_application_quit
- Scan race condition fix (timeout cancellation + scanning flag)
- Fix widget memory leaks (g_list_free → g_list_free_full)
- Add GError logging on subprocess spawn failures
- strtol endptr validation in version parsers
- Window validity checks in GTK idle callbacks
- Remove dead code (info_body), dead typedef g_free
- Pango markup escaping fixes (<> and & in info dialogs)
- Translation audit: 6 key mismatches fixed, 3 missing entries added
- i18n for zenity askpass prompt, atexit always registered
- Case-insensitive lsusb scan filter

* Wed May 20 2026 Renan Mayrinck <renan@renanmayrinck.com> - 1.5.0-1
- Initial RPM release
