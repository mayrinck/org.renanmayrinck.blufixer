#include "blufixer.h"
#include <stdarg.h>

void show_info_dialog(const char *body_markup) {
    AdwAlertDialog *dialog = ADW_ALERT_DIALOG(adw_alert_dialog_new(NULL, NULL));
    GtkWidget *label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), body_markup);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_widget_set_margin_start(label, 20);
    gtk_widget_set_margin_end(label, 20);
    gtk_widget_set_margin_top(label, 12);
    gtk_widget_set_margin_bottom(label, 12);
    adw_alert_dialog_set_extra_child(dialog, label);
    adw_alert_dialog_add_response(dialog, "close", _("Got it"));
    adw_dialog_present(ADW_DIALOG(dialog), app_data.window);
}

const char* dialog_color(void) {
    return adw_style_manager_get_dark(adw_style_manager_get_default()) ? "#00ffff" : "#00008b";
}

void show_info_markup(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char *markup = g_strdup_vprintf(fmt, args);
    va_end(args);
    show_info_dialog(markup);
    g_free(markup);
}

void on_info_copy_activated(AdwActionRow *row, gpointer user_data) {
    const char *text = adw_action_row_get_subtitle(row);
    const char *msg = user_data ? (const char *)user_data : _("Copied!");
    if (text && strlen(text) > 0) {
        gdk_clipboard_set_text(gtk_widget_get_clipboard(GTK_WIDGET(row)), text);
        adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay), adw_toast_new(msg));
    }
}

