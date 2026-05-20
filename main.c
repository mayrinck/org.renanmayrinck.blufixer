#include <gtk/gtk.h>
#include <adwaita.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define APP_VERSION "1.5.0"

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
    GtkWidget *row_broadcom;
    
    GtkWidget *btn_csr;
    GtkWidget *lbl_csr;
    GtkWidget *btn_ertm;
    GtkWidget *lbl_ertm;
    GtkWidget *btn_realtek;
    GtkWidget *lbl_realtek;

    GtkWidget *btn_legacy;
    GtkWidget *lbl_legacy;
    GtkWidget *btn_broadcom;
    GtkWidget *lbl_broadcom;
    GtkWidget *btn_rfkill;
    GtkWidget *btn_perm;
    GtkWidget *btn_cache;
    GtkWidget *btn_restart;

    /* Referências para atualização dinâmica da Ficha Técnica */
    GtkWidget *row_tech_name;
    GtkWidget *row_tech_vendor;
    GtkWidget *row_tech_id;
    GtkWidget *row_tech_version;
} app_data;

/* Lista de controle para rastrear e limpar as linhas da interface no scan */
static GList *dynamic_rows = NULL;

/* Linha de loader exibida durante a varredura */
static GtkWidget *scan_loader_row = NULL;

/* Detecção multiplataforma: ferramentas de sistema detectadas em tempo real */
static char priv_cmd[512]  = "pkexec";          /* Comando de elevação (pkexec | sudo -A) */
static char dl_cmd[16]      = "wget";           /* Ferramenta de download (wget | curl) */
static char restart_cmd[64] = "systemctl restart bluetooth";  /* Comando de reinicialização do serviço */
static char stop_cmd[64]    = "systemctl stop bluetooth";     /* Comando de parada do serviço */
static char start_cmd[64]   = "systemctl start bluetooth";    /* Comando de inicio do serviço */
static char fw_path[64]     = "/lib/firmware";  /* Diretório de firmware do kernel */
static gboolean has_elevation = TRUE;            /* FALSE se nenhum método de elevação for detectado */
static gboolean in_flatpak = FALSE;              /* TRUE se executando dentro de Flatpak */

/* Buffer para armazenar detalhes do último erro para exibição na janela de detalhes */
static char last_error_detail[4096] = "";

typedef struct {
    char command[1024];
    GtkWidget *button;
    GtkWidget *spinner;
    char success_msg[64];
    char error_msg[64];
    gboolean (*is_active)(void);
    gboolean expect_active;
    int status_code;
} CommandContext;

/* Pre-declaração necessária para o fluxo de re-escaner */
static void scan_bluetooth_devices(void);
static const char* detect_manufacturer(const char *vendor, const char *product, const char *desc);
static void query_bluetooth_version(void);
static void detect_system_tools(void);

/* =========================================================================
   2. AÇÕES DO MENU SUPERIOR (ATUALIZADO PARA BLUFIXER)
   ========================================================================= */
static void on_about_action(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    AdwAboutDialog *about = ADW_ABOUT_DIALOG(adw_about_dialog_new());
    adw_about_dialog_set_application_name(about, "BluFixer");
    adw_about_dialog_set_version(about, APP_VERSION);
    adw_about_dialog_set_developer_name(about, "Renan Mayrinck");
    adw_about_dialog_set_license_type(about, GTK_LICENSE_MIT_X11);
    adw_about_dialog_set_website(about, "https://www.renanmayrinck.com");
    adw_about_dialog_set_copyright(about, "©2026 Renan Mayrinck");
    adw_about_dialog_set_application_icon(about, "org.renanmayrinck.blufixer");
    adw_dialog_present(ADW_DIALOG(about), app_data.window);
}

static void on_github_action(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    const char *url = "https://github.com/mayrinck/org.renanmayrinck.blufixer";
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

static void on_show_error_detail(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    if (strlen(last_error_detail) == 0) return;

    AdwAlertDialog *dialog = ADW_ALERT_DIALOG(adw_alert_dialog_new("Falha na operação", NULL));

    g_autofree char *valid = g_utf8_make_valid(last_error_detail, -1);
    g_autofree char *escaped = g_markup_escape_text(valid, -1);
    g_autofree char *markup = g_strdup_printf(
        "<b>Comando executado:</b>\n"
        "<tt>%s</tt>\n\n"
        "Verifique se o comando acima foi executado corretamente. "
        "Você pode copiar o texto abaixo para reportar o problema.",
        escaped);

    GtkWidget *label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), markup);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);
    gtk_widget_set_margin_start(label, 12);
    gtk_widget_set_margin_end(label, 12);
    gtk_widget_set_margin_bottom(label, 12);

    adw_alert_dialog_set_extra_child(dialog, label);
    adw_alert_dialog_add_responses(dialog, "close", "Fechar", NULL);
    adw_alert_dialog_set_default_response(dialog, "close");
    adw_alert_dialog_set_close_response(dialog, "close");

    adw_dialog_present(ADW_DIALOG(dialog), GTK_WIDGET(app_data.window));
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
        "<b>Reinicialização do Serviço Bluetooth</b>\n\n"
        "<b>O que faz:</b> Força o serviço do sistema a derrubar e reerguer a stack inteira de bluetooth do sistema operacional. Isso limpa buffers de memória corrompidos e força o carregamento imediato de quaisquer modificações feitas nos arquivos conf.\n\n"
        "<b>Comando Executado:</b>\n"
        "<span font_family='monospace' foreground='%s'>• %s</span>", color, restart_cmd);
    show_info_dialog("Detalhamento Técnico", markup);
    g_free(markup);
}

