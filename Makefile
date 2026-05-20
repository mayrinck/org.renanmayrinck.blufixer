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

.PHONY: all clean install uninstall dist flatpak

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

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
	install -m 0644 hicolor/index.theme $(ICONDIR)/
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
	rm -rf /tmp/$$distdir/.flatpak-builder /tmp/$$distdir/.git /tmp/$$distdir/.opencode; \
	tar -C /tmp -czf $(APP_ID)-$(VERSION).tar.gz $$distdir; \
	rm -rf /tmp/$$distdir; \
	echo "Created $(APP_ID)-$(VERSION).tar.gz"

flatpak:
	flatpak-builder --force-clean --user \
		--install-deps-from=flathub \
		build-dir $(APP_ID).yml

flatpak-bundle: flatpak
	flatpak build-bundle ~/.local/share/flatpak/repo \
		$(APP_ID)-$(VERSION).flatpak $(APP_ID) $(VERSION)
