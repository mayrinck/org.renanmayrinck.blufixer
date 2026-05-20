#include <gtk/gtk.h>
#include <adwaita.h>
#include <stdlib.h>
#include <string.h>

/* =========================================================================
   1. ESTADO GLOBAL DA APLICAÇÃO E CONTROLE DE LINHAS DINÂMICAS
   ========================================================================= */
static struct {
    GtkWidget *window;
    GtkWidget *toast_overlay;
    GtkWidget *view_stack;
    GtkWidget *devices_group;
    GtkWidget *status_group;   /* Seção da Ficha Técnica do dispositivo ativo */
    GtkWidget *fixes_group;    /* Seção de Correções Reversíveis */
    GtkWidget *actions_group;  /* Seção de Ações Imediatas */
    GtkWidget *back_button;
    GMenu     *main_menu;
    char selected_vendor[8];
    char selected_product[8];
    char selected_desc[256];
    
    GtkWidget *row_csr;
    GtkWidget *row_realtek;
    
    GtkWidget *btn_csr;
    GtkWidget *lbl_csr;
    GtkWidget *btn_ertm;
    GtkWidget *lbl_ertm;
    GtkWidget *btn_realtek;
    GtkWidget *lbl_realtek;

    /* Referências para atualização dinâmica da Ficha Técnica */
    GtkWidget *row_tech_name;
    GtkWidget *row_tech_vendor;
    GtkWidget *row_tech_id;
} app_data;

/* Lista de controle para rastrear e limpar as linhas da interface no scan */
static GList *dynamic_rows = NULL;

/* Detecção multiplataforma: ferramentas de sistema detectadas em tempo real */
static char priv_cmd[16]  = "pkexec";        /* Comando de elevação (pkexec | sudo) */
static char dl_cmd[16]    = "wget";           /* Ferramenta de download (wget | curl) */
static char fw_path[64]   = "/lib/firmware";  /* Diretório de firmware do kernel */

typedef struct {
    char command[1024];
    GtkWidget *button;
    GtkWidget *spinner;
    char success_msg[64];
    char error_msg[64];
    int status_code;
} CommandContext;

/* Pre-declaração necessária para o fluxo de re-escaner */
static void scan_bluetooth_devices(void);
static void detect_system_tools(void);

/* =========================================================================
   2. AÇÕES DO MENU SUPERIOR (ATUALIZADO PARA BLUFIXER)
   ========================================================================= */
static void on_about_action(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    AdwAboutDialog *about = ADW_ABOUT_DIALOG(adw_about_dialog_new());
    adw_about_dialog_set_application_name(about, "BluFixer");
    adw_about_dialog_set_version(about, "1.5.0");
    adw_about_dialog_set_developer_name(about, "Renan Mayrinck");
    adw_about_dialog_set_license_type(about, GTK_LICENSE_MIT_X11);
    adw_about_dialog_set_website(about, "https://www.renanmayrinck.com");
    adw_about_dialog_set_copyright(about, "© 2026 Renan Mayrinck");
    adw_about_dialog_set_application_icon(about, "preferences-system-bluetooth-symbolic");
    adw_dialog_present(ADW_DIALOG(about), app_data.window);
}

static void on_github_action(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    const char *url = "https://github.com/seu-usuario/blufixer";
    GtkUriLauncher *launcher = gtk_uri_launcher_new(url);
    gtk_uri_launcher_launch(launcher, GTK_WINDOW(app_data.window), NULL, NULL, NULL);
    g_object_unref(launcher);
}

static void on_donate_action(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    const char *url = "https://liberapay.com/RenanMayrinck/";
    GtkUriLauncher *launcher = gtk_uri_launcher_new(url);
    gtk_uri_launcher_launch(launcher, GTK_WINDOW(app_data.window), NULL, NULL, NULL);
    g_object_unref(launcher);
}

/* =========================================================================
   3. EXPLICADORES TÉCNICOS BALIZADOS
   ========================================================================= */
static void show_info_dialog(const char *title, const char *body_markup) {
    AdwAlertDialog *dialog = ADW_ALERT_DIALOG(adw_alert_dialog_new(title, NULL));
    GtkWidget *label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), body_markup);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_widget_set_margin_start(label, 20);
    gtk_widget_set_margin_end(label, 20);
    gtk_widget_set_margin_top(label, 12);
    gtk_widget_set_margin_bottom(label, 12);
    adw_alert_dialog_set_extra_child(dialog, label);
    adw_alert_dialog_add_response(dialog, "close", "Entendido");
    adw_dialog_present(ADW_DIALOG(dialog), app_data.window);
}