void on_info_csr_clicked(GtkButton *btn, gpointer user_data) {
    const char *c = dialog_color();
    show_info_markup(
        "<b>%s</b>\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>",
        info5("CSR Power Fix (Connection/Reconnection)",
              "Corre\u00e7\u00e3o de Energia CSR (Conex\u00e3o/Reconex\u00e3o)",
              "Correcci\u00f3n de Energ\u00eda CSR (Conexi\u00f3n/Reconexi\u00f3n)",
              "\u0418\u0441\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0435 \u044d\u043d\u0435\u0440\u0433\u043e\u043f\u043e\u0442\u0440\u0435\u0431\u043b\u0435\u043d\u0438\u044f CSR (\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435/\u041f\u0435\u0440\u0435\u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435)",
              "CSR\u7535\u6e90\u4fee\u590d\uff08\u8fde\u63a5/\u91cd\u65b0\u8fde\u63a5\uff09"),
        info5("Bluetooth devices with CSR (Cambridge Silicon Radio) chipsets have a low-power mode that, after initial pairing, keeps the BLE profile active. This prevents new profiles (such as A2DP for audio) from being negotiated correctly, resulting in:",
            "Dispositivos Bluetooth com chipset CSR (Cambridge Silicon Radio) possuem um modo de baixo consumo que, ap\u00f3s o pareamento inicial, mant\u00e9m o perfil de baixa energia (BLE) ativo. Isso impede que novos perfis (como A2DP para \u00e1udio) sejam negociados corretamente, resultando em:",
            "Los dispositivos Bluetooth con chipsets CSR (Cambridge Silicon Radio) tienen un modo de bajo consumo que, tras el emparejamiento inicial, mantiene el perfil BLE activo. Esto impide que nuevos perfiles (como A2DP para audio) se negocien correctamente, resultando en:",
            "\u0423\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0430 Bluetooth \u0441 \u0447\u0438\u043f\u0441\u0435\u0442\u0430\u043c\u0438 CSR \u0438\u043c\u0435\u044e\u0442 \u0440\u0435\u0436\u0438\u043c \u043d\u0438\u0437\u043a\u043e\u0433\u043e \u044d\u043d\u0435\u0440\u0433\u043e\u043f\u043e\u0442\u0440\u0435\u0431\u043b\u0435\u043d\u0438\u044f, \u043a\u043e\u0442\u043e\u0440\u044b\u0439 \u043f\u043e\u0441\u043b\u0435 \u043d\u0430\u0447\u0430\u043b\u044c\u043d\u043e\u0433\u043e \u0441\u043e\u043f\u0440\u044f\u0436\u0435\u043d\u0438\u044f \u043f\u043e\u0434\u0434\u0435\u0440\u0436\u0438\u0432\u0430\u0435\u0442 \u043f\u0440\u043e\u0444\u0438\u043b\u044c BLE \u0430\u043a\u0442\u0438\u0432\u043d\u044b\u043c, \u0447\u0442\u043e \u043f\u0440\u0435\u043f\u044f\u0442\u0441\u0442\u0432\u0443\u0435\u0442 \u043f\u0440\u0430\u0432\u0438\u043b\u044c\u043d\u043e\u0439 \u043d\u0435\u0433\u043e\u0446\u0438\u0430\u0446\u0438\u0438 \u043d\u043e\u0432\u044b\u0445 \u043f\u0440\u043e\u0444\u0438\u043b\u0435\u0439 (\u043d\u0430\u043f\u0440\u0438\u043c\u0435\u0440, A2DP \u0434\u043b\u044f \u0430\u0443\u0434\u0438\u043e), \u0447\u0442\u043e \u043f\u0440\u0438\u0432\u043e\u0434\u0438\u0442 \u043a:",
            "\u5177\u6709CSR\u82af\u7247\u7ec4\u7684\u84dd\u7259\u8bbe\u5907\u5177\u6709\u4f4e\u529f\u8017\u6a21\u5f0f\uff0c\u8be5\u6a21\u5f0f\u5728\u521d\u6b21\u914d\u5bf9\u540e\u4fdd\u6301BLE\u914d\u7f6e\u6587\u4ef6\u4e3b\u52a8\u3002\u8fd9\u4f1a\u963b\u6b62\u65b0\u914d\u7f6e\u6587\u4ef6\uff08\u5982A2DP\u97f3\u9891\uff09\u88ab\u6b63\u786e\u534f\u5546\uff0c\u5bfc\u81f4:"),
        c, info5("\u2022 No audio after reconnecting", "\u2022 Sem \u00e1udio ap\u00f3s reconectar", "\u2022 Sin audio despu\u00e9s de reconectar", "\u2022 \u041d\u0435\u0442 \u0437\u0432\u0443\u043a\u0430 \u043f\u043e\u0441\u043b\u0435 \u043f\u0435\u0440\u0435\u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f", "\u2022 \u91cd\u65b0\u8fde\u63a5\u540e\u65e0\u97f3\u9891"),
        c, info5("\u2022 Device shows \"Connected\" but no service", "\u2022 Dispositivo mostra \"Conectado\" mas sem servi\u00e7o", "\u2022 Dispositivo muestra \"Conectado\" pero sin servicio", "\u2022 \u0423\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u043e \u043f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0435\u0442 \"\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u043e\", \u043d\u043e \u0431\u0435\u0437 \u0443\u0441\u043b\u0443\u0433\u0438", "\u2022 \u8bbe\u5907\u663e\u793a\u5df2\u8fde\u63a5\u4f46\u65e0\u670d\u52a1"),
        c, info5("\u2022 Connection drops after seconds", "\u2022 Conex\u00e3o cai ap\u00f3s segundos", "\u2022 La conexi\u00f3n cae despu\u00e9s de segundos", "\u2022 \u0421\u043e\u0435\u0434\u0438\u043d\u0435\u043d\u0438\u0435 \u0440\u0430\u0437\u0440\u044b\u0432\u0430\u0435\u0442\u0441\u044f \u0447\u0435\u0440\u0435\u0437 \u0441\u0435\u043a\u0443\u043d\u0434\u044b", "\u2022 \u8fde\u63a5\u5728\u51e0\u79d2\u540e\u65ad\u5f00"),
        info5("This fix disables the CSR driver's power-saving mode via the btusb.enable_autosuspend=0 parameter, forcing the chip to keep all profiles active.",
            "Esta corre\u00e7\u00e3o desabilita o modo de economia de energia do driver CSR via par\u00e2metro btusb.enable_autosuspend=0, for\u00e7ando o chip a manter todos os perfis ativos.",
            "Esta correcci\u00f3n deshabilita el modo de ahorro de energ\u00eda del controlador CSR mediante el par\u00e1metro btusb.enable_autosuspend=0, forzando al chip a mantener todos los perfiles activos.",
            "\u042d\u0442\u043e \u0438\u0441\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0435 \u043e\u0442\u043a\u043b\u044e\u0447\u0430\u0435\u0442 \u0440\u0435\u0436\u0438\u043c \u044d\u043d\u0435\u0440\u0433\u043e\u0441\u0431\u0435\u0440\u0435\u0436\u0435\u043d\u0438\u044f \u0434\u0440\u0430\u0439\u0432\u0435\u0440\u0430 CSR \u0447\u0435\u0440\u0435\u0437 \u043f\u0430\u0440\u0430\u043c\u0435\u0442\u0440 btusb.enable_autosuspend=0, \u0437\u0430\u0441\u0442\u0430\u0432\u043b\u044f\u044f \u0447\u0438\u043f \u043f\u043e\u0434\u0434\u0435\u0440\u0436\u0438\u0432\u0430\u0442\u044c \u0432\u0441\u0435 \u043f\u0440\u043e\u0444\u0438\u043b\u0438 \u0430\u043a\u0442\u0438\u0432\u043d\u044b\u043c\u0438.",
            "\u6b64\u4fee\u590d\u901a\u8fc7 btusb.enable_autosuspend=0 \u53c2\u6570\u7981\u7528CSR\u9a71\u52a8\u7a0b\u5e8f\u7684\u8282\u80fd\u6a21\u5f0f\uff0c\u5f3a\u5236\u82af\u7247\u4fdd\u6301\u6240\u6709\u914d\u7f6e\u6587\u4ef6\u5904\u4e8e\u6d3b\u8dc3\u72b6\u6001\u3002"),
        info5("Command executed:", "Comando Executado:", "Comando ejecutado:", "\u0412\u044b\u043f\u043e\u043b\u043d\u044f\u0435\u043c\u0430\u044f \u043a\u043e\u043c\u0430\u043d\u0434\u0430:", "\u6267\u884c\u7684\u547d\u4ee4:"),
        c, "\u2022 echo options btusb enable_autosuspend=0 > /etc/modprobe.d/btusb.conf",
        info5("Technical details:", "Detalhes T\u00e9cnicos:", "Detalles t\u00e9cnicos:", "\u0422\u0435\u0445\u043d\u0438\u0447\u0435\u0441\u043a\u0438\u0435 \u0434\u0435\u0442\u0430\u043b\u0438:", "\u6280\u672f\u8be6\u60c5:"),
        c, info5("The enable_autosuspend parameter controls whether the btusb driver allows the hardware to enter D3 (suspended) state when there is no traffic. CSR Bluetooth chips are particularly affected because their firmware does not correctly renegotiate profiles when leaving the suspended state.",
            "O par\u00e2metro enable_autosuspend controla se o driver btusb permite que o hardware entre no estado D3 (suspenso) quando n\u00e3o h\u00e1 tr\u00e1fego. CSR Bluetooth chips s\u00e3o particularmente afetados porque seu firmware n\u00e3o renegocia corretamente os perfis ao sair do estado de suspens\u00e3o.",
            "El par\u00e1metro enable_autosuspend controla si el controlador btusb permite que el hardware entre en estado D3 (suspendido) cuando no hay tr\u00e1fico. Los chips Bluetooth CSR se ven particularmente afectados porque su firmware no renegocia correctamente los perfiles al salir del estado suspendido.",
            "\u041f\u0430\u0440\u0430\u043c\u0435\u0442\u0440 enable_autosuspend \u0443\u043f\u0440\u0430\u0432\u043b\u044f\u0435\u0442, \u0440\u0430\u0437\u0440\u0435\u0448\u0430\u0435\u0442 \u043b\u0438 \u0434\u0440\u0430\u0439\u0432\u0435\u0440 btusb \u043e\u0431\u043e\u0440\u0443\u0434\u043e\u0432\u0430\u043d\u0438\u044e \u0432\u0445\u043e\u0434\u0438\u0442\u044c \u0432 \u0441\u043e\u0441\u0442\u043e\u044f\u043d\u0438\u0435 D3 (\u043f\u0440\u0438\u043e\u0441\u0442\u0430\u043d\u043e\u0432\u043a\u0430) \u043f\u0440\u0438 \u043e\u0442\u0441\u0443\u0442\u0441\u0442\u0432\u0438\u0438 \u0442\u0440\u0430\u0444\u0438\u043a\u0430. \u0427\u0438\u043f\u044b CSR Bluetooth \u043e\u0441\u043e\u0431\u0435\u043d\u043d\u043e \u0443\u044f\u0437\u0432\u0438\u043c\u044b, \u043f\u043e\u0442\u043e\u043c\u0443 \u0447\u0442\u043e \u0438\u0445 \u043f\u0440\u043e\u0448\u0438\u0432\u043a\u0430 \u043d\u0435\u043a\u043e\u0440\u0440\u0435\u043a\u0442\u043d\u043e \u043f\u0435\u0440\u0435\u043d\u0430\u0441\u0442\u0440\u0430\u0438\u0432\u0430\u0435\u0442 \u043f\u0440\u043e\u0444\u0438\u043b\u0438 \u043f\u0440\u0438 \u0432\u044b\u0445\u043e\u0434\u0435 \u0438\u0437 \u043f\u0440\u0438\u043e\u0441\u0442\u0430\u043d\u043e\u0432\u043b\u0435\u043d\u043d\u043e\u0433\u043e \u0441\u043e\u0441\u0442\u043e\u044f\u043d\u0438\u044f.",
            "enable_autosuspend\u53c2\u6570\u63a7\u5236btusb\u9a71\u52a8\u7a0b\u5e8f\u662f\u5426\u5141\u8bb8\u786c\u4ef6\u5728\u65e0\u6d41\u91cf\u65f6\u8fdb\u5165D3\uff08\u6302\u8d77\uff09\u72b6\u6001\u3002CSR\u84dd\u7259\u82af\u7247\u53d7\u5f71\u54cd\u7279\u522b\u5927\uff0c\u56e0\u4e3a\u5176\u56fa\u4ef6\u5728\u79bb\u5f00\u6302\u8d77\u72b6\u6001\u65f6\u65e0\u6cd5\u6b63\u786e\u91cd\u65b0\u534f\u5546\u914d\u7f6e\u6587\u4ef6\u3002"));
}
void on_info_ertm_clicked(GtkButton *btn, gpointer user_data) {
    const char *c = dialog_color();
    show_info_markup(
        "<b>%s</b>\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s",
        info5("ERTM Fix (Enhanced Re-Transmission Mode)",
              "Correção ERTM (Modo de Retransmissão Avançada)",
              "Corrección ERTM (Modo de Retransmisión Mejorada)",
              "Исправление ERTM (Улучшенный режим повторной передачи)",
              "ERTM修复（增强重传模式）"),
        info5("ERTM is a Bluetooth protocol feature that ensures reliable data delivery via retransmission. In some chipsets, especially Intel and Broadcom, the ERTM implementation is problematic and causes:",
            "O ERTM \u00e9 um recurso do protocolo Bluetooth que garante entrega confi\u00e1vel de dados via retransmiss\u00e3o. Em alguns chipsets, especialmente Intel e Broadcom, a implementa\u00e7\u00e3o do ERTM \u00e9 problem\u00e1tica e causa:",
            "ERTM es una caracter\u00edstica del protocolo Bluetooth que garantiza la entrega confiable de datos mediante retransmisi\u00f3n. En algunos conjuntos de chips, especialmente Intel y Broadcom, la implementaci\u00f3n de ERTM es problem\u00e1tica y causa:",
            "ERTM \u2014 \u044d\u0442\u043e \u0444\u0443\u043d\u043a\u0446\u0438\u044f \u043f\u0440\u043e\u0442\u043e\u043a\u043e\u043b\u0430 Bluetooth, \u043e\u0431\u0435\u0441\u043f\u0435\u0447\u0438\u0432\u0430\u044e\u0449\u0430\u044f \u043d\u0430\u0434\u0451\u0436\u043d\u0443\u044e \u0434\u043e\u0441\u0442\u0430\u0432\u043a\u0443 \u0434\u0430\u043d\u043d\u044b\u0445 \u0447\u0435\u0440\u0435\u0437 \u043f\u043e\u0432\u0442\u043e\u0440\u043d\u0443\u044e \u043f\u0435\u0440\u0435\u0434\u0430\u0447\u0443. \u0412 \u043d\u0435\u043a\u043e\u0442\u043e\u0440\u044b\u0445 \u0447\u0438\u043f\u0441\u0435\u0442\u0430\u0445, \u043e\u0441\u043e\u0431\u0435\u043d\u043d\u043e Intel \u0438 Broadcom, \u0440\u0435\u0430\u043b\u0438\u0437\u0430\u0446\u0438\u044f ERTM \u043f\u0440\u043e\u0431\u043b\u0435\u043c\u0430\u0442\u0438\u0447\u043d\u0430 \u0438 \u043f\u0440\u0438\u0432\u043e\u0434\u0438\u0442 \u043a:",
            "ERTM\u662f\u84dd\u7259\u534f\u8bae\u7684\u4e00\u9879\u529f\u80fd\uff0c\u901a\u8fc7\u91cd\u4f20\u786e\u4fdd\u53ef\u9760\u7684\u6570\u636e\u4f20\u9012\u3002\u5728\u67d0\u4e9b\u82af\u7247\u7ec4\u4e2d\uff0c\u5c24\u5176\u662fIntel\u548cBroadcom\uff0cERTM\u7684\u5b9e\u73b0\u5b58\u5728\u95ee\u9898\u5e76\u5bfc\u81f4:"),
        c, info5("\u2022 Failure to connect audio devices (A2DP)", "\u2022 Falha ao conectar dispositivos de \u00e1udio (A2DP)", "\u2022 Fallo al conectar dispositivos de audio (A2DP)", "\u2022 \u041d\u0435\u0443\u0434\u0430\u0447\u0430 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f \u0430\u0443\u0434\u0438\u043e\u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432 (A2DP)", "\u2022 \u65e0\u6cd5\u8fde\u63a5\u97f3\u9891\u8bbe\u5907 (A2DP)"),
        c, info5("\u2022 Connection drops when playing media", "\u2022 Queda de conex\u00e3o ao reproduzir m\u00eddia", "\u2022 La conexi\u00f3n cae al reproducir contenido multimedia", "\u2022 \u0421\u043e\u0435\u0434\u0438\u043d\u0435\u043d\u0438\u0435 \u0440\u0430\u0437\u0440\u044b\u0432\u0430\u0435\u0442\u0441\u044f \u043f\u0440\u0438 \u0432\u043e\u0441\u043f\u0440\u043e\u0438\u0437\u0432\u0435\u0434\u0435\u043d\u0438\u0438 \u043c\u0435\u0434\u0438\u0430", "\u2022 \u64ad\u653e\u5a92\u4f53\u65f6\u8fde\u63a5\u65ad\u5f00"),
        c, info5("\u2022 \"Connection Reset by Peer\" error in syslog", "\u2022 Erro \"Connection Reset by Peer\" no syslog", "\u2022 Error \"Connection Reset by Peer\" en syslog", "\u2022 \u041e\u0448\u0438\u0431\u043a\u0430 \"Connection Reset by Peer\" \u0432 syslog", "\u2022 syslog\u4e2d\u7684\u201cConnection Reset by Peer\u201d\u9519\u8bef"),
        info5("This fix disables ERTM globally in the Bluetooth driver, forcing the use of basic L2CAP mode which is more compatible.",
            "Esta corre\u00e7\u00e3o desabilita o ERTM globalmente no driver Bluetooth, for\u00e7ando o uso do modo b\u00e1sico L2CAP que \u00e9 mais compat\u00edvel.",
            "Esta correcci\u00f3n deshabilita ERTM globalmente en el controlador Bluetooth, forzando el uso del modo L2CAP b\u00e1sico que es m\u00e1s compatible.",
            "\u042d\u0442\u043e \u0438\u0441\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0435 \u043e\u0442\u043a\u043b\u044e\u0447\u0430\u0435\u0442 ERTM \u0433\u043b\u043e\u0431\u0430\u043b\u044c\u043d\u043e \u0432 \u0434\u0440\u0430\u0439\u0432\u0435\u0440\u0435 Bluetooth, \u0437\u0430\u0441\u0442\u0430\u0432\u043b\u044f\u044f \u0438\u0441\u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u044c \u0431\u0430\u0437\u043e\u0432\u044b\u0439 \u0440\u0435\u0436\u0438\u043c L2CAP, \u043a\u043e\u0442\u043e\u0440\u044b\u0439 \u0431\u043e\u043b\u0435\u0435 \u0441\u043e\u0432\u043c\u0435\u0441\u0442\u0438\u043c.",
            "\u6b64\u4fee\u590d\u5168\u5c40\u7981\u7528\u84dd\u7259\u9a71\u52a8\u7a0b\u5e8f\u4e2d\u7684ERTM\uff0c\u5f3a\u5236\u4f7f\u7528\u66f4\u517c\u5bb9\u7684\u57fa\u672cL2CAP\u6a21\u5f0f\u3002"),
        info5("Command executed:", "Comando Executado:", "Comando ejecutado:", "\u0412\u044b\u043f\u043e\u043b\u043d\u044f\u0435\u043c\u0430\u044f \u043a\u043e\u043c\u0430\u043d\u0434\u0430:", "\u6267\u884c\u7684\u547d\u4ee4:"),
        c, "\u2022 echo options bluetooth disable_ertm=Y > /etc/modprobe.d/bluetooth-ertm.conf",
        info5("Note: In both cases the bluetooth service is restarted automatically to apply the changes.",
            "Nota: Em ambos os casos o servi\u00e7o bluetooth \u00e9 reiniciado automaticamente para aplicar as mudan\u00e7as.",
            "Nota: En ambos casos, el servicio bluetooth se reinicia autom\u00e1ticamente para aplicar los cambios.",
            "\u041f\u0440\u0438\u043c\u0435\u0447\u0430\u043d\u0438\u0435: \u0412 \u043e\u0431\u043e\u0438\u0445 \u0441\u043b\u0443\u0447\u0430\u044f\u0445 \u0441\u043b\u0443\u0436\u0431\u0430 bluetooth \u0430\u0432\u0442\u043e\u043c\u0430\u0442\u0438\u0447\u0435\u0441\u043a\u0438 \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a\u0430\u0435\u0442\u0441\u044f \u0434\u043b\u044f \u043f\u0440\u0438\u043c\u0435\u043d\u0435\u043d\u0438\u044f \u0438\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u0439.",
            "\u6ce8\u610f\uff1a\u5728\u4e24\u79cd\u60c5\u51b5\u4e0b\uff0c\u84dd\u7259\u670d\u52a1\u90fd\u4f1a\u81ea\u52a8\u91cd\u542f\u4ee5\u5e94\u7528\u66f4\u6539\u3002"));
}

