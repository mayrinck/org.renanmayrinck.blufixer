APP_ID     := org.renanmayrinck.blufixer
VERSION    := 1.5.5
TARGET     := blufixer
SOURCE     := main.c

PREFIX     ?= /usr/local
BINDIR     := $(DESTDIR)$(PREFIX)/bin
ICONDIR    := $(DESTDIR)$(PREFIX)/share/icons/hicolor
APPDIR     := $(DESTDIR)$(PREFIX)/share/applications
POLICYDIR  := $(DESTDIR)$(PREFIX)/share/polkit-1/actions
METADIR    := $(DESTDIR)$(PREFIX)/share/metainfo

PKGS       := gtk4 libadwaita-1
CFLAGS     ?= -O2 -g
CFLAGS     += $(shell pkg-config --cflags $(PKGS)) -DVERSION=\"$(VERSION)\" -DAPP_ID=\"$(APP_ID)\"
LDFLAGS    ?=
LDFLAGS    += $(shell pkg-config --libs $(PKGS))

.PHONY: all clean install uninstall dist flatpak flatpak-bundle rpm deb

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	install -d $(BINDIR)
	install -m 0755 $(TARGET) $(BINDIR)/$(TARGET)
	install -d $(POLICYDIR)
	install -m 0644 $(APP_ID).policy $(POLICYDIR)/
	install -d $(APPDIR)
	install -m 0644 $(APP_ID).desktop $(APPDIR)/
	for size in 24x24 48x48 128x128; do \
		install -d $(ICONDIR)/$$size/apps; \
		install -m 0644 hicolor/$$size/apps/$(APP_ID).png $(ICONDIR)/$$size/apps/; \
	done
	install -d $(ICONDIR)/scalable/apps
	install -m 0644 hicolor/scalable/apps/$(APP_ID).svg $(ICONDIR)/scalable/apps/
	gtk-update-icon-cache -f -t $(ICONDIR) 2>/dev/null || true

uninstall:
	rm -f $(BINDIR)/$(TARGET)
	rm -f $(POLICYDIR)/$(APP_ID).policy
	rm -f $(APPDIR)/$(APP_ID).desktop
	for size in 24x24 48x48 128x128; do \
		rm -f $(ICONDIR)/$$size/apps/$(APP_ID).png; \
	done
	rm -f $(ICONDIR)/scalable/apps/$(APP_ID).svg
	-rmdir $(ICONDIR)/scalable/apps 2>/dev/null; true
	gtk-update-icon-cache -f -t $(ICONDIR) 2>/dev/null || true

clean:
	rm -f $(TARGET)

dist: clean
	distdir=$(APP_ID)-$(VERSION); \
	rm -rf /tmp/$$distdir /tmp/$$distdir.tar.gz; \
	mkdir /tmp/$$distdir; \
	cp -a . /tmp/$$distdir/; \
	rm -rf /tmp/$$distdir/.flatpak-builder /tmp/$$distdir/.git /tmp/$$distdir/.opencode /tmp/$$distdir/build-dir /tmp/$$distdir/org.renanmayrinck.blufixer-1.5.5.tar.gz /tmp/$$distdir/org.renanmayrinck.blufixer-1.5.5.flatpak /tmp/$$distdir/blufixer*.deb; \
	tar -C /tmp -czf $(APP_ID)-$(VERSION).tar.gz $$distdir; \
	rm -rf /tmp/$$distdir; \
	echo "Created $(APP_ID)-$(VERSION).tar.gz"

flatpak:
	rm -rf /tmp/flatpak-build /tmp/flatpak-state
	flatpak-builder --force-clean --user \
		--disable-rofiles-fuse \
		--install \
		--state-dir=/tmp/flatpak-state \
		--install-deps-from=flathub \
		/tmp/flatpak-build $(APP_ID).yml

flatpak-bundle: flatpak
	flatpak build-bundle $(HOME)/.local/share/flatpak/repo \
		$(APP_ID)-$(VERSION).flatpak $(APP_ID) master

rpm: dist
	mkdir -p $(HOME)/rpmbuild/SOURCES $(HOME)/rpmbuild/SPECS
	cp $(APP_ID)-$(VERSION).tar.gz $(HOME)/rpmbuild/SOURCES/
	cp $(APP_ID).spec $(HOME)/rpmbuild/SPECS/
	rpmbuild -ba $(HOME)/rpmbuild/SPECS/$(APP_ID).spec
	@echo ""
	@echo "RPMs created:"
	@ls -1 $(HOME)/rpmbuild/RPMS/x86_64/$(APP_ID)-*.rpm 2>/dev/null || true
	@ls -1 $(HOME)/rpmbuild/SRPMS/$(APP_ID)-*.rpm 2>/dev/null || true
	@echo ""
	@echo "To rebuild on another Fedora release:"
	@echo "  rpm --rebuild $(HOME)/rpmbuild/SRPMS/blufixer-$(VERSION)-*.src.rpm"
	@echo ""

