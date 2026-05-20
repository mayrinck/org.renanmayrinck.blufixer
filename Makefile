APP_ID     := org.renanmayrinck.blufixer
VERSION    := 1.5.0
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

.PHONY: all clean install uninstall dist flatpak flatpak-bundle rpm

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
	rm -rf /tmp/$$distdir/.flatpak-builder /tmp/$$distdir/.git /tmp/$$distdir/.opencode /tmp/$$distdir/build-dir /tmp/$$distdir/org.renanmayrinck.blufixer-1.5.0.tar.gz /tmp/$$distdir/org.renanmayrinck.blufixer-1.5.0.flatpak; \
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