void on_info_realtek_clicked(GtkButton *btn, gpointer user_data) {
    const char *c = dialog_color();
    show_info_markup(
        "<b>%s</b>\n\n"
        "%s\n\n"
        "%s\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s",
        info5("Realtek Bluetooth Firmware", "Firmware Realtek Bluetooth", "Firmware Bluetooth Realtek", "\u041f\u0440\u043e\u0448\u0438\u0432\u043a\u0430 Bluetooth Realtek", "Realtek\u84dd\u7259\u56fa\u4ef6"),
        info5("Realtek chipsets (especially RTL8761B, RTL8821CE, RTL8822CU) may not work correctly without proprietary firmware. Distributions focused on free software, such as Debian and Fedora, do not include these files.",
            "Chipsets Realtek (especialmente RTL8761B, RTL8821CE, RTL8822CU) podem n\u00e3o funcionar corretamente sem firmware propriet\u00e1rio. Distribui\u00e7\u00f5es focadas em software livre, como Debian e Fedora, n\u00e3o incluem estes arquivos.",
            "Los conjuntos de chips Realtek (especialmente RTL8761B, RTL8821CE, RTL8822CU) pueden no funcionar correctamente sin firmware propietario. Las distribuciones centradas en software libre, como Debian y Fedora, no incluyen estos archivos.",
            "\u0427\u0438\u043f\u0441\u0435\u0442\u044b Realtek (\u043e\u0441\u043e\u0431\u0435\u043d\u043d\u043e RTL8761B, RTL8821CE, RTL8822CU) \u043c\u043e\u0433\u0443\u0442 \u043d\u0435 \u0440\u0430\u0431\u043e\u0442\u0430\u0442\u044c \u043a\u043e\u0440\u0440\u0435\u043a\u0442\u043d\u043e \u0431\u0435\u0437 \u043f\u0440\u043e\u043f\u0440\u0438\u0435\u0442\u0430\u0440\u043d\u043e\u0439 \u043f\u0440\u043e\u0448\u0438\u0432\u043a\u0438. \u0414\u0438\u0441\u0442\u0440\u0438\u0431\u0443\u0442\u0438\u0432\u044b, \u043e\u0440\u0438\u0435\u043d\u0442\u0438\u0440\u043e\u0432\u0430\u043d\u043d\u044b\u0435 \u043d\u0430 \u0441\u0432\u043e\u0431\u043e\u0434\u043d\u043e\u0435 \u041f\u041e, \u0442\u0430\u043a\u0438\u0435 \u043a\u0430\u043a Debian \u0438 Fedora, \u043d\u0435 \u0432\u043a\u043b\u044e\u0447\u0430\u044e\u0442 \u044d\u0442\u0438 \u0444\u0430\u0439\u043b\u044b.",
            "Realtek\u82af\u7247\u7ec4\uff08\u7279\u522b\u662fRTL8761B\u3001RTL8821CE\u3001RTL8822CU\uff09\u5728\u6ca1\u6709\u4e13\u6709\u56fa\u4ef6\u7684\u60c5\u51b5\u4e0b\u53ef\u80fd\u65e0\u6cd5\u6b63\u5e38\u5de5\u4f5c\u3002\u4e13\u6ce8\u4e8e\u81ea\u7531\u8f6f\u4ef6\u7684\u53d1\u884c\u7248\uff0c\u5982Debian\u548cFedora\uff0c\u4e0d\u5305\u542b\u8fd9\u4e9b\u6587\u4ef6\u3002"),
        info5("This fix installs the Realtek firmware on the system, allowing the chip to be correctly initialized by the btusb driver.",
            "Esta corre\u00e7\u00e3o instala o firmware Realtek no sistema, permitindo que o chip seja corretamente inicializado pelo driver btusb.",
            "Esta correcci\u00f3n instala el firmware Realtek en el sistema, permitiendo que el chip sea correctamente inicializado por el controlador btusb.",
            "\u042d\u0442\u043e \u0438\u0441\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0435 \u0443\u0441\u0442\u0430\u043d\u0430\u0432\u043b\u0438\u0432\u0430\u0435\u0442 \u043f\u0440\u043e\u0448\u0438\u0432\u043a\u0443 Realtek \u0432 \u0441\u0438\u0441\u0442\u0435\u043c\u0443, \u043f\u043e\u0437\u0432\u043e\u043b\u044f\u044f \u0447\u0438\u043f\u0443 \u043a\u043e\u0440\u0440\u0435\u043a\u0442\u043d\u043e \u0438\u043d\u0438\u0446\u0438\u0430\u043b\u0438\u0437\u0438\u0440\u043e\u0432\u0430\u0442\u044c\u0441\u044f \u0434\u0440\u0430\u0439\u0432\u0435\u0440\u043e\u043c btusb.",
            "\u6b64\u4fee\u590d\u5c06Realtek\u56fa\u4ef6\u5b89\u88c5\u5230\u7cfb\u7edf\u4e2d\uff0c\u5141\u8bb8\u82af\u7247\u88abbtusb\u9a71\u52a8\u7a0b\u5e8f\u6b63\u786e\u521d\u59cb\u5316\u3002"),
        info5("Install command:", "Comando de Instala\u00e7\u00e3o:", "Comando de instalaci\u00f3n:", "\u041a\u043e\u043c\u0430\u043d\u0434\u0430 \u0443\u0441\u0442\u0430\u043d\u043e\u0432\u043a\u0438:", "\u5b89\u88c5\u547d\u4ee4:"),
        c, "\u2022 rtl8761b_fw.bin obtained from firmware-realtek package",
        info5("Attention: Installation requires administrative access. The firmware will be extracted and loaded automatically.",
            "Aten\u00e7\u00e3o: A instala\u00e7\u00e3o requer acesso administrativo. O firmware ser\u00e1 extra\u00eddo e carregado automaticamente.",
            "Atenci\u00f3n: La instalaci\u00f3n requiere acceso administrativo. El firmware se extraer\u00e1 y cargar\u00e1 autom\u00e1ticamente.",
            "\u0412\u043d\u0438\u043c\u0430\u043d\u0438\u0435: \u0423\u0441\u0442\u0430\u043d\u043e\u0432\u043a\u0430 \u0442\u0440\u0435\u0431\u0443\u0435\u0442 \u0430\u0434\u043c\u0438\u043d\u0438\u0441\u0442\u0440\u0430\u0442\u0438\u0432\u043d\u043e\u0433\u043e \u0434\u043e\u0441\u0442\u0443\u043f\u0430. \u041f\u0440\u043e\u0448\u0438\u0432\u043a\u0430 \u0431\u0443\u0434\u0435\u0442 \u0430\u0432\u0442\u043e\u043c\u0430\u0442\u0438\u0447\u0435\u0441\u043a\u0438 \u0438\u0437\u0432\u043b\u0435\u0447\u0435\u043d\u0430 \u0438 \u0437\u0430\u0433\u0440\u0443\u0436\u0435\u043d\u0430.",
            "\u6ce8\u610f\uff1a\u5b89\u88c5\u9700\u8981\u7ba1\u7406\u5458\u6743\u9650\u3002\u56fa\u4ef6\u5c06\u81ea\u52a8\u63d0\u53d6\u5e76\u52a0\u8f7d\u3002"));
}