static void on_info_legacy_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>Forçar Pareamento em Modo Legado</b>\n\n"
        "<b>O que faz:</b> Muitos dispositivos antigos (fones de ouvido baratos, kits viva-voz veiculares, mouses e teclados Bluetooth 2.0) utilizam o método de pareamento legado com PIN fixo (0000 ou 1234). O sistema de Bluetooth moderno bloqueia esse método por padrão por questões de segurança, impedindo o pareamento.\n\n"
        "Esta correção altera o arquivo de configuração do Bluetooth para forçar o uso do método de segurança legado, permitindo que esses dispositivos pareiem normalmente.\n\n"
        "<b>Comando de Aplicação:</b>\n"
        "<span font_family='monospace' foreground='%s'>• sed -i 's/^#\\?[[:space:]]*Security=ssp/Security=legacy/' /etc/bluetooth/main.conf</span>\n\n"
        "<b>Comando de Reversão:</b>\n"
        "<span font_family='monospace' foreground='%s'>• sed -i 's/^Security=legacy/#Security=ssp/' /etc/bluetooth/main.conf</span>\n\n"
        "<b>Nota:</b> Em ambos os casos o serviço bluetooth é reiniciado automaticamente para aplicar as mudanças.", color, color);
    show_info_dialog("Detalhamento Técnico", markup);
    g_free(markup);
}

static void on_info_broadcom_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>Firmware Broadcom / Cypress (b43)</b>\n\n"
        "<b>O que faz:</b> Dispositivos Bluetooth Broadcom / Cypress dependem de firmware proprietário que é removido de distribuições como Debian por questões de licenciamento. Sem estes arquivos, o hardware não inicializa corretamente e pode nem ser detectado pelo sistema.\n\n"
        "Esta correção instala o pacote de firmware <span font_family='monospace'>b43</span> necessário para o funcionamento do chipset Broadcom.\n\n"
        "<b>Comando de Instalação:</b>\n"
        "<span font_family='monospace' foreground='%s'>• [gerenciador de pacotes] install b43-fwcutter e dependências</span>\n\n"
        "<b>Atenção:</b> A instalação requer acesso administrativo. O firmware será extraído e carregado automaticamente.", color);
    show_info_dialog("Detalhamento Técnico", markup);
    g_free(markup);
}

static void on_info_perm_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>Permissões de Grupo para Bluetooth</b>\n\n"
        "<b>O que faz:</b> Em instalações minimalistas, o usuário logado pode não pertencer ao grupo <span font_family='monospace'>lp</span>, responsável por conceder acesso ao socket D-Bus do Bluetooth. Isso faz com que o ícone do Bluetooth apareça na bandeja, mas nenhum dispositivo consiga parear ou conectar.\n\n"
        "Esta ação adiciona o usuário atual ao grupo <span font_family='monospace'>lp</span>, resolvendo o problema de permissão.\n\n"
        "<b>Comando Executado:</b>\n"
        "<span font_family='monospace' foreground='%s'>• usermod -aG lp &lt;usuario_atual&gt;</span>\n\n"
        "<b>Importante:</b> Após a execução, é necessário <b>reiniciar a sessão</b> (logout e login) para que as novas permissões tenham efeito.", color);
    show_info_dialog("Detalhamento Técnico", markup);
    g_free(markup);
}

static void on_info_cache_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>Limpeza do Cache de Dispositivos Bluetooth</b>\n\n"
        "<b>O que faz:</b> O sistema armazena informações de dispositivos pareados anteriormente em <span font_family='monospace'>/var/lib/bluetooth/[MAC]/[dispositivo]/</span>. Se um dispositivo foi resetado ou seu perfil de serviço mudou, esse cache antigo pode causar erros como \"Protocolo não suportado\" ou \"Connection Refused\".\n\n"
        "Esta ação para o serviço Bluetooth, remove todas as pastas de cache de dispositivos e reinicia o serviço, forçando um pareamento limpo do zero.\n\n"
        "<b>Comando Executado:</b>\n"
        "<span font_family='monospace' foreground='%s'>• stop bluetooth</span>\n"
        "<span font_family='monospace' foreground='%s'>• rm -rf /var/lib/bluetooth/*/*</span>\n"
        "<span font_family='monospace' foreground='%s'>• start bluetooth</span>\n\n"
        "<b>Atenção:</b> Todos os dispositivos pareados serão removidos e precisarão ser pareados novamente.", color, color, color);
    show_info_dialog("Detalhamento Técnico", markup);
    g_free(markup);
}

/* =========================================================================
   4. SISTEMA CONTEXTUAL DE VISIBILIDADE E REVERSÃO (MUDANÇA DINÂMICA)
    ========================================================================= */
