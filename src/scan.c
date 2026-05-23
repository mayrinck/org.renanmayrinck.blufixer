#include "blufixer.h"

GList *dynamic_rows = NULL;
GtkWidget *scan_loader_row = NULL;
gint scanning = FALSE;
guint scan_timeout_id = 0;

typedef struct { const char *vendor; const char *product; const char *label; const char *icon; } CategoryEntry;

static char* sanitize_desc(const char *s) {
    if (!s) return NULL;
    int count = 0;
    for (const char *p = s; *p; p++)
        if (*p == '&') count++;
    if (count == 0) return g_strdup(s);
    gsize len = strlen(s);
    char *r = g_malloc(len + count * 2 + 1), *dst = r;
    for (const char *src = s; *src; src++) {
        if (*src == '&') { *dst++ = '-'; *dst++ = 'e'; *dst++ = '-'; }
        else             { *dst++ = *src; }
    }
    *dst = '\0';
    return r;
}

static const CategoryEntry* lookup_category(const char *vendor, const char *product) {
    static const CategoryEntry table[] = {
        {"12ba", "0030", "Headset", "audio-headset-symbolic"},
        {"12ba", "0100", "Gamepad", "input-gaming-symbolic"},
        {"12ba", "0210", "Gamepad", "input-gaming-symbolic"},
        {"1ea7", "0064", "Mouse", "input-mouse-symbolic"},
        {"054c", "0ce6", "Gamepad", "input-gaming-symbolic"},
        {"054c", "0df2", "Gamepad", "input-gaming-symbolic"},
        {"054c", "05c4", "Gamepad", "input-gaming-symbolic"},
        {"054c", "09cc", "Gamepad", "input-gaming-symbolic"},
    };
    for (size_t i = 0; i < G_N_ELEMENTS(table); i++) {
        if (g_strcmp0(vendor, table[i].vendor) == 0 &&
            g_strcmp0(product, table[i].product) == 0)
            return &table[i];
    }
    return NULL;
}

const char* detect_manufacturer(const char *vendor, const char *product, const char *desc) {
    if (g_strcmp0(vendor, "0a12") == 0) {
        if ((product && g_strcmp0(product, "0001") == 0) ||
            (desc && g_strrstr(desc, "Barrot")))
            return "Barrot / Generic";
        return "CSR";
    }
    if (g_strcmp0(vendor, "0bda") == 0) return "Realtek";
    if (g_strcmp0(vendor, "8086") == 0 || g_strcmp0(vendor, "8087") == 0) return "Intel";
    if (g_strcmp0(vendor, "0cf3") == 0) return "Qualcomm / Atheros";
    if (g_strcmp0(vendor, "0a5c") == 0 || g_strcmp0(vendor, "1000") == 0) return "Broadcom / Cypress";
    if (g_strcmp0(vendor, "0e8d") == 0 || g_strcmp0(vendor, "14c3") == 0) return "MediaTek";
    if (g_strcmp0(vendor, "148f") == 0) return "Ralink";
    if (g_strcmp0(vendor, "12ba") == 0 || g_strcmp0(vendor, "054c") == 0) return "Sony";
    return "Generic";
}

void scan_device_free(gpointer data) {
    ScanDevice *d = (ScanDevice *)data;
    g_free(d->desc);
    g_free(d);
}