void on_info_rfkill_clicked(GtkButton *btn, gpointer user_data) {
    const char *c = dialog_color();
    show_info_markup(
        "<b>%s</b>\n\n"
        "%s\n\n"
        "<b>%s</b>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>",
        info5("RFKILL Electrical Unblock (Airplane Mode)", "Desbloqueio El\u00e9trico via RFKILL (Modo Avi\u00e3o)", "Desbloqueo El\u00e9ctrico v\u00eda RFKILL (Modo Avión)", "\u042d\u043b\u0435\u043a\u0442\u0440\u0438\u0447\u0435\u0441\u043a\u043e\u0435 \u0440\u0430\u0437\u0431\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u0430\u043d\u0438\u0435 RFKILL (\u0420\u0435\u0436\u0438\u043c \u043f\u043e\u043b\u0451\u0442\u0430)", "\u901a\u8fc7RFKILL\u7535\u6c14\u89e3\u9501\uff08\u98de\u884c\u6a21\u5f0f\uff09"),
        info5("Executes a direct hardware command to break software locks (Soft Blocks) that persist even when the user tries to activate Bluetooth through their desktop environment's native settings.",
            "Executa um comando direto de hardware para quebrar travas de software (Soft Blocks) que persistem mesmo quando o usu\u00e1rio tenta ativar o Bluetooth pelas configura\u00e7\u00f5es nativas da sua interface de desktop.",
            "Ejecuta un comando directo de hardware para romper bloqueos de software (Soft Blocks) que persisten incluso cuando el usuario intenta activar Bluetooth desde la configuraci\u00f3n nativa de su escritorio.",
            "\u0412\u044b\u043f\u043e\u043b\u043d\u044f\u0435\u0442 \u043f\u0440\u044f\u043c\u0443\u044e \u043a\u043e\u043c\u0430\u043d\u0434\u0443 \u043e\u0431\u043e\u0440\u0443\u0434\u043e\u0432\u0430\u043d\u0438\u044f \u0434\u043b\u044f \u0441\u043d\u044f\u0442\u0438\u044f \u043f\u0440\u043e\u0433\u0440\u0430\u043c\u043c\u043d\u044b\u0445 \u0431\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u043e\u043a (Soft Blocks), \u043a\u043e\u0442\u043e\u0440\u044b\u0435 \u0441\u043e\u0445\u0440\u0430\u043d\u044f\u044e\u0442\u0441\u044f \u0434\u0430\u0436\u0435 \u043f\u0440\u0438 \u043f\u043e\u043f\u044b\u0442\u043a\u0435 \u0432\u043a\u043b\u044e\u0447\u0438\u0442\u044c Bluetooth \u0447\u0435\u0440\u0435\u0437 \u0441\u0442\u0430\u043d\u0434\u0430\u0440\u0442\u043d\u044b\u0435 \u043d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438 \u0440\u0430\u0431\u043e\u0447\u0435\u0433\u043e \u0441\u0442\u043e\u043b\u0430.",
            "\u6267\u884c\u786c\u4ef6\u76f4\u63a5\u547d\u4ee4\u4ee5\u6253\u7834\u8f6f\u4ef6\u9501\uff08Soft Blocks\uff09\uff0c\u8be5\u9501\u5728\u7528\u6237\u5c1d\u8bd5\u901a\u8fc7\u684c\u9762\u73af\u5883\u539f\u751f\u8bbe\u7f6e\u542f\u7528\u84dd\u7259\u65f6\u4ecd\u7136\u5b58\u5728\u3002"),
        info5("Command executed:", "Comando Executado:", "Comando ejecutado:", "\u0412\u044b\u043f\u043e\u043b\u043d\u044f\u0435\u043c\u0430\u044f \u043a\u043e\u043c\u0430\u043d\u0434\u0430:", "\u6267\u884c\u7684\u547d\u4ee4:"),
        c, "\u2022 rfkill unblock bluetooth &amp;&amp; bluetoothctl power on");
}

void on_info_restart_clicked(GtkButton *btn, gpointer user_data) {
    const char *c = dialog_color();
    show_info_markup(
        "<b>%s</b>\n\n"
        "%s\n\n"
        "<b>%s</b>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>",
        info5("Bluetooth Service Restart", "Reinicializa\u00e7\u00e3o do Servi\u00e7o Bluetooth", "Reinicio del Servicio Bluetooth", "\u041f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a \u0441\u043b\u0443\u0436\u0431\u044b Bluetooth", "\u91cd\u542f\u84dd\u7259\u670d\u52a1"),
        info5("Forces the system service to tear down and rebuild the entire Bluetooth stack. This clears corrupted memory buffers and forces immediate loading of any configuration file changes made.",
            "For\u00e7a o servi\u00e7o do sistema a derrubar e reerguer a stack inteira de bluetooth do sistema operacional. Isso limpa buffers de mem\u00f3ria corrompidos e for\u00e7a o carregamento imediato de quaisquer modifica\u00e7\u00f5es feitas nos arquivos de configura\u00e7\u00e3o.",
            "Fuerza al servicio del sistema a derribar y reconstruir toda la pila Bluetooth. Esto limpia los b\u00faferes de memoria corruptos y fuerza la carga inmediata de cualquier cambio en los archivos de configuraci\u00f3n.",
            "\u041f\u0440\u0438\u043d\u0443\u0436\u0434\u0430\u0435\u0442 \u0441\u0438\u0441\u0442\u0435\u043c\u043d\u0443\u044e \u0441\u043b\u0443\u0436\u0431\u0443 \u043e\u0441\u0442\u0430\u043d\u043e\u0432\u0438\u0442\u044c \u0438 \u043f\u0435\u0440\u0435\u0441\u0442\u0440\u043e\u0438\u0442\u044c \u0432\u0435\u0441\u044c \u0441\u0442\u0435\u043a Bluetooth. \u042d\u0442\u043e \u043e\u0447\u0438\u0449\u0430\u0435\u0442 \u043f\u043e\u0432\u0440\u0435\u0436\u0434\u0451\u043d\u043d\u044b\u0435 \u0431\u0443\u0444\u0435\u0440\u044b \u043f\u0430\u043c\u044f\u0442\u0438 \u0438 \u043e\u0431\u0435\u0441\u043f\u0435\u0447\u0438\u0432\u0430\u0435\u0442 \u043d\u0435\u043c\u0435\u0434\u043b\u0435\u043d\u043d\u0443\u044e \u0437\u0430\u0433\u0440\u0443\u0437\u043a\u0443 \u043b\u044e\u0431\u044b\u0445 \u0438\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u0439 \u0432 \u043a\u043e\u043d\u0444\u0438\u0433\u0443\u0440\u0430\u0446\u0438\u043e\u043d\u043d\u044b\u0445 \u0444\u0430\u0439\u043b\u0430\u0445.",
            "\u5f3a\u5236\u7cfb\u7edf\u670d\u52a1\u62c6\u9664\u5e76\u91cd\u5efa\u6574\u4e2a\u84dd\u7259\u534f\u8bae\u6808\u3002\u8fd9\u4f1a\u6e05\u9664\u53d7\u635f\u7684\u5185\u5b58\u7f13\u51b2\u533a\uff0c\u5e76\u5f3a\u5236\u7acb\u5373\u52a0\u8f7d\u5bf9\u914d\u7f6e\u6587\u4ef6\u505a\u51fa\u7684\u4efb\u4f55\u4fee\u6539\u3002"),
        info5("Command executed:", "Comando Executado:", "Comando ejecutado:", "\u0412\u044b\u043f\u043e\u043b\u043d\u044f\u0435\u043c\u0430\u044f \u043a\u043e\u043c\u0430\u043d\u0434\u0430:", "\u6267\u884c\u7684\u547d\u4ee4:"),
        c, restart_cmd);
}