static void on_info_csr_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>Correção CSR / Barrot (Firmware Clonado)</b>\n\n"
        "<b>O que faz:</b> Muitos adaptadores USB CSR 4.0/5.0 baratos utilizam chips clonados (Barrot) que violam o gerenciamento de energia padrão do Linux. Isso causa falhas de suspensão agressivas e travamentos na pilha de rádio USB.\n\n"
        "Esta correção injeta parâmetros no módulo <span font_family='monospace'>btusb</span> para desativar o 'scatternet', forçar a persistência do firmware na memória e desativar o autodesligamento elétrico.\n\n"
        "<b>Comando de Aplicação:</b>\n"
        "<span font_family='monospace' foreground='%s'>• echo \"options btusb disable_scatternet=1 force_load_firmware=1 enable_autosuspend=0\" &gt; /etc/modprobe.d/btusb.conf</span>\n\n"
        "<b>Comando de Reversão:</b>\n"
        "<span font_family='monospace' foreground='%s'>• rm -f /etc/modprobe.d/btusb.conf</span>", color, color);
    show_info_dialog("Detalhamento Técnico", markup);
    g_free(markup);
}

static void on_info_ertm_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>Desativação do Modo ERTM (Controles de Videogame)</b>\n\n"
        "<b>O que faz:</b> O Enhanced ReTransmission Mode (ERTM) possui incompatibilidades severas com as pilhas bluetooth de controles modernos (Xbox One, Series S/X, PS4/PS5). O controle chega a parear, mas desconecta sozinho após poucos segundos.\n\n"
        "Desativar o ERTM força o Kernel a tratar o fluxo de dados em modo bruto estável eliminando quedas de conexão.\n\n"
        "<b>Comando de Aplicação:</b>\n"
        "<span font_family='monospace' foreground='%s'>• echo \"options bluetooth disable_ertm=1\" &gt; /etc/modprobe.d/bluetooth-ertm.conf</span>\n\n"
        "<b>Comando de Reversão:</b>\n"
        "<span font_family='monospace' foreground='%s'>• rm -f /etc/modprobe.d/bluetooth-ertm.conf</span>", color, color);
    show_info_dialog("Detalhamento Técnico", markup);
    g_free(markup);
}

static void on_info_realtek_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>Injeção de Firmware Proprietário Realtek RTL8761B</b>\n\n"
        "<b>O que faz:</b> Adaptadores USB baseados em chips Realtek mais novos frequentemente não funcionam após a instalação do Linux porque os arquivos binários de firmware (<span font_family='monospace'>.bin</span>) protegidos por direitos autorais são omitidos pelas distribuições.\n\n"
        "Esta correção faz o download do binário oficial diretamente do repositório de linux-firmware estável.\n\n"
        "<b>Comando de Instalação:</b>\n"
        "<span font_family='monospace' foreground='%s'>• wget/curl https://git.kernel.org/.../rtl8761b_fw.bin -O /lib/firmware/rtl_bt/rtl8761b_fw.bin</span>\n\n"
        "<b>Comando de Remoção:</b>\n"
        "<span font_family='monospace' foreground='%s'>• rm -f /lib/firmware/rtl_bt/rtl8761b_fw.bin /lib/firmware/rtl_bt/rtl8761b_config.bin</span>", color, color);
    show_info_dialog("Detalhamento Técnico", markup);
    g_free(markup);
}

static void on_info_rfkill_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>Desbloqueio Elétrico via RFKILL (Modo Avião)</b>\n\n"
        "<b>O que faz:</b> Executa um comando direto de hardware para quebrar travas de software (Soft Blocks) que persistem mesmo quando o usuário tenta ativar o Bluetooth pelas configurações nativas da sua interface de desktop.\n\n"
        "<b>Comando Executado:</b>\n"
        "<span font_family='monospace' foreground='%s'>• rfkill unblock bluetooth &amp;&amp; bluetoothctl power on</span>", color);
    show_info_dialog("Detalhamento Técnico", markup);
    g_free(markup);
}

static void on_info_restart_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>Reinicialização Controlada do Daemon BlueZ</b>\n\n"
        "<b>O que faz:</b> Força o Systemd a derrubar e reerguer a stack inteira de bluetooth do sistema operacional. Isso limpa buffers de memória corrompidos e força o carregamento imediato de quaisquer modificações feitas nos arquivos conf.\n\n"
        "<b>Comando Executado:</b>\n"
        "<span font_family='monospace' foreground='%s'>• systemctl restart bluetooth</span>", color);
    show_info_dialog("Detalhamento Técnico", markup);
    g_free(markup);
}

