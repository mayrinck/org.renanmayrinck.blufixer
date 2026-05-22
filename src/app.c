#include "blufixer.h"

ThemeId current_theme = THEME_SYSTEM;

AppData app_data;

static void on_about_action(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    AdwAboutDialog *about = ADW_ABOUT_DIALOG(adw_about_dialog_new());
    adw_about_dialog_set_application_name(about, "BluFixer");
    adw_about_dialog_set_version(about, APP_VERSION);
    adw_about_dialog_set_developer_name(about, "Renan Mayrinck");
    adw_about_dialog_set_copyright(about, "\u00a92026 Renan Mayrinck");
    adw_about_dialog_set_application_icon(about, "org.renanmayrinck.blufixer");
    adw_about_dialog_add_link(about, _("GitHub Repository"), "https://github.com/mayrinck/blufixer");
    adw_about_dialog_set_issue_url(about, "https://github.com/mayrinck/blufixer/issues");

    g_autofree char *legal = g_strdup_printf(
        "%s\n\n"
        "MIT License\n"
        "Copyright \u00a92026 Renan Mayrinck\n\n"
        "Permission is hereby granted, free of charge, to any person obtaining a copy of "
        "this software and associated documentation files (the \"Software\"), to deal "
        "in the Software without restriction, including without limitation the rights "
        "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell "
        "copies of the Software, and to permit persons to whom the Software is "
        "furnished to do so, subject to the following conditions:\n\n"
        "The above copyright notice and this permission notice shall be included in all "
        "copies or substantial portions of the Software.\n\n"
        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR "
        "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
        "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE "
        "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
        "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
        "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE "
        "SOFTWARE.\n\n"
        "%s",
        _("Developed by Renan Mayrinck."),
        _("This software was developed with the assistance of artificial intelligence."));
    adw_about_dialog_add_legal_section(about, _("Legal Notice"), "\u00a92026 Renan Mayrinck", GTK_LICENSE_CUSTOM, legal);

    adw_dialog_present(ADW_DIALOG(about), app_data.window);
}

static void on_donate_action(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    const char *url = "https://liberapay.com/RenanMayrinck/";
    GtkUriLauncher *launcher = gtk_uri_launcher_new(url);
    gtk_uri_launcher_launch(launcher, GTK_WINDOW(app_data.window), NULL, NULL, NULL);
    g_object_unref(launcher);
}