void on_info_legacy_clicked(GtkButton *btn, gpointer user_data) {
    const char *c = dialog_color();
    show_info_markup(
        "<b>%s</b>\n\n"
        "%s\n\n"
        "%s\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s",
        info5("Legacy Bluetooth Compatibility Mode", "Modo de Compatibilidade Bluetooth Legado", "Modo de Compatibilidad Bluetooth Heredado", "\u0420\u0435\u0436\u0438\u043c \u0443\u0441\u0442\u0430\u0440\u0435\u0432\u0448\u0435\u0439 \u0441\u043e\u0432\u043c\u0435\u0441\u0442\u0438\u043c\u043e\u0441\u0442\u0438 Bluetooth", "\u9057\u7559\u84dd\u7259\u517c\u5bb9\u6a21\u5f0f"),
        info5("Since BlueZ 5.x, SSP (Secure Simple Pairing) mode is the default. However, older devices (manufactured before 2014) do not support SSP and simply refuse to pair or connect.",
            "A partir do BlueZ 5.x, o modo de seguran\u00e7a SSP (Secure Simple Pairing) \u00e9 o padr\u00e3o. Por\u00e9m, dispositivos antigos (fabricados antes de 2014) n\u00e3o suportam SSP e simplesmente se recusam a parear ou conectar.",
            "Desde BlueZ 5.x, el modo SSP (Secure Simple Pairing) es el predeterminado. Sin embargo, los dispositivos antiguos (fabricados antes de 2014) no soportan SSP y simplemente se niegan a emparejar o conectar.",
            "\u041d\u0430\u0447\u0438\u043d\u0430\u044f \u0441 BlueZ 5.x, \u0440\u0435\u0436\u0438\u043c SSP (Secure Simple Pairing) \u044f\u0432\u043b\u044f\u0435\u0442\u0441\u044f \u0440\u0435\u0436\u0438\u043c\u043e\u043c \u043f\u043e \u0443\u043c\u043e\u043b\u0447\u0430\u043d\u0438\u044e. \u041e\u0434\u043d\u0430\u043a\u043e \u0441\u0442\u0430\u0440\u044b\u0435 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0430 (\u0438\u0437\u0433\u043e\u0442\u043e\u0432\u043b\u0435\u043d\u043d\u044b\u0435 \u0434\u043e 2014 \u0433\u043e\u0434\u0430) \u043d\u0435 \u043f\u043e\u0434\u0434\u0435\u0440\u0436\u0438\u0432\u0430\u044e\u0442 SSP \u0438 \u043f\u0440\u043e\u0441\u0442\u043e \u043e\u0442\u043a\u0430\u0437\u044b\u0432\u0430\u044e\u0442\u0441\u044f \u043e\u0442 \u0441\u043e\u043f\u0440\u044f\u0436\u0435\u043d\u0438\u044f \u0438\u043b\u0438 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f.",
            "\u81eaBlueZ 5.x\u4ee5\u6765\uff0cSSP\uff08\u5b89\u5168\u7b80\u5355\u914d\u5bf9\uff09\u6a21\u5f0f\u662f\u9ed8\u8ba4\u6a21\u5f0f\u3002\u4f46\u662f\uff0c\u8001\u5f0f\u8bbe\u5907\uff082014\u5e74\u4e4b\u524d\u751f\u4ea7\uff09\u4e0d\u652f\u6301SSP\uff0c\u7b80\u76f4\u62d2\u7edd\u914d\u5bf9\u6216\u8fde\u63a5\u3002"),
        info5("This fix enables legacy security mode in the Bluetooth daemon, allowing older devices to pair and connect normally.",
            "Esta corre\u00e7\u00e3o ativa o modo de seguran\u00e7a legado no daemon Bluetooth, permitindo que dispositivos mais antigos pareiem e conectem normalmente.",
            "Esta correcci\u00f3n activa el modo de seguridad heredado en el daemon Bluetooth, permitiendo que los dispositivos m\u00e1s antiguos se emparejen y conecten normalmente.",
            "\u042d\u0442\u043e \u0438\u0441\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0435 \u0432\u043a\u043b\u044e\u0447\u0430\u0435\u0442 \u0443\u0441\u0442\u0430\u0440\u0435\u0432\u0448\u0438\u0439 \u0440\u0435\u0436\u0438\u043c \u0431\u0435\u0437\u043e\u043f\u0430\u0441\u043d\u043e\u0441\u0442\u0438 \u0432 \u0434\u0435\u043c\u043e\u043d\u0435 Bluetooth, \u043f\u043e\u0437\u0432\u043e\u043b\u044f\u044f \u0441\u0442\u0430\u0440\u044b\u043c \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0430\u043c \u043d\u043e\u0440\u043c\u0430\u043b\u044c\u043d\u043e \u0441\u043e\u043f\u0440\u044f\u0433\u0430\u0442\u044c\u0441\u044f \u0438 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0430\u0442\u044c\u0441\u044f.",
            "\u6b64\u4fee\u590d\u5728\u84dd\u7259\u540e\u53f0\u8fdb\u7a0b\u4e2d\u542f\u7528\u9057\u7559\u5b89\u5168\u6a21\u5f0f\uff0c\u5141\u8bb8\u8001\u5f0f\u8bbe\u5907\u6b63\u5e38\u914d\u5bf9\u548c\u8fde\u63a5\u3002"),
        info5("Command executed:", "Comando Executado:", "Comando ejecutado:", "\u0412\u044b\u043f\u043e\u043b\u043d\u044f\u0435\u043c\u0430\u044f \u043a\u043e\u043c\u0430\u043d\u0434\u0430:", "\u6267\u884c\u7684\u547d\u4ee4:"),
        c, "\u2022 echo Security=legacy >> /etc/bluetooth/main.conf",
        info5("Note: In both cases the bluetooth service is restarted automatically to apply the changes.",
            "Nota: Em ambos os casos o servi\u00e7o bluetooth \u00e9 reiniciado automaticamente para aplicar as mudan\u00e7as.",
            "Nota: En ambos casos, el servicio bluetooth se reinicia autom\u00e1ticamente para aplicar los cambios.",
            "\u041f\u0440\u0438\u043c\u0435\u0447\u0430\u043d\u0438\u0435: \u0412 \u043e\u0431\u043e\u0438\u0445 \u0441\u043b\u0443\u0447\u0430\u044f\u0445 \u0441\u043b\u0443\u0436\u0431\u0430 bluetooth \u0430\u0432\u0442\u043e\u043c\u0430\u0442\u0438\u0447\u0435\u0441\u043a\u0438 \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a\u0430\u0435\u0442\u0441\u044f \u0434\u043b\u044f \u043f\u0440\u0438\u043c\u0435\u043d\u0435\u043d\u0438\u044f \u0438\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u0439.",
            "\u6ce8\u610f\uff1a\u5728\u4e24\u79cd\u60c5\u51b5\u4e0b\uff0c\u84dd\u7259\u670d\u52a1\u90fd\u4f1a\u81ea\u52a8\u91cd\u542f\u4ee5\u5e94\u7528\u66f4\u6539\u3002"));
}