/* =========================================================================
   4. SISTEMA CONTEXTUAL DE VISIBILIDADE E REVERSÃO (MUDANÇA DINÂMICA)
   ========================================================================= */
static void update_actions_page_state(void) {
    gboolean is_realtek = g_strrstr(app_data.selected_desc, "Realtek") != NULL;
    gtk_widget_set_visible(app_data.row_realtek, is_realtek);

    gboolean is_csr = (g_strrstr(app_data.selected_desc, "CSR") != NULL || g_strrstr(app_data.selected_desc, "Cambridge") != NULL);
    gtk_widget_set_visible(app_data.row_csr, is_csr);

    if (g_file_test("/etc/modprobe.d/btusb.conf", G_FILE_TEST_EXISTS)) {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_csr), "Reverter");
        gtk_widget_remove_css_class(app_data.btn_csr, "suggested-action");
        gtk_widget_add_css_class(app_data.btn_csr, "destructive-action");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_csr), "Aplicar");
        gtk_widget_remove_css_class(app_data.btn_csr, "destructive-action");
        gtk_widget_add_css_class(app_data.btn_csr, "suggested-action");
    }

    if (g_file_test("/etc/modprobe.d/bluetooth-ertm.conf", G_FILE_TEST_EXISTS)) {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_ertm), "Reverter");
        gtk_widget_add_css_class(app_data.btn_ertm, "destructive-action");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_ertm), "Aplicar");
        gtk_widget_remove_css_class(app_data.btn_ertm, "destructive-action");
    }

    g_autofree char *fw_check = g_strdup_printf("%s/rtl_bt/rtl8761b_fw.bin", fw_path);
    if (g_file_test(fw_check, G_FILE_TEST_EXISTS)) {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_realtek), "Remover");
        gtk_widget_add_css_class(app_data.btn_realtek, "destructive-action");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_realtek), "Instalar");
        gtk_widget_remove_css_class(app_data.btn_realtek, "destructive-action");
    }
}

/* =========================================================================
   5. PROCESSAMENTO ASSÍNCRONO
   ========================================================================= */
static gboolean on_command_finished(gpointer user_data) {
    CommandContext *ctx = (CommandContext *)user_data;
    gtk_spinner_stop(GTK_SPINNER(ctx->spinner));
    gtk_widget_set_visible(ctx->spinner, FALSE);
    gtk_widget_set_sensitive(ctx->button, TRUE);
    
    AdwToast *toast = adw_toast_new(ctx->status_code == 0 ? ctx->success_msg : ctx->error_msg);
    adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay), toast);
    
    update_actions_page_state();
    g_free(ctx);
    return G_SOURCE_REMOVE;
}

static gpointer command_thread_func(gpointer user_data) {
    CommandContext *ctx = (CommandContext *)user_data;
    ctx->status_code = system(ctx->command);
    g_idle_add(on_command_finished, ctx);
    return NULL;
}

static void launch_async_action(GtkWidget *button, const char *cmd, const char *success, const char *error) {
    GtkWidget *spinner = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "spinner"));
    gtk_widget_set_visible(spinner, TRUE);
    gtk_spinner_start(GTK_SPINNER(spinner));
    gtk_widget_set_sensitive(button, FALSE);
    CommandContext *ctx = g_new0(CommandContext, 1);
    g_strlcpy(ctx->command, cmd, sizeof(ctx->command));
    g_strlcpy(ctx->success_msg, success, sizeof(ctx->success_msg));
    g_strlcpy(ctx->error_msg, error, sizeof(ctx->error_msg));
    ctx->button = button; ctx->spinner = spinner;
    g_thread_new("bt_worker", command_thread_func, ctx);
}

/* =========================================================================
   6. CALLBACKS DE INTERAÇÃO E RE-ESCANER
   ========================================================================= */
static void on_fix_csr_clicked(GtkButton *b, gpointer d) {
    char cmd[1024];
    if (g_file_test("/etc/modprobe.d/btusb.conf", G_FILE_TEST_EXISTS)) {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'rm -f /etc/modprobe.d/btusb.conf && systemctl restart bluetooth'", priv_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, "Configuração CSR revertida!", "Erro ao reverter.");
    } else {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'echo \"options btusb disable_scatternet=1 force_load_firmware=1 enable_autosuspend=0\" > /etc/modprobe.d/btusb.conf && systemctl restart bluetooth'", priv_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, "Correção CSR aplicada!", "Erro ao aplicar.");
    }
}

