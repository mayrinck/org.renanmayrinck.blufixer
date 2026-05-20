%global app_id org.renanmayrinck.blufixer

Name:       blufixer
Version:    1.5.0
Release:    1%{?dist}
Summary:    Bluetooth troubleshooting utility for Linux

License:    MIT
URL:        https://github.com/mayrinck/%{app_id}
Source0:    %{app_id}-%{version}.tar.gz

BuildRequires: gcc
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
* Wed May 20 2026 Renan Mayrinck <renan@renanmayrinck.com> - 1.5.0-1
- Initial RPM release
