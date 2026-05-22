#include "blufixer.h"

/* fix state helpers */
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

void update_actions_page_state(void) {
    const char *mfr = detect_manufacturer(app_data.selected_vendor, app_data.selected_product, app_data.selected_desc);
    gboolean is_realtek = g_strcmp0(mfr, "Realtek") == 0;
    gtk_widget_set_visible(app_data.row_realtek, is_realtek);

    gtk_widget_set_visible(app_data.row_csr, TRUE);

    gboolean is_broadcom = g_strcmp0(mfr, "Broadcom / Cypress") == 0;
    gtk_widget_set_visible(app_data.row_broadcom, is_broadcom);

    if (csr_is_active()) {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_csr), _("Revert"));
        gtk_widget_remove_css_class(app_data.btn_csr, "suggested-action");
        gtk_widget_add_css_class(app_data.btn_csr, "destructive-action");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_csr), _("Apply"));
        gtk_widget_remove_css_class(app_data.btn_csr, "destructive-action");
        gtk_widget_add_css_class(app_data.btn_csr, "suggested-action");
    }

    if (ertm_is_active()) {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_ertm), _("Revert"));
        gtk_widget_remove_css_class(app_data.btn_ertm, "suggested-action");
        gtk_widget_add_css_class(app_data.btn_ertm, "destructive-action");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_ertm), _("Apply"));
        gtk_widget_remove_css_class(app_data.btn_ertm, "destructive-action");
        gtk_widget_add_css_class(app_data.btn_ertm, "suggested-action");
    }

    if (realtek_is_active()) {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_realtek), _("Remove"));
        gtk_widget_remove_css_class(app_data.btn_realtek, "suggested-action");
        gtk_widget_add_css_class(app_data.btn_realtek, "destructive-action");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_realtek), _("Install"));
        gtk_widget_remove_css_class(app_data.btn_realtek, "destructive-action");
        gtk_widget_add_css_class(app_data.btn_realtek, "suggested-action");
    }

    if (broadcom_fw_is_active()) {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_broadcom), _("Installed"));
        gtk_widget_add_css_class(app_data.btn_broadcom, "flat");
        gtk_widget_remove_css_class(app_data.btn_broadcom, "suggested-action");
        gtk_widget_remove_css_class(app_data.btn_broadcom, "destructive-action");
        gtk_widget_set_sensitive(app_data.btn_broadcom, FALSE);
    } else {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_broadcom), _("Install"));
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
        gtk_label_set_text(GTK_LABEL(app_data.lbl_legacy), _("Revert"));
        gtk_widget_remove_css_class(app_data.btn_legacy, "suggested-action");
        gtk_widget_add_css_class(app_data.btn_legacy, "destructive-action");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_legacy), _("Apply"));
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

static gboolean on_command_finished(gpointer user_data) {
    CommandContext *ctx = (CommandContext *)user_data;
    if (!gtk_widget_get_realized(app_data.window)) {
        g_free(ctx->error_detail);
        g_free(ctx);
        return G_SOURCE_REMOVE;
    }
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
        last_error_detail = ctx->error_detail;
        ctx->error_detail = NULL;

        AdwToast *toast = adw_toast_new(ctx->error_msg);
        adw_toast_set_button_label(toast, _("View details"));
        adw_toast_set_action_name(toast, "win.show_error_detail");
        adw_toast_set_priority(toast, ADW_TOAST_PRIORITY_HIGH);
        adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay), toast);
    }

    g_free(ctx);
    return G_SOURCE_REMOVE;
}