static void on_fix_ertm_clicked(GtkButton *b, gpointer d) {
    char cmd[1024];
    if (g_file_test("/etc/modprobe.d/bluetooth-ertm.conf", G_FILE_TEST_EXISTS)) {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'rm -f /etc/modprobe.d/bluetooth-ertm.conf && systemctl restart bluetooth'", priv_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, "ERTM reativado com sucesso!", "Erro ao reverter.");
    } else {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'echo \"options bluetooth disable_ertm=1\" > /etc/modprobe.d/bluetooth-ertm.conf && systemctl restart bluetooth'", priv_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, "ERTM desativado!", "Erro ao desativar.");
    }
}

static void on_fix_realtek_clicked(GtkButton *b, gpointer d) {
    char cmd[1024];
    g_autofree char *fw_check = g_strdup_printf("%s/rtl_bt/rtl8761b_fw.bin", fw_path);

    if (g_file_test(fw_check, G_FILE_TEST_EXISTS)) {
        g_snprintf(cmd, sizeof(cmd),
            "%s bash -c 'rm -f %s/rtl_bt/rtl8761b_fw.bin %s/rtl_bt/rtl8761b_config.bin && modprobe -r btusb && modprobe btusb'",
            priv_cmd, fw_path, fw_path);
        launch_async_action(GTK_WIDGET(b), cmd, "Firmware Realtek removido!", "Erro ao remover.");
    } else {
        const char *url_fw = "https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git/plain/rtl_bt/rtl8761b_fw.bin";
        const char *url_cfg = "https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git/plain/rtl_bt/rtl8761b_config.bin";

        if (g_strcmp0(dl_cmd, "curl") == 0) {
            g_snprintf(cmd, sizeof(cmd),
                "%s bash -c 'mkdir -p %s/rtl_bt && "
                "curl -Lo %s/rtl_bt/rtl8761b_fw.bin %s && "
                "curl -Lo %s/rtl_bt/rtl8761b_config.bin %s && "
                "modprobe -r btusb && modprobe btusb'",
                priv_cmd, fw_path, fw_path, url_fw, fw_path, url_cfg);
        } else {
            g_snprintf(cmd, sizeof(cmd),
                "%s bash -c 'mkdir -p %s/rtl_bt && "
                "wget %s -O %s/rtl_bt/rtl8761b_fw.bin && "
                "wget %s -O %s/rtl_bt/rtl8761b_config.bin && "
                "modprobe -r btusb && modprobe btusb'",
                priv_cmd, fw_path, url_fw, fw_path, url_cfg, fw_path);
        }
        launch_async_action(GTK_WIDGET(b), cmd, "Firmware Realtek instalado!", "Erro no download.");
    }
}

static void on_fix_rfkill_clicked(GtkButton *b, gpointer d) {
    char cmd[1024];
    g_snprintf(cmd, sizeof(cmd), "%s bash -c 'rfkill unblock bluetooth && bluetoothctl power on'", priv_cmd);
    launch_async_action(GTK_WIDGET(b), cmd, "Antena liberada!", "Erro ao execute.");
}

static void on_restart_service_clicked(GtkButton *b, gpointer d) {
    char cmd[1024];
    g_snprintf(cmd, sizeof(cmd), "%s systemctl restart bluetooth", priv_cmd);
    launch_async_action(GTK_WIDGET(b), cmd, "Serviço BlueZ reiniciado!", "Erro ao reiniciar.");
}

static void on_scan_clicked(GtkButton *btn, gpointer user_data) {
    scan_bluetooth_devices();
    adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay), adw_toast_new("Varredura USB concluída!"));
}

static void on_info_copy_activated(AdwActionRow *row, gpointer user_data) {
    const char *text = adw_action_row_get_subtitle(row);
    const char *msg = user_data ? (const char *)user_data : "Copiado!";
    if (text && strlen(text) > 0) {
        gdk_clipboard_set_text(gtk_widget_get_clipboard(GTK_WIDGET(row)), text);
        adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay), adw_toast_new(msg));
    }
}