static gboolean broadcom_fw_is_active(void);
static gboolean tool_available(const char *name);
static void update_actions_page_state(void) {
    const char *mfr = detect_manufacturer(app_data.selected_vendor, app_data.selected_product, app_data.selected_desc);
    gboolean is_realtek = g_strcmp0(mfr, "Realtek") == 0;
    gtk_widget_set_visible(app_data.row_realtek, is_realtek);

    gtk_widget_set_visible(app_data.row_csr, TRUE);

    gboolean is_broadcom = g_strcmp0(mfr, "Broadcom / Cypress") == 0;
    gtk_widget_set_visible(app_data.row_broadcom, is_broadcom);

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
        gtk_widget_remove_css_class(app_data.btn_ertm, "suggested-action");
        gtk_widget_add_css_class(app_data.btn_ertm, "destructive-action");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_ertm), "Aplicar");
        gtk_widget_remove_css_class(app_data.btn_ertm, "destructive-action");
        gtk_widget_add_css_class(app_data.btn_ertm, "suggested-action");
    }

    g_autofree char *fw_check = g_strdup_printf("%s/rtl_bt/rtl8761b_fw.bin", fw_path);
    if (g_file_test(fw_check, G_FILE_TEST_EXISTS)) {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_realtek), "Remover");
        gtk_widget_remove_css_class(app_data.btn_realtek, "suggested-action");
        gtk_widget_add_css_class(app_data.btn_realtek, "destructive-action");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_realtek), "Instalar");
        gtk_widget_remove_css_class(app_data.btn_realtek, "destructive-action");
        gtk_widget_add_css_class(app_data.btn_realtek, "suggested-action");
    }

    if (broadcom_fw_is_active()) {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_broadcom), "Instalado");
        gtk_widget_add_css_class(app_data.btn_broadcom, "flat");
        gtk_widget_remove_css_class(app_data.btn_broadcom, "suggested-action");
        gtk_widget_remove_css_class(app_data.btn_broadcom, "destructive-action");
        gtk_widget_set_sensitive(app_data.btn_broadcom, FALSE);
    } else {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_broadcom), "Instalar");
        gtk_widget_remove_css_class(app_data.btn_broadcom, "flat");
        gtk_widget_add_css_class(app_data.btn_broadcom, "suggested-action");
        gtk_widget_remove_css_class(app_data.btn_broadcom, "destructive-action");
        gtk_widget_set_sensitive(app_data.btn_broadcom, has_elevation);
    }

    g_autofree char *bt_contents = NULL;
    gboolean is_legacy = FALSE;
    if (g_file_get_contents("/etc/bluetooth/main.conf", &bt_contents, NULL, NULL))
        is_legacy = g_strrstr(bt_contents, "Security=legacy") != NULL;

    if (is_legacy) {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_legacy), "Reverter");
        gtk_widget_remove_css_class(app_data.btn_legacy, "suggested-action");
        gtk_widget_add_css_class(app_data.btn_legacy, "destructive-action");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_legacy), "Aplicar");
        gtk_widget_remove_css_class(app_data.btn_legacy, "destructive-action");
        gtk_widget_add_css_class(app_data.btn_legacy, "suggested-action");
    }

    gtk_widget_set_sensitive(app_data.btn_csr, has_elevation);
    gtk_widget_set_sensitive(app_data.btn_ertm, has_elevation);
    gtk_widget_set_sensitive(app_data.btn_realtek, has_elevation);
    gtk_widget_set_sensitive(app_data.btn_legacy, has_elevation);
    gtk_widget_set_sensitive(app_data.btn_rfkill, has_elevation);
    gtk_widget_set_sensitive(app_data.btn_perm, has_elevation);
    gtk_widget_set_sensitive(app_data.btn_cache, has_elevation);
    gtk_widget_set_sensitive(app_data.btn_restart, has_elevation);
}

/* =========================================================================
   5. PROCESSAMENTO ASSÍNCRONO COM VERIFICAÇÃO PÓS-COMANDO
   ========================================================================= */

/* Funções de verificação de estado para cada correção */
static gboolean csr_is_active(void) {
    return g_file_test("/etc/modprobe.d/btusb.conf", G_FILE_TEST_EXISTS);
}
static gboolean ertm_is_active(void) {
    return g_file_test("/etc/modprobe.d/bluetooth-ertm.conf", G_FILE_TEST_EXISTS);
}
static gboolean realtek_is_active(void) {
    g_autofree char *fw_check = g_strdup_printf("%s/rtl_bt/rtl8761b_fw.bin", fw_path);
    return g_file_test(fw_check, G_FILE_TEST_EXISTS);
}
static gboolean legacy_is_active(void) {
    g_autofree char *contents = NULL;
    if (g_file_get_contents("/etc/bluetooth/main.conf", &contents, NULL, NULL))
        return g_strrstr(contents, "Security=legacy") != NULL;
    return FALSE;
}
static gboolean broadcom_fw_is_active(void) {
    g_autofree char *fw_dir = g_strdup_printf("%s/b43", fw_path);
    return g_file_test(fw_dir, G_FILE_TEST_IS_DIR);
}

static gboolean on_command_finished(gpointer user_data) {
    CommandContext *ctx = (CommandContext *)user_data;
    gtk_spinner_stop(GTK_SPINNER(ctx->spinner));
    gtk_widget_set_visible(ctx->spinner, FALSE);
    gtk_widget_set_sensitive(ctx->button, TRUE);

    update_actions_page_state();

    gboolean ok = TRUE;
    if (ctx->is_active)
        ok = ctx->is_active() == ctx->expect_active;
    else
        ok = (ctx->status_code == 0);

    if (ok) {
        adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay),
            adw_toast_new(ctx->success_msg));
    } else {
        g_snprintf(last_error_detail, sizeof(last_error_detail),
            "Comando: %s\n\nCódigo de saída: %d\n\n"
            "Ocorreu um erro durante a execução do comando acima. "
            "Verifique se o serviço Bluetooth está ativo e se você "
            "possui permissões de superusuário.",
            ctx->command, ctx->status_code);

        AdwToast *toast = adw_toast_new(ctx->error_msg);
        adw_toast_set_button_label(toast, "Ver detalhes");
        adw_toast_set_action_name(toast, "win.show_error_detail");
        adw_toast_set_priority(toast, ADW_TOAST_PRIORITY_HIGH);
        adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay), toast);
    }

    g_free(ctx);
    return G_SOURCE_REMOVE;
}

