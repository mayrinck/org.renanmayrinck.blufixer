#include "blufixer.h"

GList *dynamic_rows = NULL;
GtkWidget *scan_loader_row = NULL;
gint scanning = FALSE;
guint scan_timeout_id = 0;

const char* detect_manufacturer(const char *vendor, const char *product, const char *desc) {
    if (g_strcmp0(vendor, "0a12") == 0) {
        if ((product && g_strcmp0(product, "0001") == 0) ||
            (desc && g_strrstr(desc, "Barrot")))
            return "Barrot / Generic";
        return "CSR";
    }
    if (g_strcmp0(vendor, "0bda") == 0) return "Realtek";
    if (g_strcmp0(vendor, "8087") == 0) return "Intel";
    if (g_strcmp0(vendor, "0cf3") == 0) return "Qualcomm / Atheros";
    if (g_strcmp0(vendor, "0a5c") == 0) return "Broadcom / Cypress";
    if (g_strcmp0(vendor, "0e8d") == 0) return "MediaTek";
    if (g_strcmp0(vendor, "148f") == 0) return "Ralink";
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

        GtkWidget *desc_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        gtk_widget_set_halign(desc_box, GTK_ALIGN_START);
        gtk_widget_set_valign(desc_box, GTK_ALIGN_CENTER);

        GtkWidget *badge = create_brand_badge(d->manufacturer);
        g_autofree char *id_label = g_strdup_printf(_("Hardware ID: %s:%s"), d->vendor, d->product);
        GtkWidget *lbl_sub = gtk_label_new(id_label);
        gtk_widget_add_css_class(lbl_sub, "device-sub");

        gtk_box_append(GTK_BOX(desc_box), badge);
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