void on_info_broadcom_clicked(GtkButton *btn, gpointer user_data) {
    const char *c = dialog_color();
    show_info_markup(
        "<b>%s</b>\n\n"
        "%s\n\n"
        "%s\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s",
        info5("Broadcom / Cypress Firmware (b43)", "Firmware Broadcom / Cypress (b43)", "Firmware Broadcom / Cypress (b43)", "\u041f\u0440\u043e\u0448\u0438\u0432\u043a\u0430 Broadcom / Cypress (b43)", "Broadcom/Cypress\u56fa\u4ef6(b43)"),
        info5("Broadcom / Cypress Bluetooth devices depend on proprietary firmware that is removed from distributions like Debian due to licensing issues. Without these files, the hardware does not initialize correctly and may not even be detected by the system.",
            "Dispositivos Bluetooth Broadcom / Cypress dependem de firmware propriet\u00e1rio que \u00e9 removido de distribui\u00e7\u00f5es como Debian por quest\u00f5es de licenciamento. Sem estes arquivos, o hardware n\u00e3o inicializa corretamente e pode nem ser detectado pelo sistema.",
            "Los dispositivos Bluetooth Broadcom / Cypress dependen de firmware propietario que se elimina de distribuciones como Debian por problemas de licencia. Sin estos archivos, el hardware no se inicializa correctamente y puede que ni siquiera sea detectado por el sistema.",
            "\u0423\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0430 Bluetooth Broadcom / Cypress \u0437\u0430\u0432\u0438\u0441\u044f\u0442 \u043e\u0442 \u043f\u0440\u043e\u043f\u0440\u0438\u0435\u0442\u0430\u0440\u043d\u043e\u0439 \u043f\u0440\u043e\u0448\u0438\u0432\u043a\u0438, \u043a\u043e\u0442\u043e\u0440\u0430\u044f \u0443\u0434\u0430\u043b\u044f\u0435\u0442\u0441\u044f \u0438\u0437 \u0442\u0430\u043a\u0438\u0445 \u0434\u0438\u0441\u0442\u0440\u0438\u0431\u0443\u0442\u0438\u0432\u043e\u0432, \u043a\u0430\u043a Debian, \u0438\u0437-\u0437\u0430 \u043f\u0440\u043e\u0431\u043b\u0435\u043c \u043b\u0438\u0446\u0435\u043d\u0437\u0438\u0440\u043e\u0432\u0430\u043d\u0438\u044f. \u0411\u0435\u0437 \u044d\u0442\u0438\u0445 \u0444\u0430\u0439\u043b\u043e\u0432 \u043e\u0431\u043e\u0440\u0443\u0434\u043e\u0432\u0430\u043d\u0438\u0435 \u043d\u0435 \u0438\u043d\u0438\u0446\u0438\u0430\u043b\u0438\u0437\u0438\u0440\u0443\u0435\u0442\u0441\u044f \u0434\u043e\u043b\u0436\u043d\u044b\u043c \u043e\u0431\u0440\u0430\u0437\u043e\u043c \u0438 \u043c\u043e\u0436\u0435\u0442 \u0434\u0430\u0436\u0435 \u043d\u0435 \u043e\u0431\u043d\u0430\u0440\u0443\u0436\u0438\u0432\u0430\u0442\u044c\u0441\u044f \u0441\u0438\u0441\u0442\u0435\u043c\u043e\u0439.",
            "Broadcom/Cypress\u84dd\u7259\u8bbe\u5907\u4f9d\u8d56\u4e13\u6709\u56fa\u4ef6\uff0c\u7531\u4e8e\u8bb8\u53ef\u8bc1\u95ee\u9898\uff0cDebian\u7b49\u53d1\u884c\u7248\u4f1a\u5220\u9664\u8be5\u56fa\u4ef6\u3002\u6ca1\u6709\u8fd9\u4e9b\u6587\u4ef6\uff0c\u786c\u4ef6\u65e0\u6cd5\u6b63\u786e\u521d\u59cb\u5316\uff0c\u751a\u81f3\u53ef\u80fd\u65e0\u6cd5\u88ab\u7cfb\u7edf\u68c0\u6d4b\u5230\u3002"),
        info5("This fix installs the b43 firmware package necessary for Broadcom chipset operation.",
            "Esta corre\u00e7\u00e3o instala o pacote de firmware b43 necess\u00e1rio para o funcionamento do chipset Broadcom.",
            "Esta correcci\u00f3n instala el paquete de firmware b43 necesario para el funcionamiento del conjunto de chips Broadcom.",
            "\u042d\u0442\u043e \u0438\u0441\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0435 \u0443\u0441\u0442\u0430\u043d\u0430\u0432\u043b\u0438\u0432\u0430\u0435\u0442 \u043f\u0430\u043a\u0435\u0442 \u043f\u0440\u043e\u0448\u0438\u0432\u043a\u0438 b43, \u043d\u0435\u043e\u0431\u0445\u043e\u0434\u0438\u043c\u044b\u0439 \u0434\u043b\u044f \u0440\u0430\u0431\u043e\u0442\u044b \u0447\u0438\u043f\u0441\u0435\u0442\u0430 Broadcom.",
            "\u6b64\u4fee\u590d\u5b89\u88c5Broadcom\u82af\u7247\u7ec4\u8fd0\u884c\u6240\u9700\u7684b43\u56fa\u4ef6\u5305\u3002"),
        info5("Install command:", "Comando de Instala\u00e7\u00e3o:", "Comando de instalaci\u00f3n:", "\u041a\u043e\u043c\u0430\u043d\u0434\u0430 \u0443\u0441\u0442\u0430\u043d\u043e\u0432\u043a\u0438:", "\u5b89\u88c5\u547d\u4ee4:"),
        c, "\u2022 [package manager] install b43-fwcutter and dependencies",
        info5("Attention: Installation requires administrative access. The firmware will be extracted and loaded automatically.",
            "Aten\u00e7\u00e3o: A instala\u00e7\u00e3o requer acesso administrativo. O firmware ser\u00e1 extra\u00eddo e carregado automaticamente.",
            "Atenci\u00f3n: La instalaci\u00f3n requiere acceso administrativo. El firmware se extraer\u00e1 y cargar\u00e1 autom\u00e1ticamente.",
            "\u0412\u043d\u0438\u043c\u0430\u043d\u0438\u0435: \u0423\u0441\u0442\u0430\u043d\u043e\u0432\u043a\u0430 \u0442\u0440\u0435\u0431\u0443\u0435\u0442 \u0430\u0434\u043c\u0438\u043d\u0438\u0441\u0442\u0440\u0430\u0442\u0438\u0432\u043d\u043e\u0433\u043e \u0434\u043e\u0441\u0442\u0443\u043f\u0430. \u041f\u0440\u043e\u0448\u0438\u0432\u043a\u0430 \u0431\u0443\u0434\u0435\u0442 \u0430\u0432\u0442\u043e\u043c\u0430\u0442\u0438\u0447\u0435\u0441\u043a\u0438 \u0438\u0437\u0432\u043b\u0435\u0447\u0435\u043d\u0430 \u0438 \u0437\u0430\u0433\u0440\u0443\u0436\u0435\u043d\u0430.",
            "\u6ce8\u610f\uff1a\u5b89\u88c5\u9700\u8981\u7ba1\u7406\u5458\u6743\u9650\u3002\u56fa\u4ef6\u5c06\u81ea\u52a8\u63d0\u53d6\u5e76\u52a0\u8f7d\u3002"));
}

void on_info_perm_clicked(GtkButton *btn, gpointer user_data) {
    const char *c = dialog_color();
    show_info_markup(
        "<b>%s</b>\n\n"
        "%s\n\n"
        "%s\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s",
        info5("Bluetooth Group Permissions", "Permiss\u00f5es de Grupo para Bluetooth", "Permisos de Grupo para Bluetooth", "\u0413\u0440\u0443\u043f\u043f\u043e\u0432\u044b\u0435 \u0440\u0430\u0437\u0440\u0435\u0448\u0435\u043d\u0438\u044f Bluetooth", "\u84dd\u7259\u7ec4\u6743\u9650"),
        info5("On minimal installations, the logged-in user may not belong to the lp group, which grants access to the Bluetooth D-Bus socket. This causes the Bluetooth icon to appear in the tray, but no devices can pair or connect.",
            "Em instala\u00e7\u00f5es minimalistas, o usu\u00e1rio logado pode n\u00e3o pertencer ao grupo lp, respons\u00e1vel por conceder acesso ao socket D-Bus do Bluetooth. Isso faz com que o \u00edcone do Bluetooth apare\u00e7a na bandeja, mas nenhum dispositivo consiga parear ou conectar.",
            "En instalaciones minimalistas, el usuario conectado puede no pertenecer al grupo lp, responsable de otorgar acceso al socket D-Bus de Bluetooth. Esto hace que el icono de Bluetooth aparezca en la bandeja, pero ning\u00fan dispositivo pueda emparejarse o conectarse.",
            "\u041f\u0440\u0438 \u043c\u0438\u043d\u0438\u043c\u0430\u043b\u044c\u043d\u044b\u0445 \u0443\u0441\u0442\u0430\u043d\u043e\u0432\u043a\u0430\u0445 \u0432\u043e\u0448\u0435\u0434\u0448\u0438\u0439 \u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u0435\u043b\u044c \u043c\u043e\u0436\u0435\u0442 \u043d\u0435 \u043f\u0440\u0438\u043d\u0430\u0434\u043b\u0435\u0436\u0430\u0442\u044c \u043a \u0433\u0440\u0443\u043f\u043f\u0435 lp, \u043a\u043e\u0442\u043e\u0440\u0430\u044f \u043f\u0440\u0435\u0434\u043e\u0441\u0442\u0430\u0432\u043b\u044f\u0435\u0442 \u0434\u043e\u0441\u0442\u0443\u043f \u043a \u0441\u043e\u043a\u0435\u0442\u0443 D-Bus Bluetooth. \u0418\u0437-\u0437\u0430 \u044d\u0442\u043e\u0433\u043e \u0437\u043d\u0430\u0447\u043e\u043a Bluetooth \u043f\u043e\u044f\u0432\u043b\u044f\u0435\u0442\u0441\u044f \u0432 \u0442\u0440\u0435\u0435, \u043d\u043e \u043d\u0438 \u043e\u0434\u043d\u043e \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u043e \u043d\u0435 \u043c\u043e\u0436\u0435\u0442 \u0431\u044b\u0442\u044c \u0441\u043e\u043f\u0440\u044f\u0436\u0435\u043d\u043e \u0438\u043b\u0438 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u043e.",
            "\u5728\u6700\u5c0f\u5316\u5b89\u88c5\u4e2d\uff0c\u5f53\u524d\u767b\u5f55\u7528\u6237\u53ef\u80fd\u4e0d\u5c5e\u4e8elp\u7ec4\uff0c\u8be5\u7ec4\u7528\u4e8e\u6388\u4e88\u5bf9\u84dd\u7259D-Bus\u5957\u63a5\u5b57\u7684\u8bbf\u95ee\u6743\u9650\u3002\u8fd9\u4f1a\u5bfc\u81f4\u84dd\u7259\u56fe\u6807\u51fa\u73b0\u5728\u7cfb\u7edf\u6258\u76d8\u4e2d\uff0c\u4f46\u65e0\u6cd5\u914d\u5bf9\u6216\u8fde\u63a5\u4efb\u4f55\u8bbe\u5907\u3002"),
        info5("This action adds the current user to the lp group, resolving the permission problem.",
            "Esta a\u00e7\u00e3o adiciona o usu\u00e1rio atual ao grupo lp, resolvendo o problema de permiss\u00e3o.",
            "Esta acci\u00f3n agrega el usuario actual al grupo lp, resolviendo el problema de permisos.",
            "\u042d\u0442\u043e \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u0435 \u0434\u043e\u0431\u0430\u0432\u043b\u044f\u0435\u0442 \u0442\u0435\u043a\u0443\u0449\u0435\u0433\u043e \u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u0435\u043b\u044f \u0432 \u0433\u0440\u0443\u043f\u043f\u0443 lp, \u0440\u0435\u0448\u0430\u044f \u043f\u0440\u043e\u0431\u043b\u0435\u043c\u0443 \u0440\u0430\u0437\u0440\u0435\u0448\u0435\u043d\u0438\u0439.",
            "\u6b64\u64cd\u4f5c\u5c06\u5f53\u524d\u7528\u6237\u6dfb\u52a0\u5230lp\u7ec4\uff0c\u89e3\u51b3\u6743\u9650\u95ee\u9898\u3002"),
        info5("Command executed:", "Comando Executado:", "Comando ejecutado:", "\u0412\u044b\u043f\u043e\u043b\u043d\u044f\u0435\u043c\u0430\u044f \u043a\u043e\u043c\u0430\u043d\u0434\u0430:", "\u6267\u884c\u7684\u547d\u4ee4:"),
        c, "\u2022 usermod -aG lp &lt;current_user&gt;",
        info5("Important: After execution, you need to restart the session (logout and login) for the new permissions to take effect.",
            "Importante: Ap\u00f3s a execu\u00e7\u00e3o, \u00e9 necess\u00e1rio reiniciar a sess\u00e3o (logout e login) para que as novas permiss\u00f5es tenham efeito.",
            "Importante: Despu\u00e9s de la ejecuci\u00f3n, es necesario reiniciar la sesi\u00f3n (cerrar sesi\u00f3n e iniciarla de nuevo) para que los nuevos permisos surtan efecto.",
            "\u0412\u0430\u0436\u043d\u043e: \u041f\u043e\u0441\u043b\u0435 \u0432\u044b\u043f\u043e\u043b\u043d\u0435\u043d\u0438\u044f \u043d\u0435\u043e\u0431\u0445\u043e\u0434\u0438\u043c\u043e \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u044c \u0441\u0435\u0430\u043d\u0441 (\u0432\u044b\u0439\u0442\u0438 \u0438 \u0432\u043e\u0439\u0442\u0438), \u0447\u0442\u043e\u0431\u044b \u043d\u043e\u0432\u044b\u0435 \u0440\u0430\u0437\u0440\u0435\u0448\u0435\u043d\u0438\u044f \u0432\u0441\u0442\u0443\u043f\u0438\u043b\u0438 \u0432 \u0441\u0438\u043b\u0443.",
            "\u91cd\u8981\uff1a\u6267\u884c\u540e\uff0c\u60a8\u9700\u8981\u91cd\u542f\u4f1a\u8bdd\uff08\u6ce8\u9500\u5e76\u91cd\u65b0\u767b\u5f55\uff09\u4ee5\u4f7f\u65b0\u6743\u9650\u751f\u6548\u3002"));
}

