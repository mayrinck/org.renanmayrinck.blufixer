#ifndef BLUFIXER_H
#define BLUFIXER_H

#include <gtk/gtk.h>
#include <adwaita.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define APP_VERSION "1.6.0"

#define LANG_CODE_EN    "en"
#define LANG_CODE_PT    "pt_BR"
#define LANG_CODE_ES    "es"
#define LANG_CODE_RU    "ru"
#define LANG_CODE_ZH    "zh_CN"

typedef enum {
    LANG_SYS = -1,
    LANG_EN  = 0,
    LANG_PT  = 1,
    LANG_ES  = 2,
    LANG_RU  = 3,
    LANG_ZH  = 4,
} LangId;

typedef enum {
    THEME_SYSTEM = 0,
    THEME_LIGHT  = 1,
    THEME_DARK   = 2,
} ThemeId;

typedef struct { const char *en, *pt, *es, *ru, *zh; } TransEntry;

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

typedef struct {
    char vendor[8];
    char product[8];
    char *desc;
    char manufacturer[32];
} ScanDevice;

typedef struct {
    GtkWidget *update_btn;
    gboolean success;
    char latest_version[32];
} UpdateCheckData;

/* Global app data */
typedef struct {
    GtkWidget *window;
    GtkApplication *app;
    GtkWidget *toast_overlay;
    GtkWidget *view_stack;
    GtkWidget *devices_group;
    GtkWidget *status_group;
    GtkWidget *fixes_group;
    GtkWidget *actions_group;
    GtkWidget *back_button;
    GtkWidget *update_btn;
    GMenu     *main_menu;
    GMenu     *lang_section;
    GMenu     *theme_section;
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

    GtkWidget *row_tech_name;
    GtkWidget *row_tech_vendor;
    GtkWidget *row_tech_id;
    GtkWidget *row_tech_version;
} AppData;

extern AppData app_data;

extern LangId current_lang;
extern ThemeId current_theme;
extern GList *dynamic_rows;
extern GtkWidget *scan_loader_row;
extern gint scanning;
extern guint scan_timeout_id;
extern char priv_cmd[512];
extern char dl_cmd[16];
extern char restart_cmd[64];
extern char stop_cmd[64];
extern char start_cmd[64];
extern char fw_path[64];
extern gboolean has_elevation;
extern gboolean in_flatpak;
extern char askpass_path[128];
extern char last_error_detail[4096];

/* i18n.c */
const char* _(const char *en);
const char* info5(const char *en, const char *pt, const char *es, const char *ru, const char *zh);
LangId lang_from_str(const char *s);
const char* lang_code(LangId id);
const char* lang_label(LangId id);
void detect_language(void);
void rebuild_language_menu(void);
void set_language(LangId lang);

/* theme.c (inline in main) */
void detect_theme(void);
void rebuild_theme_menu(void);
void set_theme(ThemeId t);

/* dialogs.c */
void show_info_dialog(const char *body_markup);
const char* dialog_color(void);
void show_info_markup(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void on_info_copy_activated(AdwActionRow *row, gpointer user_data);
void on_info_csr_clicked(GtkButton *btn, gpointer user_data);
void on_info_ertm_clicked(GtkButton *btn, gpointer user_data);
void on_info_realtek_clicked(GtkButton *btn, gpointer user_data);
void on_info_rfkill_clicked(GtkButton *btn, gpointer user_data);
void on_info_restart_clicked(GtkButton *btn, gpointer user_data);
void on_info_legacy_clicked(GtkButton *btn, gpointer user_data);
void on_info_broadcom_clicked(GtkButton *btn, gpointer user_data);
void on_info_perm_clicked(GtkButton *btn, gpointer user_data);
void on_info_cache_clicked(GtkButton *btn, gpointer user_data);

/* scan.c */
const char* detect_manufacturer(const char *vendor, const char *product, const char *desc);
void scan_bluetooth_devices(void);
void scan_device_free(gpointer data);

/* fixes.c */
void update_actions_page_state(void);
void launch_async_action(GtkWidget *button, const char *cmd,
    const char *success, const char *error,
    gboolean (*is_active)(void), gboolean expect_active);
void run_fix_csr(void);
void run_fix_ertm(void);
void run_fix_realtek(void);
void run_fix_legacy(void);
void run_fix_broadcom(void);
void run_fix_rfkill(void);
void run_fix_perm(void);
void run_fix_cache(void);
void run_restart_service(void);

/* version.c */
int compare_versions(const char *v1, const char *v2);
void query_bluetooth_version(void);
void check_for_updates(GtkWidget *update_btn);

/* platform.c */
gboolean tool_available(const char *name);
void detect_system_tools(void);

/* ui_helpers.c */
void init_custom_styles(void);
GtkWidget* create_brand_badge(const char *brand);
GtkWidget* create_action_row_button(const char *label_txt, GCallback cb, GtkWidget **out_spinner, GtkWidget **out_lbl);
GtkWidget* create_menu_button(void);

/* main.c (callbacks referenced from other modules) */
void on_device_selected(GtkButton *btn, gpointer user_data);
void on_scan_clicked(GtkButton *btn, gpointer user_data);
gboolean delayed_scan(gpointer user_data);

#endif