static void on_show_error_detail(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    if (strlen(last_error_detail) == 0) return;

    AdwAlertDialog *dialog = ADW_ALERT_DIALOG(adw_alert_dialog_new(_("Operation failed"), NULL));

    g_autofree char *valid = g_utf8_make_valid(last_error_detail, -1);
    g_autofree char *escaped = g_markup_escape_text(valid, -1);
    g_autofree char *markup = g_strdup_printf(
        "<b>%s</b>\n<tt>%s</tt>\n\n%s",
        info5("Executed command:", "Comando executado:",
              "Comando ejecutado:",
              "\u0412\u044b\u043f\u043e\u043b\u043d\u0435\u043d\u043d\u0430\u044f \u043a\u043e\u043c\u0430\u043d\u0434\u0430:",
              "\u5df2\u6267\u884c\u7684\u547d\u4ee4:"),
        escaped,
        info5("Check if the command was executed correctly. You can copy the text below to report the problem.",
              "Verifique se o comando acima foi executado corretamente. Voc\u00ea pode copiar o texto abaixo para reportar o problema.",
              "Verifique si el comando se ejecut\u00f3 correctamente. Puede copiar el texto a continuaci\u00f3n para informar del problema.",
              "\u041f\u0440\u043e\u0432\u0435\u0440\u044c\u0442\u0435, \u0431\u044b\u043b\u0430 \u043b\u0438 \u043a\u043e\u043c\u0430\u043d\u0434\u0430 \u0432\u044b\u043f\u043e\u043b\u043d\u0435\u043d\u0430 \u043f\u0440\u0430\u0432\u0438\u043b\u044c\u043d\u043e. \u0412\u044b \u043c\u043e\u0436\u0435\u0442\u0435 \u0441\u043a\u043e\u043f\u0438\u0440\u043e\u0432\u0430\u0442\u044c \u0442\u0435\u043a\u0441\u0442 \u043d\u0438\u0436\u0435, \u0447\u0442\u043e\u0431\u044b \u0441\u043e\u043e\u0431\u0449\u0438\u0442\u044c \u043e \u043f\u0440\u043e\u0431\u043b\u0435\u043c\u0435.",
              "\u68c0\u67e5\u547d\u4ee4\u662f\u5426\u5df2\u6b63\u786e\u6267\u884c\u3002\u60a8\u53ef\u4ee5\u590d\u5236\u4e0b\u9762\u7684\u6587\u672c\u6765\u62a5\u544a\u95ee\u9898\u3002"));

    GtkWidget *label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), markup);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);
    gtk_widget_set_margin_start(label, 12);
    gtk_widget_set_margin_end(label, 12);
    gtk_widget_set_margin_bottom(label, 12);

    adw_alert_dialog_set_extra_child(dialog, label);
    adw_alert_dialog_add_responses(dialog, "close", _("Close"), NULL);
    adw_alert_dialog_set_default_response(dialog, "close");
    adw_alert_dialog_set_close_response(dialog, "close");

    adw_dialog_present(ADW_DIALOG(dialog), GTK_WIDGET(app_data.window));
}

void detect_theme(void) {
    current_theme = THEME_SYSTEM;
    g_autofree char *cfg = g_build_filename(g_get_user_config_dir(), "blufixer", "theme", NULL);
    g_autofree char *saved = NULL;
    if (g_file_get_contents(cfg, &saved, NULL, NULL)) {
        g_strchomp(saved);
        if (g_strcmp0(saved, "light") == 0) current_theme = THEME_LIGHT;
        else if (g_strcmp0(saved, "dark") == 0) current_theme = THEME_DARK;
    }
    AdwStyleManager *sm = adw_style_manager_get_default();
    adw_style_manager_set_color_scheme(sm,
        current_theme == THEME_LIGHT ? ADW_COLOR_SCHEME_FORCE_LIGHT :
        current_theme == THEME_DARK  ? ADW_COLOR_SCHEME_FORCE_DARK :
                                       ADW_COLOR_SCHEME_DEFAULT);
}

void set_theme(ThemeId t) {
    if (current_theme == t) return;
    current_theme = t;
    g_autofree char *dir = g_build_filename(g_get_user_config_dir(), "blufixer", NULL);
    g_mkdir_with_parents(dir, 0755);
    g_autofree char *cfg = g_build_filename(dir, "theme", NULL);
    g_file_set_contents(cfg,
        t == THEME_LIGHT ? "light" :
        t == THEME_DARK  ? "dark"  : "system", -1, NULL);
    AdwStyleManager *sm = adw_style_manager_get_default();
    adw_style_manager_set_color_scheme(sm,
        t == THEME_LIGHT ? ADW_COLOR_SCHEME_FORCE_LIGHT :
        t == THEME_DARK  ? ADW_COLOR_SCHEME_FORCE_DARK :
                           ADW_COLOR_SCHEME_DEFAULT);
}