void on_info_cache_clicked(GtkButton *btn, gpointer user_data) {
    const char *c = dialog_color();
    show_info_markup(
        "<b>%s</b>\n\n"
        "%s\n\n"
        "%s\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s",
        info5("Bluetooth Device Cache Cleanup", "Limpeza do Cache de Dispositivos Bluetooth", "Limpieza de Cach\u00e9 de Dispositivos Bluetooth", "\u041e\u0447\u0438\u0441\u0442\u043a\u0430 \u043a\u044d\u0448\u0430 Bluetooth-\u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432", "\u84dd\u7259\u8bbe\u5907\u7f13\u5b58\u6e05\u9664"),
        info5("The system stores information about previously paired devices in /var/lib/bluetooth/[MAC]/[device]/. If a device has been reset or its service profile has changed, this old cache can cause errors like \"Protocol not supported\" or \"Connection Refused\".",
            "O sistema armazena informa\u00e7\u00f5es de dispositivos pareados anteriormente em /var/lib/bluetooth/[MAC]/[dispositivo]/. Se um dispositivo foi resetado ou seu perfil de servi\u00e7o mudou, esse cache antigo pode causar erros como \"Protocolo n\u00e3o suportado\" ou \"Connection Refused\".",
            "El sistema almacena informaci\u00f3n de dispositivos previamente emparejados en /var/lib/bluetooth/[MAC]/[dispositivo]/. Si un dispositivo ha sido restablecido o su perfil de servicio ha cambiado, esta cach\u00e9 antigua puede causar errores como \"Protocolo no compatible\" o \"Connection Refused\".",
            "\u0421\u0438\u0441\u0442\u0435\u043c\u0430 \u0445\u0440\u0430\u043d\u0438\u0442 \u0438\u043d\u0444\u043e\u0440\u043c\u0430\u0446\u0438\u044e \u043e \u0440\u0430\u043d\u0435\u0435 \u0441\u043e\u043f\u0440\u044f\u0436\u0451\u043d\u043d\u044b\u0445 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0430\u0445 \u0432 /var/lib/bluetooth/[MAC]/[\u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u043e]/. \u0415\u0441\u043b\u0438 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u043e \u0431\u044b\u043b\u043e \u0441\u0431\u0440\u043e\u0448\u0435\u043d\u043e \u0438\u043b\u0438 \u0435\u0433\u043e \u043f\u0440\u043e\u0444\u0438\u043b\u044c \u0443\u0441\u043b\u0443\u0433\u0438 \u0438\u0437\u043c\u0435\u043d\u0438\u043b\u0441\u044f, \u044d\u0442\u043e\u0442 \u0441\u0442\u0430\u0440\u044b\u0439 \u043a\u044d\u0448 \u043c\u043e\u0436\u0435\u0442 \u0432\u044b\u0437\u044b\u0432\u0430\u0442\u044c \u043e\u0448\u0438\u0431\u043a\u0438, \u0442\u0430\u043a\u0438\u0435 \u043a\u0430\u043a \"\u041f\u0440\u043e\u0442\u043e\u043a\u043e\u043b \u043d\u0435 \u043f\u043e\u0434\u0434\u0435\u0440\u0436\u0438\u0432\u0430\u0435\u0442\u0441\u044f\" \u0438\u043b\u0438 \"Connection Refused\".",
            "\u7cfb\u7edf\u5c06\u4e4b\u524d\u914d\u5bf9\u8fc7\u7684\u8bbe\u5907\u7684\u4fe1\u606f\u5b58\u50a8\u5728/var/lib/bluetooth/[MAC]/[\u8bbe\u5907]/\u3002\u5982\u679c\u8bbe\u5907\u5df2\u91cd\u7f6e\u6216\u5176\u670d\u52a1\u914d\u7f6e\u6587\u4ef6\u5df2\u66f4\u6539\uff0c\u8fd9\u4e2a\u65e7\u7f13\u5b58\u53ef\u80fd\u4f1a\u5bfc\u81f4\u9519\u8bef\uff0c\u5982\u201c\u4e0d\u652f\u6301\u7684\u534f\u8bae\u201d\u6216\u201cConnection Refused\u201d\u3002"),
        info5("This action stops the Bluetooth service, removes all device cache folders, and restarts the service, forcing a clean pairing from scratch.",
            "Esta a\u00e7\u00e3o para o servi\u00e7o Bluetooth, remove todas as pastas de cache de dispositivos e reinicia o servi\u00e7o, for\u00e7ando um pareamento limpo do zero.",
            "Esta acci\u00f3n detiene el servicio Bluetooth, elimina todas las carpetas de cach\u00e9 de dispositivos y reinicia el servicio, forzando un emparejamiento limpio desde cero.",
            "\u042d\u0442\u043e \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u0435 \u043e\u0441\u0442\u0430\u043d\u0430\u0432\u043b\u0438\u0432\u0430\u0435\u0442 \u0441\u043b\u0443\u0436\u0431\u0443 Bluetooth, \u0443\u0434\u0430\u043b\u044f\u0435\u0442 \u0432\u0441\u0435 \u043f\u0430\u043f\u043a\u0438 \u043a\u044d\u0448\u0430 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432 \u0438 \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a\u0430\u0435\u0442 \u0441\u043b\u0443\u0436\u0431\u0443, \u043e\u0431\u0435\u0441\u043f\u0435\u0447\u0438\u0432\u0430\u044f \u0447\u0438\u0441\u0442\u043e\u0435 \u0441\u043e\u043f\u0440\u044f\u0436\u0435\u043d\u0438\u0435 \u0441 \u043d\u0443\u043b\u044f.",
            "\u6b64\u64cd\u4f5c\u505c\u6b62\u84dd\u7259\u670d\u52a1\uff0c\u5220\u9664\u6240\u6709\u8bbe\u5907\u7f13\u5b58\u6587\u4ef6\u5939\uff0c\u5e76\u91cd\u542f\u670d\u52a1\uff0c\u5f3a\u5236\u4ece\u5934\u5f00\u59cb\u6e05\u6d01\u914d\u5bf9\u3002"),
        info5("Commands executed:", "Comandos Executados:", "Comandos ejecutados:", "\u0412\u044b\u043f\u043e\u043b\u043d\u044f\u0435\u043c\u044b\u0435 \u043a\u043e\u043c\u0430\u043d\u0434\u044b:", "\u6267\u884c\u7684\u547d\u4ee4:"),
        c, "\u2022 stop bluetooth",
        c, "\u2022 rm -rf /var/lib/bluetooth/*/*",
        c, "\u2022 start bluetooth",
        info5("Attention: All paired devices will be removed and will need to be paired again.",
            "Aten\u00e7\u00e3o: Todos os dispositivos pareados ser\u00e3o removidos e precisar\u00e3o ser pareados novamente.",
            "Atenci\u00f3n: Todos los dispositivos emparejados se eliminar\u00e1n y ser\u00e1 necesario emparejarlos de nuevo.",
            "\u0412\u043d\u0438\u043c\u0430\u043d\u0438\u0435: \u0412\u0441\u0435 \u0441\u043e\u043f\u0440\u044f\u0436\u0451\u043d\u043d\u044b\u0435 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0430 \u0431\u0443\u0434\u0443\u0442 \u0443\u0434\u0430\u043b\u0435\u043d\u044b, \u0438 \u0438\u0445 \u043f\u0440\u0438\u0434\u0451\u0442\u0441\u044f \u0441\u043e\u043f\u0440\u044f\u0433\u0430\u0442\u044c \u0437\u0430\u043d\u043e\u0432\u043e.",
            "\u6ce8\u610f\uff1a\u6240\u6709\u5df2\u914d\u5bf9\u7684\u8bbe\u5907\u5c06\u88ab\u5220\u9664\uff0c\u5e76\u9700\u8981\u91cd\u65b0\u914d\u5bf9\u3002"));
}