static gpointer command_thread_func(gpointer user_data) {
    CommandContext *ctx = (CommandContext *)user_data;
    g_autoptr(GError) error = NULL;
    g_autoptr(GSubprocessLauncher) launcher = g_subprocess_launcher_new(G_SUBPROCESS_FLAGS_NONE);
    g_autoptr(GSubprocess) proc = g_subprocess_launcher_spawnv(launcher, (const char *[]){"sh", "-c", ctx->command, NULL}, &error);
    if (proc == NULL) {
        ctx->status_code = -1;
        ctx->error_detail = g_strdup_printf(
            "%s: %s\n\n%s: %d\n\n%s",
            info5("Command", "Comando", "Comando",
                  "\u041a\u043e\u043c\u0430\u043d\u0434\u0430", "\u547d\u4ee4"),
            ctx->command,
            info5("Exit code", "C\u00f3digo de sa\u00edda",
                  "C\u00f3digo de salida",
                  "\u041a\u043e\u0434 \u0432\u044b\u0445\u043e\u0434\u0430",
                  "\u9000\u51fa\u4ee3\u7801"),
            ctx->status_code,
            info5("Failed to spawn process",
                  "Falha ao iniciar o processo",
                  "Fallo al iniciar el proceso",
                  "\u041d\u0435 \u0443\u0434\u0430\u043b\u043e\u0441\u044c \u0437\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u044c \u043f\u0440\u043e\u0446\u0435\u0441\u0441",
                  "\u65e0\u6cd5\u542f\u52a8\u8fdb\u7a0b"));
        g_warning("command_thread_func: failed to spawn: %s", error ? error->message : "unknown error");
    } else {
        g_subprocess_wait(proc, NULL, &error);
        ctx->status_code = g_subprocess_get_exit_status(proc);
        if (ctx->status_code != 0) {
            ctx->error_detail = g_strdup_printf(
                "%s: %s\n\n%s: %d\n\n%s",
                info5("Command", "Comando", "Comando",
                      "\u041a\u043e\u043c\u0430\u043d\u0434\u0430", "\u547d\u4ee4"),
                ctx->command,
                info5("Exit code", "C\u00f3digo de sa\u00edda",
                      "C\u00f3digo de salida",
                      "\u041a\u043e\u0434 \u0432\u044b\u0445\u043e\u0434\u0430",
                      "\u9000\u51fa\u4ee3\u7801"),
                ctx->status_code,
                info5("An error occurred during execution of the above command. "
                      "Check if the Bluetooth service is active and if you "
                      "have superuser permissions.",
                      "Ocorreu um erro durante a execu\u00e7\u00e3o do comando acima. "
                      "Verifique se o servi\u00e7o Bluetooth est\u00e1 ativo e se voc\u00ea "
                      "possui permiss\u00f5es de superusu\u00e1rio.",
                      "Se produjo un error durante la ejecuci\u00f3n del comando anterior. "
                      "Verifique si el servicio Bluetooth est\u00e1 activo y si tiene "
                      "permisos de superusuario.",
                      "\u041f\u0440\u043e\u0438\u0437\u043e\u0448\u043b\u0430 \u043e\u0448\u0438\u0431\u043a\u0430 \u043f\u0440\u0438 \u0432\u044b\u043f\u043e\u043b\u043d\u0435\u043d\u0438\u0438 \u0432\u044b\u0448\u0435\u0443\u043a\u0430\u0437\u0430\u043d\u043d\u043e\u0439 \u043a\u043e\u043c\u0430\u043d\u0434\u044b. "
                      "\u041f\u0440\u043e\u0432\u0435\u0440\u044c\u0442\u0435, \u0430\u043a\u0442\u0438\u0432\u043d\u0430 \u043b\u0438 \u0441\u043b\u0443\u0436\u0431\u0430 Bluetooth \u0438 \u0435\u0441\u0442\u044c \u043b\u0438 \u0443 \u0432\u0430\u0441 \u043f\u0440\u0430\u0432\u0430 \u0441\u0443\u043f\u0435\u0440\u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u0435\u043b\u044f.",
                      "\u6267\u884c\u4e0a\u8ff0\u547d\u4ee4\u65f6\u53d1\u751f\u9519\u8bef\u3002\u8bf7\u68c0\u67e5\u84dd\u7259\u670d\u52a1\u662f\u5426\u5904\u4e8e\u6d3b\u52a8\u72b6\u6001\uff0c\u4ee5\u53ca\u60a8\u662f\u5426\u5177\u6709\u8d85\u7ea7\u7528\u6237\u6743\u9650\u3002"));
        }
    }
    g_idle_add(on_command_finished, ctx);
    return NULL;
}