void on_device_selected(GtkButton *btn, gpointer user_data) {
    const char *vendor = g_object_get_data(G_OBJECT(btn), "vendor");
    const char *product = g_object_get_data(G_OBJECT(btn), "product");
    const char *desc = g_object_get_data(G_OBJECT(btn), "desc");
    const char *manufacturer = g_object_get_data(G_OBJECT(btn), "manufacturer");

    gboolean is_ps_licensed = g_strcmp0(vendor, "12ba") == 0 && g_strcmp0(product, "0030") == 0;
    gboolean is_ps = g_strcmp0(manufacturer, "Sony") == 0 || is_ps_licensed;

    if (is_ps_licensed) {
        desc = "CECHYA-0080";
        manufacturer = "Sony Computer Entertainment America LLC";
    }

    g_strlcpy(app_data.selected_vendor, vendor, sizeof(app_data.selected_vendor));
    g_strlcpy(app_data.selected_product, product, sizeof(app_data.selected_product));
    g_strlcpy(app_data.selected_desc, desc, sizeof(app_data.selected_desc));

    adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_name), desc);
    adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_vendor), manufacturer);

    char id_buffer[32];
    g_snprintf(id_buffer, sizeof(id_buffer), "%s:%s", vendor, product);
    adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_id), id_buffer);

    query_bluetooth_version();
    update_actions_page_state();

    gtk_widget_set_visible(app_data.row_tech_playstation, is_ps);
    if (is_ps) {
        adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_playstation),
            is_ps_licensed ? _("Yes, for PlayStation\u00ae3") : _("Yes"));
    }

    gtk_widget_set_visible(app_data.back_button, TRUE);
    gtk_stack_set_visible_child_name(GTK_STACK(app_data.view_stack), "page_actions");
}

static void on_back_clicked(GtkButton *btn, gpointer user_data) {
    gtk_widget_set_visible(app_data.back_button, FALSE);
    gtk_stack_set_visible_child_name(GTK_STACK(app_data.view_stack), "page_devices");
}

static void on_update_btn_clicked(GtkButton *btn, gpointer user_data) {
    GtkUriLauncher *launcher = gtk_uri_launcher_new("https://github.com/mayrinck/blufixer/releases");
    gtk_uri_launcher_launch(launcher, GTK_WINDOW(app_data.window), NULL, NULL, NULL);
    g_object_unref(launcher);
}

static void on_keyboard_shortcuts(GSimpleAction *action, GVariant *param, gpointer user_data) {
    show_keyboard_shortcuts();
}

static void on_toggle_theme(GSimpleAction *action, GVariant *param, gpointer user_data) {
    set_theme(current_theme == THEME_DARK ? THEME_LIGHT : THEME_DARK);
}

static void on_copy_tech_sheet(GSimpleAction *action, GVariant *param, gpointer user_data) {
    const char *name = adw_action_row_get_subtitle(ADW_ACTION_ROW(app_data.row_tech_name));
    const char *vendor = adw_action_row_get_subtitle(ADW_ACTION_ROW(app_data.row_tech_vendor));
    const char *id = adw_action_row_get_subtitle(ADW_ACTION_ROW(app_data.row_tech_id));
    const char *version = adw_action_row_get_subtitle(ADW_ACTION_ROW(app_data.row_tech_version));
    const char *ps = NULL;
    if (gtk_widget_get_visible(app_data.row_tech_playstation))
        ps = adw_action_row_get_subtitle(ADW_ACTION_ROW(app_data.row_tech_playstation));

    if (!name || strlen(name) == 0) {
        adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay),
            adw_toast_new(_("No technical data to copy")));
        return;
    }

    g_autofree char *all;
    if (ps) {
        all = g_strdup_printf(
            "%s\n%s\n%s\n%s\n%s",
            name, vendor ? vendor : "", id ? id : "", version ? version : "", ps);
    } else {
        all = g_strdup_printf(
            "%s\n%s\n%s\n%s",
            name, vendor ? vendor : "", id ? id : "", version ? version : "");
    }
    gdk_clipboard_set_text(gtk_widget_get_clipboard(app_data.window), all);
    adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay),
        adw_toast_new(_("Technical data copied")));
}