static gpointer command_thread_func(gpointer user_data) {
    CommandContext *ctx = (CommandContext *)user_data;
    ctx->status_code = system(ctx->command);
    g_idle_add(on_command_finished, ctx);
    return NULL;
}

static void launch_async_action(GtkWidget *button, const char *cmd,
    const char *success, const char *error,
    gboolean (*is_active)(void), gboolean expect_active)
{
    GtkWidget *spinner = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "spinner"));
    gtk_widget_set_visible(spinner, TRUE);
    gtk_spinner_start(GTK_SPINNER(spinner));
    gtk_widget_set_sensitive(button, FALSE);
    CommandContext *ctx = g_new0(CommandContext, 1);
    g_strlcpy(ctx->command, cmd, sizeof(ctx->command));
    g_strlcpy(ctx->success_msg, success, sizeof(ctx->success_msg));
    g_strlcpy(ctx->error_msg, error, sizeof(ctx->error_msg));
    ctx->button = button; ctx->spinner = spinner;
    ctx->is_active = is_active;
    ctx->expect_active = expect_active;
    g_thread_new("bt_worker", command_thread_func, ctx);
}

/* =========================================================================
   6. CALLBACKS DE INTERAÇÃO E RE-ESCANER
   ========================================================================= */
static void on_fix_csr_clicked(GtkButton *b, gpointer d) {
    char cmd[1024];
    if (g_file_test("/etc/modprobe.d/btusb.conf", G_FILE_TEST_EXISTS)) {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'rm -f /etc/modprobe.d/btusb.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, "Configuração CSR revertida!", "Erro ao reverter.", csr_is_active, FALSE);
    } else {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'echo \"options btusb disable_scatternet=1 force_load_firmware=1 enable_autosuspend=0\" > /etc/modprobe.d/btusb.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, "Correção CSR aplicada!", "Erro ao aplicar.", csr_is_active, TRUE);
    }
}

static void on_fix_ertm_clicked(GtkButton *b, gpointer d) {
    char cmd[1024];
    if (g_file_test("/etc/modprobe.d/bluetooth-ertm.conf", G_FILE_TEST_EXISTS)) {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'rm -f /etc/modprobe.d/bluetooth-ertm.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, "ERTM reativado com sucesso!", "Erro ao reverter.", ertm_is_active, FALSE);
    } else {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'echo \"options bluetooth disable_ertm=1\" > /etc/modprobe.d/bluetooth-ertm.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, "ERTM desativado!", "Erro ao desativar.", ertm_is_active, TRUE);
    }
}

static void on_fix_realtek_clicked(GtkButton *b, gpointer d) {
    char cmd[1024];
    g_autofree char *fw_check = g_strdup_printf("%s/rtl_bt/rtl8761b_fw.bin", fw_path);

    if (g_file_test(fw_check, G_FILE_TEST_EXISTS)) {
        g_snprintf(cmd, sizeof(cmd),
            "%s bash -c 'rm -f %s/rtl_bt/rtl8761b_fw.bin %s/rtl_bt/rtl8761b_config.bin && modprobe -r btusb && modprobe btusb'",
            priv_cmd, fw_path, fw_path);
        launch_async_action(GTK_WIDGET(b), cmd, "Firmware Realtek removido!", "Erro ao remover.", realtek_is_active, FALSE);
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
        launch_async_action(GTK_WIDGET(b), cmd, "Firmware Realtek instalado!", "Erro no download.", realtek_is_active, TRUE);
    }
}

static void on_fix_legacy_clicked(GtkButton *b, gpointer d) {
    char cmd[1024];
    g_autofree char *contents = NULL;
    gboolean is_legacy = FALSE;

    if (g_file_get_contents("/etc/bluetooth/main.conf", &contents, NULL, NULL))
        is_legacy = g_strrstr(contents, "Security=legacy") != NULL;

    if (is_legacy) {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'sed -i \"s/^Security=legacy/#Security=ssp/\" /etc/bluetooth/main.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, "Pareamento legado desativado!", "Erro ao reverter.", legacy_is_active, FALSE);
    } else {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'sed -i \"/^[[:space:]]*Security=/d\" /etc/bluetooth/main.conf && echo \"Security=legacy\" >> /etc/bluetooth/main.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, "Pareamento legado ativado!", "Erro ao aplicar.", legacy_is_active, TRUE);
    }
}

