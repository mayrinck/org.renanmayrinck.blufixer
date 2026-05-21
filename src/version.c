#include "blufixer.h"

int compare_versions(const char *v1, const char *v2) {
    int a, b;
    while (*v1 && *v2) {
        if (g_ascii_isdigit(*v1) && g_ascii_isdigit(*v2)) {
            a = 0; while (g_ascii_isdigit(*v1)) { a = a * 10 + (*v1 - '0'); v1++; }
            b = 0; while (g_ascii_isdigit(*v2)) { b = b * 10 + (*v2 - '0'); v2++; }
            if (a != b) return a < b ? -1 : 1;
        } else if (*v1 == *v2) {
            v1++; v2++;
        } else {
            return *v1 < *v2 ? -1 : 1;
        }
    }
    return (*v1 || *v2) ? (*v1 ? 1 : -1) : 0;
}

static const char* hci_version_to_string(int hci_ver) {
    switch (hci_ver) {
        case 0:  return "1.0b";
        case 1:  return "1.0b";
        case 2:  return "1.1";
        case 3:  return "1.2";
        case 4:  return "2.0";
        case 5:  return "2.1";
        case 6:  return "3.0";
        case 7:  return "4.0";
        case 8:  return "4.1";
        case 9:  return "4.2";
        case 10: return "5.0";
        case 11: return "5.1";
        case 12: return "5.2";
        case 13: return "5.3";
        case 14: return "5.4";
        default: return NULL;
    }
}

static gboolean on_version_queried(gpointer user_data) {
    int hci_ver = GPOINTER_TO_INT(user_data);
    if (!gtk_widget_get_realized(app_data.window))
        return G_SOURCE_REMOVE;
    if (hci_ver >= 0) {
        const char *ver_str = hci_version_to_string(hci_ver);
        char label[128];
        if (ver_str)
            g_snprintf(label, sizeof(label), "%s %s", _("Bluetooth"), ver_str);
        else
            g_snprintf(label, sizeof(label), "HCI v%d", hci_ver);
        adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_version), label);
    } else {
        adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_version), _("Unknown"));
    }
    return G_SOURCE_REMOVE;
}

static gpointer query_bt_version_thread(gpointer user_data) {
    char buf[256];
    int hci_ver = -1;

    FILE *fp = popen(in_flatpak
        ? "flatpak-spawn --host bluetoothctl show 2>/dev/null"
        : "bluetoothctl show 2>/dev/null", "r");
    if (fp) {
        while (fgets(buf, sizeof(buf), fp)) {
            char *ver = strstr(buf, "Version:");
            if (ver) {
                char *p = ver + 8, *end = NULL;
                while (g_ascii_isspace(*p)) p++;
                if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
                    hci_ver = (int)strtol(p, &end, 16);
                else
                    hci_ver = (int)strtol(p, &end, 10);
                if (end == p) hci_ver = -1;
                break;
            }
        }
        if (pclose(fp) == -1) g_warning("bt_version: pclose(bluetoothctl) failed");
        if (hci_ver >= 0) { g_idle_add(on_version_queried, GINT_TO_POINTER(hci_ver)); return NULL; }
    } else
        g_warning("bt_version: popen(bluetoothctl) failed");

    fp = popen(in_flatpak
        ? "flatpak-spawn --host btmgmt info 2>/dev/null"
        : "btmgmt info 2>/dev/null", "r");
    if (fp) {
        while (fgets(buf, sizeof(buf), fp)) {
            char *v = strstr(buf, "version");
            if (v) {
                v += 7;
                while (g_ascii_isspace(*v)) v++;
                if (g_ascii_isdigit(*v) || (v[0] == '0' && (v[1] == 'x' || v[1] == 'X'))) {
                    char *end = NULL;
                    int base = (v[0] == '0' && (v[1] == 'x' || v[1] == 'X')) ? 16 : 10;
                    hci_ver = (int)strtol(v, &end, base);
                    if (end == v) hci_ver = -1;
                    break;
                }
            }
        }
        if (pclose(fp) == -1) g_warning("bt_version: pclose(btmgmt) failed");
        if (hci_ver >= 0) { g_idle_add(on_version_queried, GINT_TO_POINTER(hci_ver)); return NULL; }
    } else
        g_warning("bt_version: popen(btmgmt) failed");

    fp = popen(in_flatpak
        ? "flatpak-spawn --host hciconfig -a 2>/dev/null"
        : "hciconfig -a 2>/dev/null", "r");
    if (fp) {
        while (fgets(buf, sizeof(buf), fp)) {
            char *hci = strstr(buf, "HCI Version:");
            if (hci) {
                char *v = hci + 12, *end = NULL;
                while (g_ascii_isspace(*v)) v++;
                hci_ver = (int)strtol(v, &end, 16);
                if (end == v) hci_ver = -1;
                break;
            }
        }
        if (pclose(fp) == -1) g_warning("bt_version: pclose(hciconfig) failed");
    } else
        g_warning("bt_version: popen(hciconfig) failed");

    g_idle_add(on_version_queried, GINT_TO_POINTER(hci_ver));
    return NULL;
}

