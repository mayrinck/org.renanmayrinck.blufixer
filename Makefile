APP_ID     := org.renanmayrinck.blufixer
MOCK ?= 0
VERSION := 1.6.0
TARGET     := blufixer
SRCDIR     := src
SOURCES    := $(wildcard $(SRCDIR)/*.c)
DEPFILES   := $(SOURCES:.c=.d)

PREFIX     ?= /usr/local
BINDIR     := $(DESTDIR)$(PREFIX)/bin
ICONDIR    := $(DESTDIR)$(PREFIX)/share/icons/hicolor
APPDIR     := $(DESTDIR)$(PREFIX)/share/applications
POLICYDIR  := $(DESTDIR)$(PREFIX)/share/polkit-1/actions
METADIR    := $(DESTDIR)$(PREFIX)/share/metainfo

PKGS       := gtk4 libadwaita-1
CFLAGS     ?= -O2 -g
CFLAGS     += $(shell pkg-config --cflags $(PKGS)) -DVERSION=\"$(VERSION)\" -DAPP_ID=\"$(APP_ID)\" -I$(SRCDIR) $(if $(filter 1,$(MOCK)),-DMOCK_DEVICES,)
LDFLAGS    ?=
LDFLAGS    += $(shell pkg-config --libs $(PKGS))

ARCH       := $(shell uname -m)
DEBARCH    := $(shell dpkg-architecture -q DEB_BUILD_ARCH 2>/dev/null || echo "amd64")

.PHONY: all clean install uninstall dist rpm deb rpm-mock flatpak flatpak-bundle test

all: $(TARGET)

# Dependency tracking
%.d: %.c
	$(CC) -MM -MT "$(@:.d=.o)" $(CFLAGS) $< -o $@

-include $(DEPFILES)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build:
	mkdir -p build

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
	rm -rf build
	rm -f $(DEPFILES)

test: $(TARGET)
	@if command -v xvfb-run >/dev/null 2>&1; then \
		printf '>>> Running smoke test with xvfb-run...\n'; \
		xvfb-run -a ./$(TARGET) & \
		PID=$$!; \
		sleep 2; \
		kill $$PID 2>/dev/null; \
		wait $$PID 2>/dev/null || true; \
		printf '>>> Smoke test complete.\n'; \
	else \
		printf '>>> xvfb-run not found. Skipping smoke test.\n'; \
		printf '>>> Install xvfb-run (apt: xvfb, dnf: xorg-x11-server-Xvfb)\n'; \
	fi

# Binary tarball — contains pre-compiled executable for direct use
dist: $(TARGET) | build
	@printf '\n>>> Creating binary tarball...\n'
	distdir=BluFixer-$(VERSION); \
	rm -rf /tmp/$$distdir; \
	mkdir -p /tmp/$$distdir/share/applications; \
	mkdir -p /tmp/$$distdir/share/polkit-1/actions; \
	mkdir -p /tmp/$$distdir/share/icons/hicolor; \
	install -m 0755 $(TARGET) /tmp/$$distdir/; \
	install -m 0644 $(APP_ID).desktop /tmp/$$distdir/share/applications/; \
	install -m 0644 $(APP_ID).policy /tmp/$$distdir/share/polkit-1/actions/; \
	for size in 24x24 48x48 128x128; do \
		mkdir -p /tmp/$$distdir/share/icons/hicolor/$$size/apps; \
		install -m 0644 hicolor/$$size/apps/$(APP_ID).png /tmp/$$distdir/share/icons/hicolor/$$size/apps/; \
	done; \
	mkdir -p /tmp/$$distdir/share/icons/hicolor/scalable/apps; \
	install -m 0644 hicolor/scalable/apps/$(APP_ID).svg /tmp/$$distdir/share/icons/hicolor/scalable/apps/; \
	install -m 0644 LICENSE /tmp/$$distdir/; \
	install -m 0644 README.md /tmp/$$distdir/; \
	tar -C /tmp -czf build/$$distdir-$(ARCH).tar.gz $$distdir; \
	rm -rf /tmp/$$distdir; \
	printf 'Created build/BluFixer-$(VERSION)-$(ARCH).tar.gz\n'

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

rpm: $(TARGET) | build
	@if ! which rpmbuild >/dev/null 2>&1; then \
		printf 'rpmbuild not installed.\n'; \
		printf 'On Fedora: sudo dnf install rpm-build\n'; \
		printf 'Then run: make rpm\n'; \
		exit 1; \
	fi
	@printf '\n>>> Creating source tarball for rpmbuild...\n'
	srdir=$(APP_ID)-$(VERSION); \
	rm -rf /tmp/$$srdir /tmp/$$srdir.tar.gz; \
	mkdir /tmp/$$srdir; \
	cp -a . /tmp/$$srdir/; \
	rm -rf /tmp/$$srdir/.flatpak-builder /tmp/$$srdir/.git /tmp/$$srdir/.opencode /tmp/$$srdir/build-dir /tmp/$$srdir/$(TARGET) /tmp/$$srdir/*.deb /tmp/$$srdir/*.tar.gz /tmp/$$srdir/build; \
	tar -C /tmp -czf /tmp/$$srdir.tar.gz $$srdir; \
	rm -rf /tmp/$$srdir; \
	mkdir -p $(HOME)/rpmbuild/SOURCES $(HOME)/rpmbuild/SPECS; \
	cp /tmp/$$srdir.tar.gz $(HOME)/rpmbuild/SOURCES/ && rm -f /tmp/$$srdir.tar.gz; \
	cp $(APP_ID).spec $(HOME)/rpmbuild/SPECS/
	@printf '\n>>> Running rpmbuild (this will take a while the first time)...\n'
	rpmbuild -ba $(HOME)/rpmbuild/SPECS/$(APP_ID).spec
	@printf '\n>>> Copying binary RPM to build/...\n'
	rpmfile=$$(ls $(HOME)/rpmbuild/RPMS/*/blufixer-$(VERSION)-*.rpm 2>/dev/null | head -1); \
	if [ -n "$$rpmfile" ]; then \
		cp "$$rpmfile" build/BluFixer-$(VERSION)-$(ARCH).rpm && \
		printf 'Created build/BluFixer-$(VERSION)-$(ARCH).rpm\n'; \
	else \
		printf 'WARNING: binary RPM not found in %s/rpmbuild/RPMS/\n' $(HOME); \
	fi; \
	srpms=$$(ls $(HOME)/rpmbuild/SRPMS/blufixer-$(VERSION)-*.src.rpm 2>/dev/null); \
	printf 'SRPM(s) left in %s/rpmbuild/SRPMS/ for rebuild with mock\n' $(HOME); \
	if [ -n "$$srpms" ]; then \
		for f in $$srpms; do \
			printf '  %s\n' "$$f"; \
		done; \
	fi

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
	mock -r fedora-43-x86_64 --rebuild $(HOME)/rpmbuild/SRPMS/blufixer-$(VERSION)-*.src.rpm

deb: $(TARGET) | build
	@printf '\n>>> Creating .deb package...\n'
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
	sed 's/@VERSION@/$(VERSION)/g; s/@ARCH@/$(DEBARCH)/g' \
		debian/control > /tmp/pkg-deb/DEBIAN/control
	dpkg-deb --build --root-owner-group /tmp/pkg-deb build/BluFixer-$(VERSION)-$(DEBARCH).deb
	@printf '\nCreated build/BluFixer-$(VERSION)-$(DEBARCH).deb\n'