void launch_async_action(GtkWidget *button, const char *cmd,
    const char *success, const char *error,
    gboolean (*is_active)(void), gboolean expect_active)
{
    if (!gtk_widget_is_sensitive(button)) return;
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

void run_fix_csr(void) {
    if (!app_data.btn_csr) return;
    char cmd[4096];
    if (csr_is_active()) {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'rm -f /etc/modprobe.d/btusb.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(app_data.btn_csr, cmd, _("CSR Power config reverted!"), _("Error reverting."), csr_is_active, FALSE);
    } else {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'echo \"options btusb disable_scatternet=1 force_load_firmware=1 enable_autosuspend=0\" > /etc/modprobe.d/btusb.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(app_data.btn_csr, cmd, _("CSR Power config applied!"), _("Error applying."), csr_is_active, TRUE);
    }
}

void run_fix_ertm(void) {
    if (!app_data.btn_ertm) return;
    char cmd[4096];
    if (ertm_is_active()) {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'rm -f /etc/modprobe.d/bluetooth-ertm.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(app_data.btn_ertm, cmd, _("ERTM re-enabled!"), _("Error reverting."), ertm_is_active, FALSE);
    } else {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'echo \"options bluetooth disable_ertm=1\" > /etc/modprobe.d/bluetooth-ertm.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(app_data.btn_ertm, cmd, _("ERTM disabled!"), _("Error disabling ERTM."), ertm_is_active, TRUE);
    }
}

void run_fix_realtek(void) {
    if (!app_data.btn_realtek) return;
    char cmd[4096];
    g_autofree char *fw_check = g_strdup_printf("%s/rtl_bt/rtl8761b_fw.bin", fw_path);

    if (realtek_is_active()) {
        g_snprintf(cmd, sizeof(cmd),
            "%s bash -c 'rm -f %s/rtl_bt/rtl8761b_fw.bin %s/rtl_bt/rtl8761b_config.bin && modprobe -r btusb && modprobe btusb'",
            priv_cmd, fw_path, fw_path);
        launch_async_action(app_data.btn_realtek, cmd, _("Realtek firmware removed!"), _("Error removing."), realtek_is_active, FALSE);
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
        launch_async_action(app_data.btn_realtek, cmd, _("Realtek firmware installed!"), _("Download error."), realtek_is_active, TRUE);
    }
}

void run_fix_legacy(void) {
    if (!app_data.btn_legacy) return;
    char cmd[4096];
    g_autofree char *contents = NULL;
    gboolean is_legacy = FALSE;

    if (g_file_get_contents("/etc/bluetooth/main.conf", &contents, NULL, NULL))
        is_legacy = g_strrstr(contents, "Security=legacy") != NULL;

    if (is_legacy) {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'sed -i \"s/^Security=legacy/#Security=ssp/\" /etc/bluetooth/main.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(app_data.btn_legacy, cmd, _("Legacy pairing disabled!"), _("Error reverting."), legacy_is_active, FALSE);
    } else {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'sed -i \"/^[[:space:]]*Security=/d\" /etc/bluetooth/main.conf && echo \"Security=legacy\" >> /etc/bluetooth/main.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(app_data.btn_legacy, cmd, _("Legacy pairing enabled!"), _("Error applying."), legacy_is_active, TRUE);
    }
}

void run_fix_broadcom(void) {
    if (!app_data.btn_broadcom) return;
    if (broadcom_fw_is_active()) return;

    char cmd[4096];
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
            adw_toast_new(_("Package manager not recognized.")));
        return;
    }

    g_snprintf(cmd, sizeof(cmd),
        "%s bash -c '%s %s && modprobe -r b43 2>/dev/null; modprobe b43'",
        priv_cmd, pkg_mgr, pkgs);
    launch_async_action(app_data.btn_broadcom, cmd, _("Broadcom firmware installed!"), _("Error installing firmware."), broadcom_fw_is_active, TRUE);
}

void run_fix_rfkill(void) {
    if (!app_data.btn_rfkill) return;
    char cmd[4096];
    g_snprintf(cmd, sizeof(cmd), "%s bash -c 'rfkill unblock bluetooth && bluetoothctl power on'", priv_cmd);
    launch_async_action(app_data.btn_rfkill, cmd, _("Antenna unlocked!"), _("Execution error."), NULL, FALSE);
}

void run_restart_service(void) {
    if (!app_data.btn_restart) return;
    char cmd[4096];
    g_snprintf(cmd, sizeof(cmd), "%s %s", priv_cmd, restart_cmd);
    launch_async_action(app_data.btn_restart, cmd, _("Service restarted!"), _("Error restarting."), NULL, FALSE);
}

void run_fix_perm(void) {
    if (!app_data.btn_perm) return;
    char cmd[4096];
    const char *user = g_get_user_name();
    g_autofree char *quoted_user = g_shell_quote(user);
    g_snprintf(cmd, sizeof(cmd), "%s usermod -aG lp %s", priv_cmd, quoted_user);
    launch_async_action(app_data.btn_perm, cmd, _("Added to lp group! Restart session."), _("Error adding permissions."), NULL, FALSE);
}

void run_fix_cache(void) {
    if (!app_data.btn_cache) return;
    if (!g_file_test("/var/lib/bluetooth", G_FILE_TEST_IS_DIR)) {
        adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay),
            adw_toast_new(_("/var/lib/bluetooth not found")));
        return;
    }
    char cmd[4096];
    g_snprintf(cmd, sizeof(cmd), "%s bash -c '%s && rm -rf /var/lib/bluetooth/*/* && %s'", priv_cmd, stop_cmd, start_cmd);
    launch_async_action(app_data.btn_cache, cmd, _("Device cache cleared!"), _("Error clearing cache."), NULL, FALSE);
}