void query_bluetooth_version(void) {
    g_thread_new("bt-version", query_bt_version_thread, NULL);
}

static gboolean on_update_check_finished(gpointer user_data) {
    UpdateCheckData *data = (UpdateCheckData *)user_data;
    if (!data->update_btn || !app_data.window || !gtk_widget_get_realized(app_data.window)) {
        g_free(data);
        return G_SOURCE_REMOVE;
    }
    if (data->success && compare_versions(APP_VERSION, data->latest_version) < 0) {
        g_autofree char *tooltip = g_strdup_printf(_("Update to version %s"), data->latest_version);
        gtk_widget_set_tooltip_text(data->update_btn, tooltip);
        gtk_widget_set_visible(data->update_btn, TRUE);
    }
    g_free(data);
    return G_SOURCE_REMOVE;
}

static gpointer check_update_thread(gpointer user_data) {
    UpdateCheckData *data = (UpdateCheckData *)user_data;
    const char *api = "https://api.github.com/repos/mayrinck/blufixer/releases/latest";
    char cmd[512];
    if (g_strcmp0(dl_cmd, "curl") == 0)
        g_snprintf(cmd, sizeof(cmd), "curl -s %s", api);
    else
        g_snprintf(cmd, sizeof(cmd), "wget -q -O - %s", api);
    FILE *fp = popen(cmd, "r");
    if (!fp) { g_warning("check_update: popen(\"%s\") failed", cmd); g_idle_add(on_update_check_finished, data); return NULL; }
    char buf[4096];
    size_t len = fread(buf, 1, sizeof(buf) - 1, fp);
    buf[len] = '\0';
    if (pclose(fp) == -1) g_warning("check_update: pclose failed");
    const char *tag = strstr(buf, "\"tag_name\"");
    if (tag) {
        tag = strchr(tag + 10, '"');
        if (tag) {
            tag++;
            const char *end = strchr(tag, '"');
            if (end) {
                size_t tlen = end - tag;
                if (tlen < sizeof(data->latest_version)) {
                    memcpy(data->latest_version, tag, tlen);
                    data->latest_version[tlen] = '\0';
                    if (data->latest_version[0] == 'v' || data->latest_version[0] == 'V')
                        memmove(data->latest_version, data->latest_version + 1, tlen - 1);
                    data->success = TRUE;
                    g_idle_add(on_update_check_finished, data);
                    return NULL;
                }
            }
        }
    }
    g_idle_add(on_update_check_finished, data);
    return NULL;
}

void check_for_updates(GtkWidget *update_btn) {
    UpdateCheckData *check = g_new0(UpdateCheckData, 1);
    check->update_btn = update_btn;
    g_thread_new("check-update", check_update_thread, check);
}