static void on_device_selected(GtkButton *btn, gpointer user_data) {
    const char *vendor = g_object_get_data(G_OBJECT(btn), "vendor");
    const char *product = g_object_get_data(G_OBJECT(btn), "product");
    const char *desc = g_object_get_data(G_OBJECT(btn), "desc");
    
    g_strlcpy(app_data.selected_vendor, vendor, sizeof(app_data.selected_vendor));
    g_strlcpy(app_data.selected_product, product, sizeof(app_data.selected_product));
    g_strlcpy(app_data.selected_desc, desc, sizeof(app_data.selected_desc));
    
    char manufacturer[64] = "Genérico";
    if (g_strrstr(desc, "Realtek")) strcpy(manufacturer, "Realtek");
    else if (g_strrstr(desc, "CSR") || g_strrstr(desc, "Cambridge")) strcpy(manufacturer, "CSR");
    else if (g_strrstr(desc, "Intel")) strcpy(manufacturer, "Intel");
    
    adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_name), desc);
    adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_vendor), manufacturer);
    
    char id_buffer[32];
    g_snprintf(id_buffer, sizeof(id_buffer), "%s:%s", vendor, product);
    adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_id), id_buffer);
    
    update_actions_page_state();
    
    gtk_widget_set_visible(app_data.back_button, TRUE);
    gtk_stack_set_visible_child_name(GTK_STACK(app_data.view_stack), "page_actions");
}

static void on_back_clicked(GtkButton *btn, gpointer user_data) {
    gtk_widget_set_visible(app_data.back_button, FALSE);
    gtk_stack_set_visible_child_name(GTK_STACK(app_data.view_stack), "page_devices");
}

/* =========================================================================
   7. GERADORES DE ELEMENTOS VISUAIS CUSTOMIZADOS
   ========================================================================= */
static void init_custom_styles(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css = 
        ".badge-brand {"
        "  padding: 2px 6px;"
        "  border-radius: 4px;"
        "  font-weight: bold;"
        "  font-size: 10px;"
        "}"
        ".badge-realtek { color: #1a5fb4; background-color: rgba(26, 95, 180, 0.15); }"
        ".badge-intel   { color: #0c7c91; background-color: rgba(12, 124, 145, 0.15); }"
        ".badge-csr     { color: #e66000; background-color: rgba(230, 96, 0, 0.15); }"
        ".badge-generic { color: #666666; background-color: rgba(102, 102, 102, 0.15); }"
        ".device-title  { font-size: 15px; font-weight: 500; }"
        ".device-sub    { font-size: 13px; color: #666666; }";
    
    gtk_css_provider_load_from_string(provider, css);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);
}

static GtkWidget* create_brand_badge(const char *brand) {
    GtkWidget *lbl = gtk_label_new(brand);
    gtk_widget_add_css_class(lbl, "badge-brand");
    if (g_strcmp0(brand, "Realtek") == 0) gtk_widget_add_css_class(lbl, "badge-realtek");
    else if (g_strcmp0(brand, "Intel") == 0) gtk_widget_add_css_class(lbl, "badge-intel");
    else if (g_strcmp0(brand, "CSR") == 0) gtk_widget_add_css_class(lbl, "badge-csr");
    else gtk_widget_add_css_class(lbl, "badge-generic");
    return lbl;
}

static GtkWidget* create_action_row_button(const char *label_txt, GCallback cb, GtkWidget **out_spinner, GtkWidget **out_lbl) {
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

static GtkWidget* create_menu_button(void) {
    GtkWidget *menu_btn = gtk_menu_button_new();
    gtk_menu_button_set_icon_name(GTK_MENU_BUTTON(menu_btn), "open-menu-symbolic");
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(menu_btn), G_MENU_MODEL(app_data.main_menu));
    return menu_btn;
}