static void on_restart_app(GSimpleAction *action, GVariant *param, gpointer user_data) {
    const char *prgname = g_get_prgname() ? g_get_prgname() : "blufixer";
    const char *argv[] = {prgname, NULL};
    g_spawn_async(NULL, (char **)argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL);
    gtk_window_destroy(GTK_WINDOW(app_data.window));
    g_application_quit(G_APPLICATION(app_data.app));
}

static void on_preferences(GSimpleAction *action, GVariant *param, gpointer user_data) {
    show_preferences_dialog();
}

static gboolean on_key_pressed(GtkEventControllerKey *ctrl, guint keyval, guint keycode, GdkModifierType state, gpointer user_data) {
    if ((keyval == GDK_KEY_F5) && (state & gtk_accelerator_get_default_mod_mask()) == 0) {
        const char *page = gtk_stack_get_visible_child_name(GTK_STACK(app_data.view_stack));
        if (g_strcmp0(page, "page_devices") == 0) {
            on_scan_clicked(NULL, NULL);
            return GDK_EVENT_STOP;
        }
    }
    return GDK_EVENT_PROPAGATE;
}

static void activate(GtkApplication *app, gpointer user_data) {
    app_data.app = app;
    detect_language();
    detect_theme();
    init_custom_styles();
    detect_system_tools();

    GtkIconTheme *icon_theme = gtk_icon_theme_get_for_display(gdk_display_get_default());
    gtk_icon_theme_add_search_path(icon_theme, g_get_current_dir());

    app_data.window = adw_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(app_data.window), "BluFixer");
    gtk_window_set_default_size(GTK_WINDOW(app_data.window), 640, 640);
    gtk_widget_set_size_request(app_data.window, 640, 480);

    GSimpleActionGroup *actions = g_simple_action_group_new();
    const GActionEntry entries[] = {
        { "about", on_about_action, NULL, NULL, NULL, {0} },
        { "donate", on_donate_action, NULL, NULL, NULL, {0} },
        { "show_error_detail", on_show_error_detail, NULL, NULL, NULL, {0} },
        { "restart", on_restart_app, NULL, NULL, NULL, {0} },
        { "preferences", on_preferences, NULL, NULL, NULL, {0} },
        { "keyboard_shortcuts", on_keyboard_shortcuts, NULL, NULL, NULL, {0} },
        { "toggle_theme", on_toggle_theme, NULL, NULL, NULL, {0} },
        { "copy_tech_sheet", on_copy_tech_sheet, NULL, NULL, NULL, {0} },
    };
    g_action_map_add_action_entries(G_ACTION_MAP(actions), entries, G_N_ELEMENTS(entries), NULL);
    gtk_widget_insert_action_group(app_data.window, "win", G_ACTION_GROUP(actions));

    app_data.main_menu = g_menu_new();
    g_menu_append(app_data.main_menu, _("Preferences"), "win.preferences");
    g_menu_append(app_data.main_menu, _("Keyboard Shortcuts"), "win.keyboard_shortcuts");
    g_menu_append(app_data.main_menu, _("About"), "win.about");
    g_menu_append(app_data.main_menu, _("Donate"), "win.donate");

    app_data.toast_overlay = adw_toast_overlay_new();
    adw_application_window_set_content(ADW_APPLICATION_WINDOW(app_data.window), app_data.toast_overlay);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    adw_toast_overlay_set_child(ADW_TOAST_OVERLAY(app_data.toast_overlay), main_box);

    GtkWidget *header = adw_header_bar_new();
    app_data.back_button = gtk_button_new_from_icon_name("go-previous-symbolic");
    gtk_widget_set_visible(app_data.back_button, FALSE);
    g_signal_connect(app_data.back_button, "clicked", G_CALLBACK(on_back_clicked), NULL);
    adw_header_bar_pack_start(ADW_HEADER_BAR(header), app_data.back_button);

    app_data.update_btn = gtk_button_new_from_icon_name("software-update-available-symbolic");
    gtk_widget_set_visible(app_data.update_btn, FALSE);
    gtk_widget_add_css_class(app_data.update_btn, "update-btn");
    g_signal_connect(app_data.update_btn, "clicked", G_CALLBACK(on_update_btn_clicked), NULL);

    adw_header_bar_pack_end(ADW_HEADER_BAR(header), create_menu_button());
    adw_header_bar_pack_end(ADW_HEADER_BAR(header), app_data.update_btn);
    gtk_box_append(GTK_BOX(main_box), header);

    app_data.view_stack = gtk_stack_new();
    gtk_widget_set_vexpand(app_data.view_stack, TRUE);
    gtk_stack_set_transition_type(GTK_STACK(app_data.view_stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(app_data.view_stack), 250);
    gtk_box_append(GTK_BOX(main_box), app_data.view_stack);

    /* --- PAGE 1: DEVICES --- */
    GtkWidget *pg1 = adw_preferences_page_new();
    app_data.devices_group = adw_preferences_group_new();
    adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(app_data.devices_group), _("Device List"));
    adw_preferences_group_set_description(ADW_PREFERENCES_GROUP(app_data.devices_group), _("Select the adapter you want to modify"));

    GtkWidget *btn_scan = gtk_button_new();
    GtkWidget *scan_content = adw_button_content_new();
    adw_button_content_set_icon_name(ADW_BUTTON_CONTENT(scan_content), "view-refresh-symbolic");
    adw_button_content_set_label(ADW_BUTTON_CONTENT(scan_content), _("Scan"));
    gtk_button_set_child(GTK_BUTTON(btn_scan), scan_content);
    gtk_widget_add_css_class(btn_scan, "suggested-action");
    g_signal_connect(btn_scan, "clicked", G_CALLBACK(on_scan_clicked), NULL);

    adw_preferences_group_set_header_suffix(ADW_PREFERENCES_GROUP(app_data.devices_group), btn_scan);
    adw_preferences_page_add(ADW_PREFERENCES_PAGE(pg1), ADW_PREFERENCES_GROUP(app_data.devices_group));
    gtk_stack_add_named(GTK_STACK(app_data.view_stack), pg1, "page_devices");

    /* --- PAGE 2: ACTIONS --- */
    GtkWidget *pg2 = adw_preferences_page_new();

    app_data.status_group = adw_preferences_group_new();
    adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(app_data.status_group), _("Device Tech Sheet"));
    adw_preferences_page_add(ADW_PREFERENCES_PAGE(pg2), ADW_PREFERENCES_GROUP(app_data.status_group));

    app_data.row_tech_name = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(app_data.row_tech_name), _("Device Name"));
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(app_data.row_tech_name), TRUE);
    g_signal_connect(app_data.row_tech_name, "activated", G_CALLBACK(on_info_copy_activated), (gpointer)_("Device Name copied"));
    GtkWidget *copy_icon = gtk_image_new_from_icon_name("edit-copy-symbolic");
    adw_action_row_add_suffix(ADW_ACTION_ROW(app_data.row_tech_name), copy_icon);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.status_group), app_data.row_tech_name);

    app_data.row_tech_vendor = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(app_data.row_tech_vendor), _("Possible Manufacturer"));
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(app_data.row_tech_vendor), TRUE);
    gtk_widget_set_tooltip_text(app_data.row_tech_vendor,
        _("The possible manufacturer name of the board on the device, not the retail brand."));
    g_signal_connect(app_data.row_tech_vendor, "activated", G_CALLBACK(on_info_copy_activated), (gpointer)_("Manufacturer copied"));
    GtkWidget *copy_icon_vendor = gtk_image_new_from_icon_name("edit-copy-symbolic");
    adw_action_row_add_suffix(ADW_ACTION_ROW(app_data.row_tech_vendor), copy_icon_vendor);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.status_group), app_data.row_tech_vendor);

    app_data.row_tech_id = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(app_data.row_tech_id), _("Hardware ID"));
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(app_data.row_tech_id), TRUE);
    g_signal_connect(app_data.row_tech_id, "activated", G_CALLBACK(on_info_copy_activated), (gpointer)_("Hardware ID copied"));
    GtkWidget *copy_icon_id = gtk_image_new_from_icon_name("edit-copy-symbolic");
    adw_action_row_add_suffix(ADW_ACTION_ROW(app_data.row_tech_id), copy_icon_id);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.status_group), app_data.row_tech_id);

    app_data.row_tech_version = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(app_data.row_tech_version), _("Bluetooth Version"));
    adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_version), "\u2014");
    gtk_widget_set_tooltip_text(app_data.row_tech_version,
        _("Bluetooth version is detected via bluetoothctl, btmgmt or hciconfig."));
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(app_data.row_tech_version), FALSE);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.status_group), app_data.row_tech_version);

    app_data.row_tech_playstation = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(app_data.row_tech_playstation), _("Licensed Playstation Accessory"));
    adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_playstation), _("Yes"));
    gtk_widget_set_tooltip_text(app_data.row_tech_playstation,
        _("This device may use Bluetooth technologies licensed by Sony Interactive Entertainment."));
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(app_data.row_tech_playstation), FALSE);
    gtk_widget_set_visible(app_data.row_tech_playstation, FALSE);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.status_group), app_data.row_tech_playstation);

    app_data.fixes_group = adw_preferences_group_new();
    adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(app_data.fixes_group), _("System Fixes"));
    adw_preferences_group_set_description(ADW_PREFERENCES_GROUP(app_data.fixes_group),
        _("Applies configuration file changes or driver/firmware downloads. Some settings are hardware-specific and all can be reverted."));
    adw_preferences_page_add(ADW_PREFERENCES_PAGE(pg2), ADW_PREFERENCES_GROUP(app_data.fixes_group));

    app_data.actions_group = adw_preferences_group_new();
    adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(app_data.actions_group), _("Immediate Actions"));
    adw_preferences_group_set_description(ADW_PREFERENCES_GROUP(app_data.actions_group),
        _("Direct commands applied to hardware or system services. Cannot be reverted (only repeated)."));
    adw_preferences_page_add(ADW_PREFERENCES_PAGE(pg2), ADW_PREFERENCES_GROUP(app_data.actions_group));

    struct { const char *t; const char *sub; GCallback info; GCallback fix; int target_section; } rows[] = {
        {_("CSR Energy Fix"), _("Recommended for CSR and Barrot/Generic dongles. Stabilizes the radio to fix detection and connection loops."), G_CALLBACK(on_info_csr_clicked), G_CALLBACK(run_fix_csr), 0},
        {_("Disable ERTM for Gamepads"), _("Fixes automatic disconnections of modern Bluetooth gamepads."), G_CALLBACK(on_info_ertm_clicked), G_CALLBACK(run_fix_ertm), 0},
        {_("Force Legacy Pairing Mode"), _("Allows old Bluetooth devices to pair with a fixed PIN."), G_CALLBACK(on_info_legacy_clicked), G_CALLBACK(run_fix_legacy), 0},
        {_("Install Realtek Firmware (RTL8761B)"), _("Downloads and injects the missing official driver binaries."), G_CALLBACK(on_info_realtek_clicked), G_CALLBACK(run_fix_realtek), 0},
        {_("Install Broadcom/Cypress Firmware (b43)"), _("Downloads and extracts the missing proprietary firmware for Broadcom chipsets."), G_CALLBACK(on_info_broadcom_clicked), G_CALLBACK(run_fix_broadcom), 0},
        {_("Unblock Antenna"), _("Forces activation of adapters stuck in Airplane Mode."), G_CALLBACK(on_info_rfkill_clicked), G_CALLBACK(run_fix_rfkill), 1},
        {_("Add user permissions"), _("Adds the user to the lp group for Bluetooth D-Bus access."), G_CALLBACK(on_info_perm_clicked), G_CALLBACK(run_fix_perm), 1},
        {_("Clear device cache"), _("Removes all pairing caches to fix connection errors."), G_CALLBACK(on_info_cache_clicked), G_CALLBACK(run_fix_cache), 1},
        {_("Restart Bluetooth Service"), _("Clears caches and buffers by restarting the system service."), G_CALLBACK(on_info_restart_clicked), G_CALLBACK(run_restart_service), 1}
    };

    for(size_t i = 0; i < G_N_ELEMENTS(rows); i++) {
        GtkWidget *row = adw_action_row_new();
        adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), rows[i].t);
        adw_action_row_set_subtitle(ADW_ACTION_ROW(row), rows[i].sub);

        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        GtkWidget *info = gtk_button_new_from_icon_name("dialog-information-symbolic");
        gtk_widget_add_css_class(info, "flat");
        g_signal_connect(info, "clicked", rows[i].info, NULL);

        GtkWidget *sp, *lbl;
        const char *initial_text = (i >= 5) ? _("Run") : _("Apply");
        GtkWidget *fix = create_action_row_button(initial_text, rows[i].fix, &sp, &lbl);

        if (i == 0) { app_data.row_csr = row; app_data.btn_csr = fix; app_data.lbl_csr = lbl; }
        else if (i == 1) { app_data.btn_ertm = fix; app_data.lbl_ertm = lbl; }
        else if (i == 2) { app_data.btn_legacy = fix; app_data.lbl_legacy = lbl; }
        else if (i == 3) { app_data.row_realtek = row; app_data.btn_realtek = fix; app_data.lbl_realtek = lbl; }
        else if (i == 4) { app_data.row_broadcom = row; app_data.btn_broadcom = fix; app_data.lbl_broadcom = lbl; }
        else if (i == 5) { app_data.btn_rfkill = fix; }
        else if (i == 6) { app_data.btn_perm = fix; }
        else if (i == 7) { app_data.btn_cache = fix; }
        else if (i == 8) { app_data.btn_restart = fix; }

        gtk_box_append(GTK_BOX(box), info);
        gtk_box_append(GTK_BOX(box), fix);
        adw_action_row_add_suffix(ADW_ACTION_ROW(row), box);

        if(rows[i].target_section == 0) {
            adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.fixes_group), row);
        } else {
            adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.actions_group), row);
        }
    }

    gtk_stack_add_named(GTK_STACK(app_data.view_stack), pg2, "page_actions");

    scan_bluetooth_devices();

    if (!has_elevation) {
        AdwToast *toast = adw_toast_new(_("No elevation method found (pkexec/sudo). Fixes will not work."));
        adw_toast_set_priority(toast, ADW_TOAST_PRIORITY_HIGH);
        adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay), toast);
    }

    check_for_updates(app_data.update_btn);

    gtk_application_set_accels_for_action(GTK_APPLICATION(app), "win.preferences", (const char *[]){"<Primary>P", NULL});
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), "win.keyboard_shortcuts", (const char *[]){"F1", NULL});
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), "win.toggle_theme", (const char *[]){"F12", NULL});
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), "win.copy_tech_sheet", (const char *[]){"<Primary><Shift>C", NULL});

    GtkEventControllerKey *key_ctrl = GTK_EVENT_CONTROLLER_KEY(gtk_event_controller_key_new());
    g_signal_connect(key_ctrl, "key-pressed", G_CALLBACK(on_key_pressed), NULL);
    gtk_widget_add_controller(app_data.window, GTK_EVENT_CONTROLLER(key_ctrl));

    gtk_window_present(GTK_WINDOW(app_data.window));
}

int main(int argc, char **argv) {
    g_autoptr(AdwApplication) app = adw_application_new("org.renanmayrinck.blufixer", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}