static void on_fix_broadcom_clicked(GtkButton *b, gpointer d) {
    if (broadcom_fw_is_active()) return;

    char cmd[1024];
    const char *pkg_mgr = NULL;
    const char *pkgs = "b43-fwcutter";

    if (tool_available("dnf")) {
        pkg_mgr = "dnf install -y";
        pkgs = "b43-fwcutter b43-openfwwf";
    } else if (tool_available("apt-get")) {
        pkg_mgr = "apt-get install -y";
        pkgs = "firmware-b43-installer";
    } else if (tool_available("zypper")) {
        pkg_mgr = "zypper install -y";
    } else if (tool_available("pacman")) {
        pkg_mgr = "pacman -S --noconfirm";
    } else {
        adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay),
            adw_toast_new("Gerenciador de pacotes não reconhecido."));
        return;
    }

    g_snprintf(cmd, sizeof(cmd),
        "%s bash -c '%s %s && modprobe -r b43 2>/dev/null; modprobe b43'",
        priv_cmd, pkg_mgr, pkgs);
    launch_async_action(GTK_WIDGET(b), cmd, "Firmware Broadcom instalado!", "Erro ao instalar firmware.", broadcom_fw_is_active, TRUE);
}

static void on_fix_rfkill_clicked(GtkButton *b, gpointer d) {
    char cmd[1024];
    g_snprintf(cmd, sizeof(cmd), "%s bash -c 'rfkill unblock bluetooth && bluetoothctl power on'", priv_cmd);
    launch_async_action(GTK_WIDGET(b), cmd, "Antena liberada!", "Erro ao execute.", NULL, FALSE);
}

static void on_restart_service_clicked(GtkButton *b, gpointer d) {
    char cmd[1024];
    g_snprintf(cmd, sizeof(cmd), "%s %s", priv_cmd, restart_cmd);
    launch_async_action(GTK_WIDGET(b), cmd, "Serviço Bluetooth reiniciado!", "Erro ao reiniciar.", NULL, FALSE);
}

static void on_fix_perm_clicked(GtkButton *b, gpointer d) {
    char cmd[1024];
    const char *user = g_get_user_name();
    g_snprintf(cmd, sizeof(cmd), "%s usermod -aG lp %s", priv_cmd, user);
    launch_async_action(GTK_WIDGET(b), cmd, "Adicionado ao grupo lp! Reinicie a sessão.", "Erro ao adicionar.", NULL, FALSE);
}

static void on_fix_cache_clicked(GtkButton *b, gpointer d) {
    char cmd[1024];
    g_snprintf(cmd, sizeof(cmd), "%s bash -c '%s && rm -rf /var/lib/bluetooth/*/* && %s'", priv_cmd, stop_cmd, start_cmd);
    launch_async_action(GTK_WIDGET(b), cmd, "Cache de dispositivos limpo!", "Erro ao limpar cache.", NULL, FALSE);
}

static gboolean delayed_scan(gpointer user_data) {
    if (scan_loader_row) {
        adw_preferences_group_remove(ADW_PREFERENCES_GROUP(app_data.devices_group), scan_loader_row);
        scan_loader_row = NULL;
    }
    scan_bluetooth_devices();
    return G_SOURCE_REMOVE;
}

static void on_scan_clicked(GtkButton *btn, gpointer user_data) {
    /* Remove linhas existentes */
    for (GList *l = dynamic_rows; l != NULL; l = l->next)
        adw_preferences_group_remove(ADW_PREFERENCES_GROUP(app_data.devices_group), GTK_WIDGET(l->data));
    g_list_free(dynamic_rows);
    dynamic_rows = NULL;

    /* Insere um loader visual */
    GtkWidget *row = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), "Escaneando dispositivos...");
    gtk_widget_set_sensitive(row, FALSE);
    GtkWidget *spinner = gtk_spinner_new();
    gtk_spinner_start(GTK_SPINNER(spinner));
    adw_action_row_add_prefix(ADW_ACTION_ROW(row), spinner);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.devices_group), row);
    scan_loader_row = row;

    g_timeout_add(500, delayed_scan, NULL);
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
    const char *manufacturer = g_object_get_data(G_OBJECT(btn), "manufacturer");
    
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
    
    gtk_widget_set_visible(app_data.back_button, TRUE);
    gtk_stack_set_visible_child_name(GTK_STACK(app_data.view_stack), "page_actions");
}

