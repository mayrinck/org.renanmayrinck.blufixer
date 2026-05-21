#include "blufixer.h"

char priv_cmd[512]  = "pkexec";
char dl_cmd[16]      = "wget";
char restart_cmd[64] = "systemctl restart bluetooth";
char stop_cmd[64]    = "systemctl stop bluetooth";
char start_cmd[64]   = "systemctl start bluetooth";
char fw_path[64]     = "/lib/firmware";
gboolean has_elevation = TRUE;
gboolean in_flatpak = FALSE;
char askpass_path[128] = "";
char last_error_detail[4096] = "";

gboolean tool_available(const char *name) {
    if (in_flatpak) {
        g_autofree char *cmd = g_strdup_printf("flatpak-spawn --host which %s >/dev/null 2>&1", name);
        return system(cmd) == 0;
    }
    return g_find_program_in_path(name) != NULL;
}

static void cleanup_temp_files(void) {
    if (askpass_path[0]) {
        int ret = unlink(askpass_path);
        (void)ret;
    }
}

void detect_system_tools(void) {
    g_autofree gchar *path = NULL;

    in_flatpak = (g_getenv("FLATPAK_ID") != NULL);

    if (in_flatpak) {
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
        } else if (tool_available("service")) {
            g_strlcpy(restart_cmd, "service bluetooth restart", sizeof(restart_cmd));
            g_strlcpy(stop_cmd, "service bluetooth stop", sizeof(stop_cmd));
            g_strlcpy(start_cmd, "service bluetooth start", sizeof(start_cmd));
        }

        if (g_file_test("/usr/lib/firmware", G_FILE_TEST_IS_DIR))
            g_strlcpy(fw_path, "/usr/lib/firmware", sizeof(fw_path));

        return;
    }

    path = g_find_program_in_path("pkexec");
    if (path) {
        g_strlcpy(priv_cmd, "pkexec", sizeof(priv_cmd));
        has_elevation = TRUE;
    } else {
        path = g_find_program_in_path("sudo");
        if (path) {
            const char *askpass_tests[] = {"zenity", "/usr/libexec/ssh-askpass",
                                           "/usr/lib/ssh/ssh-askpass", "lxqt-sudo", NULL};
            has_elevation = FALSE;
            for (int i = 0; askpass_tests[i]; i++) {
                g_autofree gchar *ap = g_find_program_in_path(askpass_tests[i]);
                if (!ap) continue;
                if (g_strrstr(ap, "zenity")) {
                    g_snprintf(askpass_path, sizeof(askpass_path),
                        "/tmp/blufixer-askpass-XXXXXX.sh");
                    int fd = g_mkstemp(askpass_path);
                    if (fd >= 0) {
                        g_autofree char *prompt = g_strdup_printf(
                            "#!/bin/sh\nexec zenity --password --title=\"BluFixer\" "
                            "--text=\"%s\"\n",
                            _("Enter password for Bluetooth fixes:"));
                        if (write(fd, prompt, strlen(prompt)) == (ssize_t)strlen(prompt) &&
                            fchmod(fd, 0755) == 0) {
                            g_snprintf(priv_cmd, sizeof(priv_cmd),
                                "SUDO_ASKPASS=%s sudo -A", askpass_path);
                            has_elevation = TRUE;
                        }
                        close(fd);
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