static void scan_bluetooth_devices(void) {
    for (GList *l = dynamic_rows; l != NULL; l = l->next) {
        adw_preferences_group_remove(ADW_PREFERENCES_GROUP(app_data.devices_group), GTK_WIDGET(l->data));
    }
    g_list_free(dynamic_rows);
    dynamic_rows = NULL;

    FILE *fp = popen("lsusb", "r");
    if (!fp) return;
    char line[256];
    int device_count = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (g_strrstr(line, "Bluetooth") || g_strrstr(line, "Wireless") || g_strrstr(line, "CSR")) {
            char vendor[8], product[8];
            char *id_ptr = g_strrstr(line, "ID ");
            if (id_ptr && sscanf(id_ptr, "ID %7[^:]:%7s", vendor, product) == 2) {
                device_count++;
                char *desc = g_strstrip(g_strdup(id_ptr + 13));
                char manufacturer[64] = "Genérico";
                
                if (g_strrstr(desc, "Realtek")) strcpy(manufacturer, "Realtek");
                else if (g_strrstr(desc, "CSR") || g_strrstr(desc, "Cambridge")) strcpy(manufacturer, "CSR");
                else if (g_strrstr(desc, "Intel")) strcpy(manufacturer, "Intel");
                
                GtkWidget *row = gtk_list_box_row_new();
                GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
                gtk_widget_set_margin_start(row_box, 16);
                gtk_widget_set_margin_end(row_box, 16);
                gtk_widget_set_margin_top(row_box, 10);
                gtk_widget_set_margin_bottom(row_box, 10);

                GtkWidget *text_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
                GtkWidget *lbl_title = gtk_label_new(desc);
                gtk_widget_add_css_class(lbl_title, "device-title");
                gtk_widget_set_halign(lbl_title, GTK_ALIGN_START);
                gtk_label_set_ellipsize(GTK_LABEL(lbl_title), PANGO_ELLIPSIZE_END);

                GtkWidget *desc_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
                gtk_widget_set_halign(desc_box, GTK_ALIGN_START);
                gtk_widget_set_valign(desc_box, GTK_ALIGN_CENTER);

                GtkWidget *badge = create_brand_badge(manufacturer);
                GtkWidget *lbl_sub = gtk_label_new(g_strdup_printf("ID de Hardware: %s:%s", vendor, product));
                gtk_widget_add_css_class(lbl_sub, "device-sub");

                gtk_box_append(GTK_BOX(desc_box), badge);
                gtk_box_append(GTK_BOX(desc_box), lbl_sub);
                gtk_box_append(GTK_BOX(text_box), lbl_title);
                gtk_box_append(GTK_BOX(text_box), desc_box);
                
                gtk_widget_set_hexpand(text_box, TRUE);
                gtk_box_append(GTK_BOX(row_box), text_box);

                GtkWidget *btn = gtk_button_new_with_label("Selecionar");
                gtk_widget_set_valign(btn, GTK_ALIGN_CENTER); gtk_widget_add_css_class(btn, "flat");
                g_object_set_data_full(G_OBJECT(btn), "vendor", g_strdup(vendor), g_free);
                g_object_set_data_full(G_OBJECT(btn), "product", g_strdup(product), g_free);
                g_object_set_data_full(G_OBJECT(btn), "desc", g_strdup(desc), g_free);
                g_signal_connect(btn, "clicked", G_CALLBACK(on_device_selected), NULL);
                
                gtk_box_append(GTK_BOX(row_box), btn);
                gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), row_box);
                
                adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.devices_group), row);
                dynamic_rows = g_list_append(dynamic_rows, row);
                g_free(desc);
            }
        }
    }
    pclose(fp);

    if (device_count == 0) {
        GtkWidget *empty_row = adw_action_row_new();
        adw_preferences_row_set_title(ADW_PREFERENCES_ROW(empty_row), "Não há dispositivos bluetooth conectados ou funcionais no momento");
        gtk_widget_set_sensitive(empty_row, FALSE);
        
        GtkWidget *info_icon = gtk_image_new_from_icon_name("dialog-information-symbolic");
        adw_action_row_add_prefix(ADW_ACTION_ROW(empty_row), info_icon);

        adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.devices_group), empty_row);
        dynamic_rows = g_list_append(dynamic_rows, empty_row);
    }
}

/* =========================================================================
   8. DETECÇÃO MULTIPLATAFORMA (pkexec/sudo, wget/curl, diretório firmware)
   ========================================================================= */
static void detect_system_tools(void) {
    g_autofree gchar *path = NULL;

    path = g_find_program_in_path("pkexec");
    if (!path) {
        path = g_find_program_in_path("sudo");
        if (path)
            g_strlcpy(priv_cmd, "sudo", sizeof(priv_cmd));
    }

    path = g_find_program_in_path("wget");
    if (!path) {
        path = g_find_program_in_path("curl");
        if (path)
            g_strlcpy(dl_cmd, "curl", sizeof(dl_cmd));
    }

    if (g_file_test("/usr/lib/firmware", G_FILE_TEST_IS_DIR))
        g_strlcpy(fw_path, "/usr/lib/firmware", sizeof(fw_path));
}

/* =========================================================================
   9. CONSTRUÇÃO DA INTERFACE UNIFICADA E MONOLÍTICA
   ========================================================================= */