static void query_bluetooth_version(void) {
    char buf[256];
    const char *ver = NULL;

    const char *btctl_cmd = in_flatpak ? "flatpak-spawn --host bluetoothctl show 2>/dev/null" : "bluetoothctl show 2>/dev/null";
    FILE *fp = popen(btctl_cmd, "r");
    if (fp) {
        while (fgets(buf, sizeof(buf), fp)) {
            if (g_str_has_prefix(buf, "Version:")) {
                ver = buf + 8;
                while (g_ascii_isspace(*ver)) ver++;
                g_strchomp((char *)ver);
                adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_version), ver);
                pclose(fp);
                return;
            }
        }
        pclose(fp);
    }

    const char *hci_cmd = in_flatpak ? "flatpak-spawn --host hciconfig -a 2>/dev/null" : "hciconfig -a 2>/dev/null";
    fp = popen(hci_cmd, "r");
    if (fp) {
        while (fgets(buf, sizeof(buf), fp)) {
            char *hci = strstr(buf, "HCI Version:");
            if (hci) {
                char *v = hci + 12;
                while (g_ascii_isspace(*v)) v++;
                char *end = v;
                while (*end && !g_ascii_isspace(*end) && *end != '(') end++;
                *end = '\0';
                adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_version), v);
                pclose(fp);
                return;
            }
        }
        pclose(fp);
    }

    adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_version), "Desconhecida");
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
        ".badge-realtek  { color: #00f;    background-color: rgba(0,   0,   255, 0.12); }"
        ".badge-intel    { color: #127CC1; background-color: rgba(18,  124, 193, 0.12); }"
        ".badge-qualcomm { color: #2e52dd; background-color: rgba(46,  82,  221, 0.12); }"
        ".badge-broadcom { color: #e00;    background-color: rgba(224, 0,   0,   0.12); }"
        ".badge-mediatek { color: #e66000; background-color: rgba(230, 96,  0,   0.12); }"
        ".badge-ralink   { color: #2ECC71; background-color: rgba(46,  204, 113, 0.12); }"
        ".badge-csr      { color: #8E44AD; background-color: rgba(142, 68,  173, 0.12); }"
        ".badge-generic  { color: #888;    background-color: rgba(136, 136, 136, 0.12); }"
        ".device-title  { font-size: 15px; font-weight: 500; }"
        ".device-sub    { font-size: 13px; color: #666666; }"
        /* Botão de detalhes em toasts de erro (exclui botão fechar ×) */
        "toast button:not(.image-button) {"
        "  background: @error_bg_color;"
        "  color: @error_fg_color;"
        "  border-radius: 6px;"
        "  font-weight: bold;"
        "}";
    
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
    if      (g_strcmp0(brand, "Realtek")           == 0) gtk_widget_add_css_class(lbl, "badge-realtek");
    else if (g_strcmp0(brand, "Intel")             == 0) gtk_widget_add_css_class(lbl, "badge-intel");
    else if (g_strcmp0(brand, "Qualcomm / Atheros") == 0) gtk_widget_add_css_class(lbl, "badge-qualcomm");
    else if (g_strcmp0(brand, "Broadcom / Cypress") == 0) gtk_widget_add_css_class(lbl, "badge-broadcom");
    else if (g_strcmp0(brand, "MediaTek")           == 0) gtk_widget_add_css_class(lbl, "badge-mediatek");
    else if (g_strcmp0(brand, "Ralink")             == 0) gtk_widget_add_css_class(lbl, "badge-ralink");
    else if (g_strcmp0(brand, "CSR")                == 0) gtk_widget_add_css_class(lbl, "badge-csr");
    else                                                    gtk_widget_add_css_class(lbl, "badge-generic");
    return lbl;
}