static gboolean on_scan_finished(gpointer user_data) {
    GPtrArray *devices = (GPtrArray *)user_data;
    if (!gtk_widget_get_realized(app_data.window)) {
        g_ptr_array_unref(devices);
        return G_SOURCE_REMOVE;
    }
    if (devices->len == 0) {
        GtkWidget *empty_row = adw_action_row_new();
        adw_preferences_row_set_title(ADW_PREFERENCES_ROW(empty_row),
            _("No Bluetooth devices connected or functional at the moment"));
        gtk_widget_set_sensitive(empty_row, FALSE);
        GtkWidget *info_icon = gtk_image_new_from_icon_name("dialog-information-symbolic");
        adw_action_row_add_prefix(ADW_ACTION_ROW(empty_row), info_icon);
        adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.devices_group), empty_row);
        dynamic_rows = g_list_append(dynamic_rows, empty_row);
    }
    for (guint i = 0; i < devices->len; i++) {
        ScanDevice *d = (ScanDevice *)g_ptr_array_index(devices, i);
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
        gtk_widget_set_margin_start(row_box, 16);
        gtk_widget_set_margin_end(row_box, 16);
        gtk_widget_set_margin_top(row_box, 10);
        gtk_widget_set_margin_bottom(row_box, 10);

        GtkWidget *text_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
        GtkWidget *lbl_title = gtk_label_new(d->desc);
        gtk_widget_add_css_class(lbl_title, "device-title");
        gtk_widget_set_halign(lbl_title, GTK_ALIGN_START);
        gtk_label_set_ellipsize(GTK_LABEL(lbl_title), PANGO_ELLIPSIZE_END);
        gtk_label_set_max_width_chars(GTK_LABEL(lbl_title), 32);

        GtkWidget *desc_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        gtk_widget_set_halign(desc_box, GTK_ALIGN_START);
        gtk_widget_set_valign(desc_box, GTK_ALIGN_CENTER);

        GtkWidget *badge = create_brand_badge(d->manufacturer);
        gtk_box_append(GTK_BOX(desc_box), badge);

        const CategoryEntry *cat = lookup_category(d->vendor, d->product);
        if (cat) {
            GtkWidget *cat_icon = gtk_image_new_from_icon_name(cat->icon);
            gtk_image_set_pixel_size(GTK_IMAGE(cat_icon), 14);
            gtk_widget_set_valign(cat_icon, GTK_ALIGN_CENTER);
            gtk_widget_set_tooltip_text(cat_icon, cat->label);
            gtk_box_append(GTK_BOX(desc_box), cat_icon);
        }

        g_autofree char *id_label = g_strdup_printf(_("Hardware ID: %s:%s"), d->vendor, d->product);
        GtkWidget *lbl_sub = gtk_label_new(id_label);
        gtk_widget_add_css_class(lbl_sub, "device-sub");
        gtk_box_append(GTK_BOX(desc_box), lbl_sub);
        gtk_box_append(GTK_BOX(text_box), lbl_title);
        gtk_box_append(GTK_BOX(text_box), desc_box);

        gtk_widget_set_hexpand(text_box, TRUE);
        gtk_box_append(GTK_BOX(row_box), text_box);

        GtkWidget *btn = gtk_button_new_with_label(_("Select"));
        gtk_widget_set_valign(btn, GTK_ALIGN_CENTER); gtk_widget_add_css_class(btn, "flat");
        g_object_set_data_full(G_OBJECT(btn), "vendor", g_strdup(d->vendor), g_free);
        g_object_set_data_full(G_OBJECT(btn), "product", g_strdup(d->product), g_free);
        g_object_set_data_full(G_OBJECT(btn), "desc", g_strdup(d->desc), g_free);
        g_object_set_data_full(G_OBJECT(btn), "manufacturer", g_strdup(d->manufacturer), g_free);
        g_signal_connect(btn, "clicked", G_CALLBACK(on_device_selected), NULL);

        gtk_box_append(GTK_BOX(row_box), btn);
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), row_box);

        adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.devices_group), row);
        dynamic_rows = g_list_append(dynamic_rows, row);
    }
    g_ptr_array_unref(devices);
    return G_SOURCE_REMOVE;
}