void show_keyboard_shortcuts(void) {
    show_info_dialog(info5(
        "<b>Keyboard Shortcuts</b>\n\n"
        "<tt>F1</tt>           Open keyboard shortcuts\n"
        "<tt>F5</tt>           Scan for Bluetooth devices (Home screen)\n"
        "<tt>F12</tt>          Toggle light/dark theme\n"
        "<tt>Ctrl+Shift+C</tt> Copy tech sheet data (Details page)",
        "<b>Atalhos do Teclado</b>\n\n"
        "<tt>F1</tt>           Abrir os atalhos do teclado\n"
        "<tt>F5</tt>           Escanear dispositivos Bluetooth (tela inicial)\n"
        "<tt>F12</tt>          Alternar tema claro/escuro\n"
        "<tt>Ctrl+Shift+C</tt> Copiar dados t\u00e9cnicos (p\u00e1gina de detalhes)",
        "<b>Atajos del Teclado</b>\n\n"
        "<tt>F1</tt>           Abrir atajos del teclado\n"
        "<tt>F5</tt>           Escanear dispositivos Bluetooth (pantalla inicial)\n"
        "<tt>F12</tt>          Alternar tema claro/oscuro\n"
        "<tt>Ctrl+May\u00fas+C</tt> Copiar datos t\u00e9cnicos (p\u00e1gina de detalles)",
        "<b>\u0413\u043e\u0440\u044f\u0447\u0438\u0435 \u043a\u043b\u0430\u0432\u0438\u0448\u0438</b>\n\n"
        "<tt>F1</tt>           \u041e\u0442\u043a\u0440\u044b\u0442\u044c \u0433\u043e\u0440\u044f\u0447\u0438\u0435 \u043a\u043b\u0430\u0432\u0438\u0448\u0438\n"
        "<tt>F5</tt>           \u0421\u043a\u0430\u043d\u0438\u0440\u043e\u0432\u0430\u0442\u044c \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0430 Bluetooth (\u0413\u043b\u0430\u0432\u043d\u044b\u0439 \u044d\u043a\u0440\u0430\u043d)\n"
        "<tt>F12</tt>          \u041f\u0435\u0440\u0435\u043a\u043b\u044e\u0447\u0438\u0442\u044c \u0441\u0432\u0435\u0442\u043b\u0443\u044e/\u0442\u0435\u043c\u043d\u0443\u044e \u0442\u0435\u043c\u0443\n"
        "<tt>Ctrl+Shift+C</tt> \u041a\u043e\u043f\u0438\u0440\u043e\u0432\u0430\u0442\u044c \u0442\u0435\u0445\u043d\u0438\u0447\u0435\u0441\u043a\u0438\u0435 \u0434\u0430\u043d\u043d\u044b\u0435 (\u0441\u0442\u0440\u0430\u043d\u0438\u0446\u0430 \u0434\u0435\u0442\u0430\u043b\u0435\u0439)",
        "<b>\u5feb\u6377\u952e</b>\n\n"
        "<tt>F1</tt>           \u6253\u5f00\u5feb\u6377\u952e\n"
        "<tt>F5</tt>           \u626b\u63cf\u84dd\u7259\u8bbe\u5907\uff08\u4e3b\u5c4f\u5e55\uff09\n"
        "<tt>F12</tt>          \u5207\u6362\u6d45\u8272/\u6df1\u8272\u4e3b\u9898\n"
        "<tt>Ctrl+Shift+C</tt> \u590d\u5236\u6280\u672f\u6570\u636e\uff08\u8be6\u60c5\u9875\uff09"
    ));
}

static void on_prefs_lang_selected(GObject *obj, GParamSpec *pspec, gpointer d) {
    LangId ids[] = {LANG_SYS, LANG_EN, LANG_PT, LANG_ES, LANG_RU, LANG_ZH};
    guint sel = gtk_drop_down_get_selected(GTK_DROP_DOWN(obj));
    if (sel < G_N_ELEMENTS(ids))
        set_language(ids[sel]);
}

static void on_prefs_theme_selected(GObject *obj, GParamSpec *pspec, gpointer d) {
    guint sel = gtk_drop_down_get_selected(GTK_DROP_DOWN(obj));
    if (sel <= THEME_DARK)
        set_theme((ThemeId)sel);
}

void show_preferences_dialog(void) {
    AdwAlertDialog *dialog = ADW_ALERT_DIALOG(adw_alert_dialog_new(_("Preferences"), NULL));

    GtkWidget *page = adw_preferences_page_new();

    GtkWidget *group = adw_preferences_group_new();
    adw_preferences_page_add(ADW_PREFERENCES_PAGE(page), ADW_PREFERENCES_GROUP(group));

    /* Language row */
    GtkWidget *lang_row = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(lang_row), _("Language"));
    adw_action_row_set_subtitle(ADW_ACTION_ROW(lang_row), info5(
        "Choose the interface language. Restart required.",
        "Escolha o idioma da interface. Reinicializa\u00e7\u00e3o necess\u00e1ria.",
        "Elija el idioma de la interfaz. Requiere reinicio.",
        "\u0412\u044b\u0431\u0435\u0440\u0438\u0442\u0435 \u044f\u0437\u044b\u043a \u0438\u043d\u0442\u0435\u0440\u0444\u0435\u0439\u0441\u0430. \u0422\u0440\u0435\u0431\u0443\u0435\u0442\u0441\u044f \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a.",
        "\u9009\u62e9\u754c\u9762\u8bed\u8a00\u3002\u9700\u8981\u91cd\u542f\u3002"));

    GtkStringList *lang_model = gtk_string_list_new(NULL);
    LangId lang_ids[] = {LANG_SYS, LANG_EN, LANG_PT, LANG_ES, LANG_RU, LANG_ZH};
    for (size_t i = 0; i < G_N_ELEMENTS(lang_ids); i++)
        gtk_string_list_append(lang_model, lang_label(lang_ids[i]));

    GtkWidget *lang_dropdown = gtk_drop_down_new(G_LIST_MODEL(lang_model), NULL);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(lang_dropdown), current_lang);
    g_signal_connect(lang_dropdown, "notify::selected", G_CALLBACK(on_prefs_lang_selected), NULL);
    adw_action_row_add_suffix(ADW_ACTION_ROW(lang_row), lang_dropdown);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(group), lang_row);

    /* Theme row */
    GtkWidget *theme_row = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(theme_row), _("Theme"));
    adw_action_row_set_subtitle(ADW_ACTION_ROW(theme_row), info5(
        "Switch between System, Light, or Dark theme.",
        "Alternar entre tema Sistema, Claro ou Escuro.",
        "Cambiar entre tema Sistema, Claro u Oscuro.",
        "\u041f\u0435\u0440\u0435\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435 \u043c\u0435\u0436\u0434\u0443 \u0441\u0438\u0441\u0442\u0435\u043c\u043d\u043e\u0439, \u0441\u0432\u0435\u0442\u043b\u043e\u0439 \u0438 \u0442\u0435\u043c\u043d\u043e\u0439 \u0442\u0435\u043c\u043e\u0439.",
        "\u5728\u7cfb\u7edf\u3001\u6d45\u8272\u6216\u6df1\u8272\u4e3b\u9898\u4e4b\u95f4\u5207\u6362\u3002"));

    GtkStringList *theme_model = gtk_string_list_new(NULL);
    gtk_string_list_append(theme_model, _("Follow System Theme"));
    gtk_string_list_append(theme_model, _("Light Theme"));
    gtk_string_list_append(theme_model, _("Dark Theme"));

    GtkWidget *theme_dropdown = gtk_drop_down_new(G_LIST_MODEL(theme_model), NULL);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(theme_dropdown), current_theme);
    g_signal_connect(theme_dropdown, "notify::selected", G_CALLBACK(on_prefs_theme_selected), NULL);
    adw_action_row_add_suffix(ADW_ACTION_ROW(theme_row), theme_dropdown);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(group), theme_row);

    /* Size constraints: wrap page in a scrolled window */
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_size_request(scroll, 480, 480);
    gtk_scrolled_window_set_max_content_width(GTK_SCROLLED_WINDOW(scroll), 540);
    gtk_scrolled_window_set_max_content_height(GTK_SCROLLED_WINDOW(scroll), 800);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), page);

    adw_alert_dialog_set_extra_child(dialog, scroll);
    adw_alert_dialog_add_responses(dialog, "close", _("Close"), NULL);
    adw_alert_dialog_set_default_response(dialog, "close");
    adw_alert_dialog_set_close_response(dialog, "close");
    adw_dialog_present(ADW_DIALOG(dialog), app_data.window);
}