rpm-mock: rpm
	@if ! which mock >/dev/null 2>&1; then \
		echo "mock not installed. Install it:"; \
		echo "  sudo dnf install mock"; \
		echo "  sudo usermod -a -G mock $$$${USER}"; \
		exit 1; \
	fi
	@if [ ! -f /etc/mock/fedora-43-x86_64.cfg ]; then \
		echo "Installing Fedora 43 mock config..."; \
		sudo install -m 0644 fedora-43-x86_64.cfg /etc/mock/ 2>/dev/null || true; \
		if [ ! -f /etc/mock/fedora-43-x86_64.cfg ]; then \
			echo "Could not install mock config. Copy it manually:"; \
			echo "  sudo install -m 0644 fedora-43-x86_64.cfg /etc/mock/"; \
			exit 1; \
		fi; \
	fi
	mock -r fedora-43-x86_64 --rebuild $(HOME)/rpmbuild/SRPMS/$(APP_ID)-$(VERSION)-*.src.rpm

DEBARCH := $(shell dpkg-architecture -q DEB_BUILD_ARCH 2>/dev/null || echo "amd64")
DEB_VERSION := $(VERSION)-1
DEB_NAME    := blufixer_$(DEB_VERSION)_$(DEBARCH).deb
DEB_FILES   := blufixer \
               $(APP_ID).policy \
               $(APP_ID).desktop \
               hicolor/24x24/apps/$(APP_ID).png \
               hicolor/48x48/apps/$(APP_ID).png \
               hicolor/128x128/apps/$(APP_ID).png \
               hicolor/scalable/apps/$(APP_ID).svg

deb: $(TARGET)
	rm -rf /tmp/pkg-deb
	mkdir -p /tmp/pkg-deb/DEBIAN
	mkdir -p /tmp/pkg-deb/usr/bin
	mkdir -p /tmp/pkg-deb/usr/share/polkit-1/actions
	mkdir -p /tmp/pkg-deb/usr/share/applications
	mkdir -p /tmp/pkg-deb/usr/share/icons/hicolor/24x24/apps
	mkdir -p /tmp/pkg-deb/usr/share/icons/hicolor/48x48/apps
	mkdir -p /tmp/pkg-deb/usr/share/icons/hicolor/128x128/apps
	mkdir -p /tmp/pkg-deb/usr/share/icons/hicolor/scalable/apps
	install -m 0755 blufixer /tmp/pkg-deb/usr/bin/
	install -m 0644 $(APP_ID).policy /tmp/pkg-deb/usr/share/polkit-1/actions/
	install -m 0644 $(APP_ID).desktop /tmp/pkg-deb/usr/share/applications/
	install -m 0644 hicolor/24x24/apps/$(APP_ID).png /tmp/pkg-deb/usr/share/icons/hicolor/24x24/apps/
	install -m 0644 hicolor/48x48/apps/$(APP_ID).png /tmp/pkg-deb/usr/share/icons/hicolor/48x48/apps/
	install -m 0644 hicolor/128x128/apps/$(APP_ID).png /tmp/pkg-deb/usr/share/icons/hicolor/128x128/apps/
	install -m 0644 hicolor/scalable/apps/$(APP_ID).svg /tmp/pkg-deb/usr/share/icons/hicolor/scalable/apps/
	install -m 0755 debian/postinst /tmp/pkg-deb/DEBIAN/postinst
	install -m 0755 debian/postrm /tmp/pkg-deb/DEBIAN/postrm
	printf 'Package: blufixer\n' > /tmp/pkg-deb/DEBIAN/control
	printf 'Version: $(DEB_VERSION)\n' >> /tmp/pkg-deb/DEBIAN/control
	printf 'Architecture: $(DEBARCH)\n' >> /tmp/pkg-deb/DEBIAN/control
	printf 'Maintainer: Renan Mayrinck <renan@mayrinck.com>\n' >> /tmp/pkg-deb/DEBIAN/control
	printf 'Section: utils\n' >> /tmp/pkg-deb/DEBIAN/control
	printf 'Priority: optional\n' >> /tmp/pkg-deb/DEBIAN/control
	printf 'Depends: libadwaita-1-0 (>= 1.0),\n' >> /tmp/pkg-deb/DEBIAN/control
	printf '         libgtk-4-1 (>= 4.0),\n' >> /tmp/pkg-deb/DEBIAN/control
	printf '         libglib2.0-0 (>= 2.80),\n' >> /tmp/pkg-deb/DEBIAN/control
	printf '         libc6 (>= 2.38),\n' >> /tmp/pkg-deb/DEBIAN/control
	printf '         policykit-1\n' >> /tmp/pkg-deb/DEBIAN/control
	printf 'Description: Bluetooth troubleshooting utility for Linux\n' >> /tmp/pkg-deb/DEBIAN/control
	printf ' BluFixer helps diagnose and fix common Bluetooth issues on Linux\n' >> /tmp/pkg-deb/DEBIAN/control
	printf ' desktops including CSR Energy Management, ERTM, Realtek/Broadcom\n' >> /tmp/pkg-deb/DEBIAN/control
	printf ' firmware, and Legacy pairing mode.\n' >> /tmp/pkg-deb/DEBIAN/control
	dpkg-deb --build --root-owner-group /tmp/pkg-deb $(DEB_NAME)
	@echo ""
	@echo "Created $(DEB_NAME)"
