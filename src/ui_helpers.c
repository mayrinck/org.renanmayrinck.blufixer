#include "blufixer.h"

void init_custom_styles(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css =
        ".badge-brand {"
        "  padding: 2px 6px;"
        "  border-radius: 4px;"
        "  font-weight: bold;"
        "  font-size: 10px;"
        "}"
        ".badge-realtek  { color: #00f;    background-color: rgba(0,   0,   255, 0.12); }"
        ".badge-intel    { color: #127CC1; background-color: rgba(18,  124, 193, 0.12); }"
        ".badge-qualcomm { color: #2e52dd; background-color: rgba(46,  82,  221, 0.12); }"
        ".badge-broadcom { color: #e00;    background-color: rgba(224, 0,   0,   0.12); }"
        ".badge-mediatek { color: #e66000; background-color: rgba(230, 96,  0,   0.12); }"
        ".badge-ralink   { color: #2ECC71; background-color: rgba(46,  204, 113, 0.12); }"
        ".badge-csr      { color: #8E44AD; background-color: rgba(142, 68,  173, 0.12); }"
        ".badge-sony { color: #fff; background-color: #000; }"
        ".badge-generic  { color: #888;    background-color: rgba(136, 136, 136, 0.12); }"
        ".device-title  { font-size: 15px; font-weight: 500; }"
        ".device-sub    { font-size: 13px; color: alpha(@theme_fg_color, 0.55); }"
        "toast button:not(.image-button) {"
        "  background: transparent;"
        "  font-weight: bold;"
        "  padding: 0;"
        "  border: none;"
        "  box-shadow: none;"
        "}"
        "button.update-btn { color: #00CCFE; }"
        "button, drop-down, menu-button { min-height: 34px; }"
        ".shortcut-key {"
        "  font-family: monospace;"
        "  font-size: 12px;"
        "  font-weight: 600;"
        "  padding: 2px 6px;"
        "  border-radius: 4px;"
        "  border: 1px solid alpha(currentColor, 0.25);"
        "  background-color: alpha(currentColor, 0.08);"
        "}";

    gtk_css_provider_load_from_string(provider, css);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);
}

GtkWidget* create_brand_badge(const char *brand) {
    GtkWidget *lbl = gtk_label_new(brand);
    gtk_widget_add_css_class(lbl, "badge-brand");
    static const struct { const char *name; const char *css; } table[] = {
        {"Realtek",             "badge-realtek"},
        {"Intel",               "badge-intel"},
        {"Qualcomm / Atheros",  "badge-qualcomm"},
        {"Broadcom / Cypress",  "badge-broadcom"},
        {"MediaTek",            "badge-mediatek"},
        {"Ralink",              "badge-ralink"},
        {"CSR",                 "badge-csr"},
        {"Sony",                "badge-sony"},
    };
    gboolean found = FALSE;
    for (size_t i = 0; i < G_N_ELEMENTS(table); i++) {
        if (g_strcmp0(brand, table[i].name) == 0) {
            gtk_widget_add_css_class(lbl, table[i].css);
            found = TRUE;
            break;
        }
    }
    if (!found) gtk_widget_add_css_class(lbl, "badge-generic");
    return lbl;
}

GtkWidget* create_action_row_button(const char *label_txt, GCallback cb, GtkWidget **out_spinner, GtkWidget **out_lbl) {
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    *out_spinner = gtk_spinner_new();
    *out_lbl = gtk_label_new(label_txt);

    gtk_box_append(GTK_BOX(box), *out_spinner);
    gtk_box_append(GTK_BOX(box), *out_lbl);
    gtk_button_set_child(GTK_BUTTON(btn), box);

    gtk_widget_set_visible(*out_spinner, FALSE);
    gtk_widget_set_valign(btn, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(btn, 115, -1);

    g_object_set_data(G_OBJECT(btn), "spinner", *out_spinner);
    g_signal_connect(btn, "clicked", cb, NULL);
    return btn;
}

GtkWidget* create_menu_button(void) {
    GtkWidget *menu_btn = gtk_menu_button_new();
    gtk_menu_button_set_icon_name(GTK_MENU_BUTTON(menu_btn), "open-menu-symbolic");
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(menu_btn), G_MENU_MODEL(app_data.main_menu));
    return menu_btn;
}