static void activate(GtkApplication *app, gpointer user_data) {
    init_custom_styles();
    detect_system_tools();

    app_data.window = adw_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(app_data.window), "BluFixer");
    gtk_window_set_default_size(GTK_WINDOW(app_data.window), 640, 640);
    gtk_widget_set_size_request(app_data.window, 640, 480);

    GSimpleActionGroup *actions = g_simple_action_group_new();
    const GActionEntry entries[] = {
        { "about", on_about_action, NULL, NULL, NULL },
        { "github", on_github_action, NULL, NULL, NULL },
        { "donate", on_donate_action, NULL, NULL, NULL }
    };
    g_action_map_add_action_entries(G_ACTION_MAP(actions), entries, G_N_ELEMENTS(entries), NULL);
    gtk_widget_insert_action_group(app_data.window, "win", G_ACTION_GROUP(actions));
    
    app_data.main_menu = g_menu_new();
    g_menu_append(app_data.main_menu, "Sobre o BluFixer", "win.about");
    g_menu_append(app_data.main_menu, "Repositório GitHub", "win.github");
    g_menu_append(app_data.main_menu, "Doar ao autor", "win.donate");

    app_data.toast_overlay = adw_toast_overlay_new();
    adw_application_window_set_content(ADW_APPLICATION_WINDOW(app_data.window), app_data.toast_overlay);
    
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    adw_toast_overlay_set_child(ADW_TOAST_OVERLAY(app_data.toast_overlay), main_box);

    GtkWidget *header = adw_header_bar_new();
    app_data.back_button = gtk_button_new_from_icon_name("go-previous-symbolic");
    gtk_widget_set_visible(app_data.back_button, FALSE);
    g_signal_connect(app_data.back_button, "clicked", G_CALLBACK(on_back_clicked), NULL);
    adw_header_bar_pack_start(ADW_HEADER_BAR(header), app_data.back_button);
    adw_header_bar_pack_end(ADW_HEADER_BAR(header), create_menu_button());
    gtk_box_append(GTK_BOX(main_box), header);

    app_data.view_stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(app_data.view_stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(app_data.view_stack), 250);
    gtk_box_append(GTK_BOX(main_box), app_data.view_stack);

    /* --- TELA 1: DISPOSITIVOS --- */
    GtkWidget *pg1 = adw_preferences_page_new();
    app_data.devices_group = adw_preferences_group_new();
    adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(app_data.devices_group), "Dispositivos USB Detectados");
    adw_preferences_group_set_description(ADW_PREFERENCES_GROUP(app_data.devices_group), "Selecione o adaptador que deseja modificar.");
    
    GtkWidget *btn_scan = gtk_button_new();
    GtkWidget *scan_content = adw_button_content_new();
    adw_button_content_set_icon_name(ADW_BUTTON_CONTENT(scan_content), "view-refresh-symbolic");
    adw_button_content_set_label(ADW_BUTTON_CONTENT(scan_content), "Escanear");
    gtk_button_set_child(GTK_BUTTON(btn_scan), scan_content);
    gtk_widget_add_css_class(btn_scan, "suggested-action");
    g_signal_connect(btn_scan, "clicked", G_CALLBACK(on_scan_clicked), NULL);
    
    adw_preferences_group_set_header_suffix(ADW_PREFERENCES_GROUP(app_data.devices_group), btn_scan);

    adw_preferences_page_add(ADW_PREFERENCES_PAGE(pg1), ADW_PREFERENCES_GROUP(app_data.devices_group));
    gtk_stack_add_named(GTK_STACK(app_data.view_stack), pg1, "page_devices");

    /* --- TELA 2: AÇÕES E SEÇÕES --- */
    GtkWidget *pg2 = adw_preferences_page_new();
    
    app_data.status_group = adw_preferences_group_new();
    adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(app_data.status_group), "Ficha Técnica do Adaptador");
    adw_preferences_page_add(ADW_PREFERENCES_PAGE(pg2), ADW_PREFERENCES_GROUP(app_data.status_group));

    app_data.row_tech_name = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(app_data.row_tech_name), "Nome do Dispositivo");
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(app_data.row_tech_name), TRUE);
    g_signal_connect(app_data.row_tech_name, "activated", G_CALLBACK(on_info_copy_activated), "Nome completo copiado!");
    GtkWidget *copy_icon = gtk_image_new_from_icon_name("edit-copy-symbolic");
    adw_action_row_add_suffix(ADW_ACTION_ROW(app_data.row_tech_name), copy_icon);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.status_group), app_data.row_tech_name);

    app_data.row_tech_vendor = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(app_data.row_tech_vendor), "Possível fabricante");
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(app_data.row_tech_vendor), TRUE);
    g_signal_connect(app_data.row_tech_vendor, "activated", G_CALLBACK(on_info_copy_activated), "Fabricante copiado!");
    GtkWidget *copy_icon_vendor = gtk_image_new_from_icon_name("edit-copy-symbolic");
    adw_action_row_add_suffix(ADW_ACTION_ROW(app_data.row_tech_vendor), copy_icon_vendor);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.status_group), app_data.row_tech_vendor);

    app_data.row_tech_id = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(app_data.row_tech_id), "Identificador de Hardware (ID)");
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(app_data.row_tech_id), TRUE);
    g_signal_connect(app_data.row_tech_id, "activated", G_CALLBACK(on_info_copy_activated), "ID copiado!");
    GtkWidget *copy_icon_id = gtk_image_new_from_icon_name("edit-copy-symbolic");
    adw_action_row_add_suffix(ADW_ACTION_ROW(app_data.row_tech_id), copy_icon_id);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.status_group), app_data.row_tech_id);

    app_data.fixes_group = adw_preferences_group_new();
    adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(app_data.fixes_group), "Correções de Sistema");
    adw_preferences_group_set_description(ADW_PREFERENCES_GROUP(app_data.fixes_group), "Modificam permanentemente arquivos de configuração ou baixam drivers e firmware, se necessário. Podem ser completamente revertidas");
    adw_preferences_page_add(ADW_PREFERENCES_PAGE(pg2), ADW_PREFERENCES_GROUP(app_data.fixes_group));

    app_data.actions_group = adw_preferences_group_new();
    adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(app_data.actions_group), "Ações Imediatas");
    adw_preferences_group_set_description(ADW_PREFERENCES_GROUP(app_data.actions_group), "Comandos diretos aplicados ao hardware ou serviços. Não podem ser revertidos (apenas repetidos)");
    adw_preferences_page_add(ADW_PREFERENCES_PAGE(pg2), ADW_PREFERENCES_GROUP(app_data.actions_group));

    struct { const char *t; const char *sub; GCallback info; GCallback fix; int target_section; } rows[] = {
        {"Correção de energia (CSR/Barrot)", "Estabiliza o rádio elétrico de dongles genéricos clonados para resolver loops de conexão.", G_CALLBACK(on_info_csr_clicked), G_CALLBACK(on_fix_csr_clicked), 0},
        {"Desativar ERTM para Controles", "Resolve desconexões automáticas de joysticks Bluetooth.", G_CALLBACK(on_info_ertm_clicked), G_CALLBACK(on_fix_ertm_clicked), 0},
        {"Instalar Firmware Realtek (RTL8761B)", "Baixa e injeta os binários oficiais ausentes do driver.", G_CALLBACK(on_info_realtek_clicked), G_CALLBACK(on_fix_realtek_clicked), 0},
        {"Descongestionar antena (RFKILL)", "Força a ativação de antenas presas no 'Modo Avião'.", G_CALLBACK(on_info_rfkill_clicked), G_CALLBACK(on_fix_rfkill_clicked), 1},
        {"Reiniciar Serviço BlueZ", "Limpa caches e buffers reiniciando o serviço do sistema.", G_CALLBACK(on_info_restart_clicked), G_CALLBACK(on_restart_service_clicked), 1}
    };

    for(int i = 0; i < 5; i++) {
        GtkWidget *row = adw_action_row_new();
        adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), rows[i].t);
        adw_action_row_set_subtitle(ADW_ACTION_ROW(row), rows[i].sub);
        
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        GtkWidget *info = gtk_button_new_from_icon_name("dialog-information-symbolic");
        gtk_widget_add_css_class(info, "flat");
        g_signal_connect(info, "clicked", rows[i].info, NULL);
        
        GtkWidget *sp, *lbl;
        const char *initial_text = (i == 4) ? "Executar" : ((i == 3) ? "Executar" : "Aplicar");
        GtkWidget *fix = create_action_row_button(initial_text, rows[i].fix, &sp, &lbl);
        
        if (i == 0) { app_data.row_csr = row; app_data.btn_csr = fix; app_data.lbl_csr = lbl; }
        else if (i == 1) { app_data.btn_ertm = fix; app_data.lbl_ertm = lbl; }
        else if (i == 2) { app_data.row_realtek = row; app_data.btn_realtek = fix; app_data.lbl_realtek = lbl; }
        
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
    gtk_window_present(GTK_WINDOW(app_data.window));
}

int main(int argc, char **argv) {
    g_autoptr(AdwApplication) app = adw_application_new("org.renanmayrinck.blufixer", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}