static gpointer scan_bt_devices_thread(gpointer user_data) {
    GPtrArray *devices = g_ptr_array_new_with_free_func(scan_device_free);
    const char *lsusb_cmd = in_flatpak ? "flatpak-spawn --host lsusb" : "lsusb";
    FILE *fp = popen(lsusb_cmd, "r");
    if (!fp) {
        g_warning("scan_bt_devices_thread: popen(\"%s\") failed", lsusb_cmd);
        g_atomic_int_set(&scanning, FALSE);
        g_idle_add(on_scan_finished, devices);
        return NULL;
    }
    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        g_autofree char *lc = g_ascii_strdown(line, -1);
        if (strstr(lc, "bluetooth") || strstr(lc, "wireless") || strstr(lc, "csr")) {
            char vendor[8] = "", product[8] = "", desc_buf[256] = "";
            char *id_ptr = g_strrstr(line, "ID ");
            if (id_ptr && sscanf(id_ptr, "ID %7[^:]:%7s %255[^\n]", vendor, product, desc_buf) >= 2) {
                ScanDevice *d = g_new0(ScanDevice, 1);
                g_strlcpy(d->vendor, vendor, sizeof(d->vendor));
                g_strlcpy(d->product, product, sizeof(d->product));
                d->desc = g_strstrip(g_strdup(desc_buf));
                const char *m = detect_manufacturer(vendor, product, d->desc);
                g_strlcpy(d->manufacturer, m, sizeof(d->manufacturer));
                g_ptr_array_add(devices, d);
            }
        }
    }
    if (pclose(fp) == -1)
        g_warning("scan_bt_devices_thread: pclose failed");
    g_atomic_int_set(&scanning, FALSE);

#ifdef MOCK_DEVICES
    {
        struct { const char *vendor; const char *product; const char *desc; const char *mfr; } mocks[] = {
            {"0a12", "0001", "CSR Bluetooth Dongle (Mock)", "CSR"},
            {"0bda", "1234", "Realtek Bluetooth Adapter (Mock)", "Realtek"},
            {"8087", "0025", "Intel Wireless Bluetooth (Mock)", "Intel"},
            {"0cf3", "1234", "Qualcomm Atheros Bluetooth (Mock)", "Qualcomm / Atheros"},
            {"0a5c", "1234", "Broadcom BCM20702 Bluetooth (Mock)", "Broadcom / Cypress"},
            {"0e8d", "1234", "MediaTek Bluetooth Adapter (Mock)", "MediaTek"},
            {"148f", "1234", "Ralink Bluetooth Radio (Mock)", "Ralink"},
            {"0a12", "0001", "Barrot Generic BT Dongle (Mock)", "Barrot / Generic"},
            {"12ba", "0024", "Sony PlayStation DualSense (Mock)", "Sony"},
            {"12ba", "0030", "Sony CECHYA-0080 Headset (Mock)", "Sony"},
            {"12ba", "0100", "Guitar Hero Live Dongle receiver (Mock)", "Sony"},
            {"12ba", "0210", "Rock Band Wireless Keyboard Controller Receiver (Mock)", "Sony"},
            {"054c", "0ce6", "DualSense PS5 Wireless Controller (Mock)", "Sony"},
            {"054c", "0df2", "DualSense Edge PS5 Wireless Controller (Mock)", "Sony"},
            {"054c", "05c4", "DualShock 4 Wireless Controller Gen. 1 (Mock)", "Sony"},
            {"054c", "09cc", "DualShock 4 Wireless Controller Gen. 2 (Mock)", "Sony"},
            {"054c", "0ba0", "PlayStation 4 USB Wireless Adaptor (Mock)", "Sony"},
            {"1ea7", "0064", "Generic Bluetooth Mouse (Mock)", "Generic"},
        };
        for (size_t i = 0; i < G_N_ELEMENTS(mocks); i++) {
            ScanDevice *d = g_new0(ScanDevice, 1);
            g_strlcpy(d->vendor, mocks[i].vendor, sizeof(d->vendor));
            g_strlcpy(d->product, mocks[i].product, sizeof(d->product));
            d->desc = g_strdup(mocks[i].desc);
            g_strlcpy(d->manufacturer, mocks[i].mfr, sizeof(d->manufacturer));
            g_ptr_array_add(devices, d);
        }
    }