/* Detecta fabricante a partir do ID do fornecedor (vendor), ID do produto e descrição */
static const char* detect_manufacturer(const char *vendor, const char *product, const char *desc) {
    /* CSR vendor, mas com product 0001 ou "Barrot" no nome → Genérico */
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

    const char *lsusb_cmd = in_flatpak ? "flatpak-spawn --host lsusb" : "lsusb";
    FILE *fp = popen(lsusb_cmd, "r");
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
                const char *manufacturer = detect_manufacturer(vendor, product, desc);
                
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
                g_autofree char *id_label = g_strdup_printf("ID de Hardware: %s:%s", vendor, product);
                GtkWidget *lbl_sub = gtk_label_new(id_label);
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
                g_object_set_data_full(G_OBJECT(btn), "manufacturer", g_strdup(manufacturer), g_free);
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
/* Verifica se um comando está disponível. Em Flatpak, consulta o host. */
static gboolean tool_available(const char *name) {
    if (in_flatpak) {
        g_autofree char *cmd = g_strdup_printf("flatpak-spawn --host which %s >/dev/null 2>&1", name);
        return system(cmd) == 0;
    }
    return g_find_program_in_path(name) != NULL;
}

static void cleanup_temp_files(void) {
    unlink("/tmp/blufixer-askpass.sh");
}

static void detect_system_tools(void) {
    g_autofree gchar *path = NULL;

    in_flatpak = (g_getenv("FLATPAK_ID") != NULL);

    if (in_flatpak) {
        /* Dentro do Flatpak: tudo é detectado e executado via host */
        if (tool_available("pkexec")) {
            g_strlcpy(priv_cmd, "flatpak-spawn --host pkexec", sizeof(priv_cmd));
            has_elevation = TRUE;
        } else {
            has_elevation = FALSE;
        }

        if (tool_available("wget"))
            g_strlcpy(dl_cmd, "wget", sizeof(dl_cmd));
        else if (tool_available("curl"))
            g_strlcpy(dl_cmd, "curl", sizeof(dl_cmd));

        if (tool_available("systemctl")) {
            /* valores padrão já são systemctl */
        } else if (tool_available("service")) {
            g_strlcpy(restart_cmd, "service bluetooth restart", sizeof(restart_cmd));
            g_strlcpy(stop_cmd, "service bluetooth stop", sizeof(stop_cmd));
            g_strlcpy(start_cmd, "service bluetooth start", sizeof(start_cmd));
        }

        /* Caminho do firmware: no Flatpak o path do host é igual */
        if (g_file_test("/usr/lib/firmware", G_FILE_TEST_IS_DIR))
            g_strlcpy(fw_path, "/usr/lib/firmware", sizeof(fw_path));

        return;
    }

    /* --- Fluxo normal (fora do Flatpak) --- */

    /* 1. Tenta pkexec (padrão em todos os desktops Linux modernos) */
    path = g_find_program_in_path("pkexec");
    if (path) {
        g_strlcpy(priv_cmd, "pkexec", sizeof(priv_cmd));
        has_elevation = TRUE;
    } else {
        /* 2. pkexec não encontrado — tenta sudo -A com um askpass gráfico */
        path = g_find_program_in_path("sudo");
        if (path) {
            const char *askpass_tests[] = {"zenity", "/usr/libexec/ssh-askpass",
                                           "/usr/lib/ssh/ssh-askpass", "lxqt-sudo", NULL};
            has_elevation = FALSE;
            for (int i = 0; askpass_tests[i]; i++) {
                g_autofree gchar *ap = g_find_program_in_path(askpass_tests[i]);
                if (!ap) continue;
                if (g_strrstr(ap, "zenity")) {
                    const char *script =
                        "#!/bin/sh\nexec zenity --password --title=\"BluFixer\" "
                        "--text=\"Digite a senha de superusuário para aplicar correções Bluetooth:\"\n";
                    if (g_file_set_contents("/tmp/blufixer-askpass.sh", script, -1, NULL) &&
                        chmod("/tmp/blufixer-askpass.sh", 0755) == 0) {
                        g_snprintf(priv_cmd, sizeof(priv_cmd),
                            "SUDO_ASKPASS=/tmp/blufixer-askpass.sh sudo -A");
                        has_elevation = TRUE;
                        atexit(cleanup_temp_files);
                    }
                } else {
                    g_snprintf(priv_cmd, sizeof(priv_cmd), "SUDO_ASKPASS=%s sudo -A", ap);
                    has_elevation = TRUE;
                }
                if (has_elevation) break;
            }
        } else {
            has_elevation = FALSE;
        }
    }

    path = g_find_program_in_path("wget");
    if (!path) {
        path = g_find_program_in_path("curl");
        if (path)
            g_strlcpy(dl_cmd, "curl", sizeof(dl_cmd));
    }

    if (g_file_test("/usr/lib/firmware", G_FILE_TEST_IS_DIR))
        g_strlcpy(fw_path, "/usr/lib/firmware", sizeof(fw_path));

    path = g_find_program_in_path("systemctl");
    if (!path) {
        path = g_find_program_in_path("service");
        if (path) {
            g_strlcpy(restart_cmd, "service bluetooth restart", sizeof(restart_cmd));
            g_strlcpy(stop_cmd, "service bluetooth stop", sizeof(stop_cmd));
            g_strlcpy(start_cmd, "service bluetooth start", sizeof(start_cmd));
        }
    }
}

/* =========================================================================
   9. CONSTRUÇÃO DA INTERFACE UNIFICADA E MONOLÍTICA
   ========================================================================= */
static void activate(GtkApplication *app, gpointer user_data) {
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
        { "github", on_github_action, NULL, NULL, NULL, {0} },
        { "donate", on_donate_action, NULL, NULL, NULL, {0} },
        { "show_error_detail", on_show_error_detail, NULL, NULL, NULL, {0} }
    };
    g_action_map_add_action_entries(G_ACTION_MAP(actions), entries, G_N_ELEMENTS(entries), NULL);
    gtk_widget_insert_action_group(app_data.window, "win", G_ACTION_GROUP(actions));
    
    app_data.main_menu = g_menu_new();
    g_menu_append(app_data.main_menu, "Sobre", "win.about");
    g_menu_append(app_data.main_menu, "Repositório no GitHub", "win.github");
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
    adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(app_data.devices_group), "Lista de adaptadores");
    adw_preferences_group_set_description(ADW_PREFERENCES_GROUP(app_data.devices_group), "Selecione o adaptador que deseja modificar");
    
    GtkWidget *btn_scan = gtk_button_new();
    GtkWidget *scan_content = adw_button_content_new();
    adw_button_content_set_icon_name(ADW_BUTTON_CONTENT(scan_content), "view-refresh-symbolic");
    adw_button_content_set_label(ADW_BUTTON_CONTENT(scan_content), "Escanear");
    gtk_button_set_child(GTK_BUTTON(btn_scan), scan_content);
    gtk_widget_set_size_request(btn_scan, -1, 34);
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
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(app_data.row_tech_name), "Nome do dispositivo");
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(app_data.row_tech_name), TRUE);
    g_signal_connect(app_data.row_tech_name, "activated", G_CALLBACK(on_info_copy_activated), "Nome completo copiado");
    GtkWidget *copy_icon = gtk_image_new_from_icon_name("edit-copy-symbolic");
    adw_action_row_add_suffix(ADW_ACTION_ROW(app_data.row_tech_name), copy_icon);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.status_group), app_data.row_tech_name);

    app_data.row_tech_vendor = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(app_data.row_tech_vendor), "Possível fabricante");
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(app_data.row_tech_vendor), TRUE);
    gtk_widget_set_tooltip_text(app_data.row_tech_vendor, "O nome do possível fabricante da placa no dispositivo, não representa a marca de venda na embalagem");
    g_signal_connect(app_data.row_tech_vendor, "activated", G_CALLBACK(on_info_copy_activated), "Nome do fabricante copiado");
    GtkWidget *copy_icon_vendor = gtk_image_new_from_icon_name("edit-copy-symbolic");
    adw_action_row_add_suffix(ADW_ACTION_ROW(app_data.row_tech_vendor), copy_icon_vendor);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.status_group), app_data.row_tech_vendor);

    app_data.row_tech_id = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(app_data.row_tech_id), "Identificador de Hardware (ID)");
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(app_data.row_tech_id), TRUE);
    g_signal_connect(app_data.row_tech_id, "activated", G_CALLBACK(on_info_copy_activated), "Hardware ID copiado");
    GtkWidget *copy_icon_id = gtk_image_new_from_icon_name("edit-copy-symbolic");
    adw_action_row_add_suffix(ADW_ACTION_ROW(app_data.row_tech_id), copy_icon_id);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.status_group), app_data.row_tech_id);

    app_data.row_tech_version = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(app_data.row_tech_version), "Versão do Bluetooth");
    adw_action_row_set_subtitle(ADW_ACTION_ROW(app_data.row_tech_version), "—");
    gtk_widget_set_tooltip_text(app_data.row_tech_version, "A versão Bluetooth só é detectada quando o adaptador está ativo e disponível no sistema");
    gtk_list_box_row_set_activatable(GTK_LIST_BOX_ROW(app_data.row_tech_version), FALSE);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(app_data.status_group), app_data.row_tech_version);

    app_data.fixes_group = adw_preferences_group_new();
    adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(app_data.fixes_group), "Correções de Sistema");
    adw_preferences_group_set_description(ADW_PREFERENCES_GROUP(app_data.fixes_group), "Executa modificações de arquivos de configuração ou download de drivers e firmware, se necessário. Em alguns casos as configurações serão específicas para o hardware e todas podem ser revertidas");
    adw_preferences_page_add(ADW_PREFERENCES_PAGE(pg2), ADW_PREFERENCES_GROUP(app_data.fixes_group));

    app_data.actions_group = adw_preferences_group_new();
    adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(app_data.actions_group), "Ações Imediatas");
    adw_preferences_group_set_description(ADW_PREFERENCES_GROUP(app_data.actions_group), "Comandos diretos aplicados ao hardware ou serviços do sistema. Não podem ser revertidos (apenas repetidos)");
    adw_preferences_page_add(ADW_PREFERENCES_PAGE(pg2), ADW_PREFERENCES_GROUP(app_data.actions_group));

    struct { const char *t; const char *sub; GCallback info; GCallback fix; int target_section; } rows[] = {
        {"Correção de energia", "Recomendada para dispositivos CSR e Barrot/Genéricos. Estabiliza o rádio elétrico do dongle para resolver loops de detecção e conexão.", G_CALLBACK(on_info_csr_clicked), G_CALLBACK(on_fix_csr_clicked), 0},
        {"Desativar ERTM para Gamepads", "Resolve desconexões automáticas de gamepads Bluetooth modernos.", G_CALLBACK(on_info_ertm_clicked), G_CALLBACK(on_fix_ertm_clicked), 0},
        {"Forçar pareamento em modo legado", "Permite que dispositivos Bluetooth antigos pareiem com PIN fixo.", G_CALLBACK(on_info_legacy_clicked), G_CALLBACK(on_fix_legacy_clicked), 0},
        {"Instalar Firmware Realtek (RTL8761B)", "Baixa e injeta os binários oficiais ausentes do driver.", G_CALLBACK(on_info_realtek_clicked), G_CALLBACK(on_fix_realtek_clicked), 0},
        {"Instalar Firmware Broadcom/Cypress (b43)", "Baixa e extrai o firmware proprietário ausente para chipsets Broadcom.", G_CALLBACK(on_info_broadcom_clicked), G_CALLBACK(on_fix_broadcom_clicked), 0},
        {"Descongestionar", "Força a ativação de adaptadores presos no 'Modo Avião'.", G_CALLBACK(on_info_rfkill_clicked), G_CALLBACK(on_fix_rfkill_clicked), 1},
        {"Adicionar permissões ao usuário atual", "Adiciona o usuário ao grupo lp para acesso ao D-Bus do Bluetooth.", G_CALLBACK(on_info_perm_clicked), G_CALLBACK(on_fix_perm_clicked), 1},
        {"Limpar o cache de dispositivos", "Remove o cache de pareamento de todos os dispositivos para resolver erros de conexão.", G_CALLBACK(on_info_cache_clicked), G_CALLBACK(on_fix_cache_clicked), 1},
        {"Reiniciar Serviço Bluetooth", "Limpa caches e buffers reiniciando o serviço do sistema.", G_CALLBACK(on_info_restart_clicked), G_CALLBACK(on_restart_service_clicked), 1}
    };

    for(int i = 0; i < 9; i++) {
        GtkWidget *row = adw_action_row_new();
        adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), rows[i].t);
        adw_action_row_set_subtitle(ADW_ACTION_ROW(row), rows[i].sub);
        
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        GtkWidget *info = gtk_button_new_from_icon_name("dialog-information-symbolic");
        gtk_widget_add_css_class(info, "flat");
        gtk_widget_set_size_request(info, -1, 34);
        g_signal_connect(info, "clicked", rows[i].info, NULL);
        
        GtkWidget *sp, *lbl;
        const char *initial_text = (i >= 5) ? "Executar" : "Aplicar";
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
        AdwToast *toast = adw_toast_new("Nenhum método de elevação encontrado (pkexec/sudo). As correções não funcionarão.");
        adw_toast_set_priority(toast, ADW_TOAST_PRIORITY_HIGH);
        adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay), toast);
    }

    gtk_window_present(GTK_WINDOW(app_data.window));
}

int main(int argc, char **argv) {
    g_autoptr(AdwApplication) app = adw_application_new("org.renanmayrinck.blufixer", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}