#endif

    /* Device-specific overrides */
    for (guint i = 0; i < devices->len; i++) {
        ScanDevice *d = (ScanDevice *)g_ptr_array_index(devices, i);
        if (g_strcmp0(d->vendor, "12ba") == 0) {
            if (g_strcmp0(d->product, "0030") == 0) {
                g_free(d->desc);
                d->desc = g_strdup("CECHYA-0080");
            } else if (g_strcmp0(d->product, "0100") == 0) {
                g_free(d->desc);
                d->desc = g_strdup("Guitar Hero Live Dongle receiver");
            } else if (g_strcmp0(d->product, "0210") == 0) {
                g_free(d->desc);
                d->desc = g_strdup("Rock Band Wireless Keyboard Controller Receiver");
            }
        }
        if (g_strcmp0(d->vendor, "054c") == 0) {
            if (g_strcmp0(d->product, "0ce6") == 0) {
                g_free(d->desc);
                d->desc = g_strdup("DualSense PS5 Wireless Controller");
            } else if (g_strcmp0(d->product, "0df2") == 0) {
                g_free(d->desc);
                d->desc = g_strdup("DualSense Edge PS5 Wireless Controller");
            } else if (g_strcmp0(d->product, "05c4") == 0) {
                g_free(d->desc);
                d->desc = g_strdup("DualShock 4 Wireless Controller Gen. 1");
            } else if (g_strcmp0(d->product, "09cc") == 0) {
                g_free(d->desc);
                d->desc = g_strdup("DualShock 4 Wireless Controller Gen. 2");
            } else if (g_strcmp0(d->product, "0ba0") == 0) {
                g_free(d->desc);
                d->desc = g_strdup("PlayStation 4 USB Wireless Adaptor");
            }
        }
    }

    for (guint i = 0; i < devices->len; i++) {
        ScanDevice *d = (ScanDevice *)g_ptr_array_index(devices, i);
        char *clean = sanitize_desc(d->desc);
        g_free(d->desc);
        d->desc = clean;
    }

    g_idle_add(on_scan_finished, devices);
    return NULL;
}

void scan_bluetooth_devices(void) {
    if (g_atomic_int_get(&scanning)) return;

    for (GList *l = dynamic_rows; l != NULL; l = l->next) {
        adw_preferences_group_remove(ADW_PREFERENCES_GROUP(app_data.devices_group), GTK_WIDGET(l->data));
    }
    g_list_free(dynamic_rows);
    dynamic_rows = NULL;

    g_atomic_int_set(&scanning, TRUE);
    g_thread_new("bt-scan", scan_bt_devices_thread, NULL);
}

gboolean delayed_scan(gpointer user_data) {
    scan_timeout_id = 0;
    if (scan_loader_row) {
        adw_preferences_group_remove(ADW_PREFERENCES_GROUP(app_data.devices_group), scan_loader_row);
        scan_loader_row = NULL;
    }
    scan_bluetooth_devices();
    return G_SOURCE_REMOVE;
}

void on_scan_clicked(GtkButton *btn, gpointer user_data) {
    if (scan_timeout_id > 0) {
        g_source_remove(scan_timeout_id);
        scan_timeout_id = 0;
    }

    for (GList *l = dynamic_rows; l != NULL; l = l->next)
        adw_preferences_group_remove(ADW_PREFERENCES_GROUP(app_data.devices_group), GTK_WIDGET(l->data));
    g_list_free(dynamic_rows);
    dynamic_rows = NULL;

    GtkWidget *row = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), _("Scanning devices..."));
    gtk_widget_set_sensitive(row, FALSE);
    GtkWidget *spinner = gtk_spinner_new();
    gtk_spinner_start(GTK_SPINNER(spinner));
    adw_action_row_add_prefix(ADW_ACTION_ROW(row), spinner);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.devices_group), row);
    scan_loader_row = row;

    scan_timeout_id = g_timeout_add(500, delayed_scan, NULL);
}
