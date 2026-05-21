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

static LangId current_lang = LANG_SYS;

typedef enum {
    THEME_SYSTEM = 0,
    THEME_LIGHT  = 1,
    THEME_DARK   = 2,
} ThemeId;

static ThemeId current_theme = THEME_SYSTEM;

typedef struct { const char *en, *pt, *es, *ru, *zh; } TransEntry;

static const TransEntry trans_table[] = {
    /* Window, menu, general UI */
    {"BluFixer",                             "BluFixer",                         "BluFixer",                         "BluFixer",                         "蓝修复"},
    {"About",                                "Sobre",                            "Acerca",                           "О программе",                      "关于"},
    {"GitHub Repository",                    "Repositório no GitHub",            "Repositorio en GitHub",            "Репозиторий на GitHub",            "GitHub仓库"},
    {"Donate",                               "Doar ao autor",                    "Donar al autor",                   "Пожертвовать",                     "捐赠"},
    {"Language",                             "Idioma",                           "Idioma",                           "Язык",                             "语言"},
    {"Scan",                                 "Escanear",                         "Escanear",                         "Сканировать",                      "扫描"},
    {"Scanning devices...",                  "Escaneando dispositivos...",       "Escaneando dispositivos...",       "Сканирование устройств...",        "正在扫描设备..."},
    {"No Bluetooth devices connected or functional at the moment",
                                             "Não há dispositivos bluetooth conectados ou funcionais no momento",
                                                                                "No hay dispositivos Bluetooth conectados o funcionales",
                                                                                                                    "Нет подключенных или работающих Bluetooth-устройств",
                                                                                                                                                        "目前没有连接或可用的蓝牙设备"},
    {"Select",                               "Selecionar",                       "Seleccionar",                      "Выбрать",                          "选择"},

    /* Device tech sheet rows */
    {"Device Name",                          "Nome do dispositivo",              "Nombre del dispositivo",           "Имя устройства",                   "设备名称"},
    {"Possible Manufacturer",                "Possível fabricante",              "Posible fabricante",               "Возможный производитель",           "可能的制造商"},
    {"Hardware ID",                          "Identificador de Hardware (ID)",   "Identificador de Hardware (ID)",   "Идентификатор оборудования (ID)",  "硬件ID"},
    {"Bluetooth",                            "Bluetooth",                         "Bluetooth",                         "Bluetooth",                         "蓝牙"},
    {"Bluetooth Version",                    "Versão do Bluetooth",               "Versión de Bluetooth",              "Версия Bluetooth",                  "蓝牙版本"},
    {"Unknown",                              "Desconhecida",                     "Desconocida",                      "Неизвестно",                       "未知"},

    /* Device list section */
    {"Device List",                          "Lista de adaptadores",             "Lista de adaptadores",             "Список адаптеров",                 "设备列表"},
    {"Select the adapter you want to modify","Selecione o adaptador que deseja modificar",
                                                                                "Seleccione el adaptador a modificar",
                                                                                                                    "Выберите адаптер для изменения",   "选择要修改的适配器"},

    /* Fix/Action section titles */
    {"System Fixes",                         "Correções de Sistema",             "Correcciones del Sistema",         "Исправления системы",              "系统修复"},
    {"Immediate Actions",                    "Ações Imediatas",                  "Acciones Inmediatas",              "Немедленные действия",             "立即操作"},
    {"Applies configuration file changes or driver/firmware downloads. Some settings are hardware-specific and all can be reverted.",
                                              "Executa modificações de arquivos de configuração ou download de drivers e firmware, se necessário. Em alguns casos as configurações serão específicas para o hardware e todas podem ser revertidas.",
                                                                                "Realiza modificaciones de archivos de configuración o descarga de controladores y firmware si es necesario. Algunas configuraciones son específicas del hardware y todas pueden revertirse.",
                                                                                                                    "Применяет изменения конфигурационных файлов или загрузку драйверов/прошивок. Некоторые настройки специфичны для оборудования и все могут быть отменены.",
                                                                                                                                                        "应用配置文件修改或驱动/固件下载。某些设置是硬件特定的，并且全部可以还原。"},
    {"Direct commands applied to hardware or system services. Cannot be reverted (only repeated).",
                                              "Comandos diretos aplicados ao hardware ou serviços do sistema. Não podem ser revertidos (apenas repetidos).",
                                                                                "Comandos directos aplicados al hardware o servicios del sistema. No se pueden revertir (solo repetir).",
                                                                                                                    "Прямые команды к оборудованию или системным службам. Не могут быть отменены (только повторены).",
                                                                                                                                                        "直接应用于硬件或系统服务的命令。不能撤销（只能重复）。"},

    /* Fix row titles/subtitles */
    {"CSR Energy Fix",                       "Corre\u00e7\u00e3o de Energia CSR",           "Corrección de energía CSR",            "Исправление энергопотребления CSR","CSR能源修复"},
    {"Recommended for CSR and Barrot/Generic dongles. Stabilizes the radio to fix detection and connection loops.",
                                              "Recomendada para dispositivos CSR e Barrot/Gen\u00e9ricos. Estabiliza o r\u00e1dio do dongle para resolver loops de detec\u00e7\u00e3o e conex\u00e3o.",
                                                                                "Recomendada para dispositivos CSR y Barrot/Genéricos. Estabiliza la radio para resolver bucles de detección y conexión.",
                                                                                                                    "Рекомендуется для устройств CSR и Barrot/Generic. Стабилизирует радио для устранения циклов обнаружения и подключения.",
                                                                                                                                                        "推荐用于CSR和Barrot/通用设备。稳定无线电以解决检测和连接循环。"},
    {"Disable ERTM for Gamepads",           "Desativar ERTM para Gamepads",      "Desactivar ERTM para Gamepads",    "Отключить ERTM для геймпадов",     "为手柄禁用ERTM"},
    {"Fixes automatic disconnections of modern Bluetooth gamepads.",
                                             "Resolve desconexões automáticas de gamepads Bluetooth modernos.",
                                                                                "Resuelve desconexiones automáticas de gamepads Bluetooth modernos.",
                                                                                                                    "Исправляет автоматические отключения современных Bluetooth-геймпадов.",
                                                                                                                                                        "修复现代蓝牙手柄的自动断开连接问题。"},
    {"Force Legacy Pairing Mode",           "Forçar pareamento em modo legado",  "Forzar emparejamiento en modo legado",
                                                                                                                    "Принудительный режим устаревшего сопряжения",
                                                                                                                                                        "强制传统配对模式"},
    {"Allows old Bluetooth devices to pair with a fixed PIN.",
                                             "Permite que dispositivos Bluetooth antigos pareiem com PIN fixo.",
                                                                                "Permite que dispositivos Bluetooth antiguos se emparejen con PIN fijo.",
                                                                                                                    "Позволяет старым Bluetooth-устройствам сопрягаться с фиксированным PIN-кодом.",
                                                                                                                                                        "允许旧的蓝牙设备使用固定PIN配对。"},
    {"Install Realtek Firmware (RTL8761B)",  "Instalar Firmware Realtek (RTL8761B)",
                                                                                "Instalar firmware Realtek (RTL8761B)",
                                                                                                                    "Установить прошивку Realtek (RTL8761B)",
                                                                                                                                                        "安装Realtek固件(RTL8761B)"},
    {"Downloads and injects the missing official driver binaries.",
                                             "Baixa e injeta os binários oficiais ausentes do driver.",
                                                                                "Descarga e inyecta los binarios oficiales faltantes del controlador.",
                                                                                                                    "Загружает и устанавливает отсутствующие официальные бинарные файлы драйвера.",
                                                                                                                                                        "下载并注入缺失的官方驱动二进制文件。"},
    {"Install Broadcom/Cypress Firmware (b43)",
                                             "Instalar Firmware Broadcom/Cypress (b43)",
                                                                                "Instalar firmware Broadcom/Cypress (b43)",
                                                                                                                    "Установить прошивку Broadcom/Cypress (b43)",
                                                                                                                                                        "安装Broadcom/Cypress固件(b43)"},
    {"Downloads and extracts the missing proprietary firmware for Broadcom chipsets.",
                                             "Baixa e extrai o firmware proprietário ausente para chipsets Broadcom.",
                                                                                "Descarga y extrae el firmware propietario faltante para chipsets Broadcom.",
                                                                                                                    "Загружает и извлекает отсутствующую проприетарную прошивку для чипсетов Broadcom.",
                                                                                                                                                        "下载并提取Broadcom芯片组缺失的专有固件。"},
    {"Unblock Antenna",                      "Descongestionar",                  "Desbloquear antena",               "Разблокировать антенну",           "解锁天线"},
    {"Forces activation of adapters stuck in Airplane Mode.",
                                             "Força a ativação de adaptadores presos no 'Modo Avião'.",
                                                                                "Fuerza la activación de adaptadores atascados en 'Modo Avión'.",
                                                                                                                    "Принудительно включает адаптеры, заблокированные в 'Режиме полёта'.",
                                                                                                                                                        "强制激活卡在飞行模式的适配器。"},
    {"Add user permissions",                 "Adicionar permissões ao usuário atual",
                                                                                "Agregar permisos al usuario actual",
                                                                                                                    "Добавить разрешения текущему пользователю",
                                                                                                                                                        "添加当前用户权限"},
    {"Adds the user to the lp group for Bluetooth D-Bus access.",
                                             "Adiciona o usuário ao grupo lp para acesso ao D-Bus do Bluetooth.",
                                                                                "Agrega el usuario al grupo lp para acceso al D-Bus de Bluetooth.",
                                                                                                                    "Добавляет пользователя в группу lp для доступа к D-Bus Bluetooth.",
                                                                                                                                                        "将用户添加到lp组以获得蓝牙D-Bus访问权限。"},
    {"Clear device cache",                   "Limpar o cache de dispositivos",   "Limpiar caché de dispositivos",    "Очистить кэш устройств",           "清除设备缓存"},
    {"Removes all pairing caches to fix connection errors.",
                                             "Remove o cache de pareamento de todos os dispositivos para resolver erros de conexão.",
                                                                                "Elimina el caché de emparejamiento de todos los dispositivos para resolver errores de conexión.",
                                                                                                                    "Удаляет кэш сопряжения всех устройств для исправления ошибок подключения.",
                                                                                                                                                        "清除所有配对缓存以修复连接错误。"},
    {"Restart Bluetooth Service",           "Reiniciar Serviço Bluetooth",       "Reiniciar Servicio Bluetooth",     "Перезапустить службу Bluetooth",   "重启蓝牙服务"},
    {"Clears caches and buffers by restarting the system service.",
                                             "Limpa caches e buffers reiniciando o serviço do sistema.",
                                                                                "Limpia cachés y búferes reiniciando el servicio del sistema.",
                                                                                                                    "Очищает кэши и буферы перезапуском системной службы.",
                                                                                                                                                        "通过重启系统服务清除缓存和缓冲区。"},

    /* Button labels */
    {"Apply",                                "Aplicar",                          "Aplicar",                          "Применить",                        "应用"},
    {"Revert",                               "Reverter",                         "Revertir",                         "Отменить",                         "还原"},
    {"Install",                              "Instalar",                         "Instalar",                         "Установить",                       "安装"},
    {"Remove",                               "Remover",                          "Remover",                          "Удалить",                          "移除"},
    {"Installed",                            "Instalado",                        "Instalado",                        "Установлено",                      "已安装"},
    {"Run",                                  "Executar",                         "Ejecutar",                         "Выполнить",                        "执行"},
    {"Got it",                               "Entendido",                        "Entendido",                        "Понятно",                          "知道了"},
    {"Close",                                "Fechar",                           "Cerrar",                           "Закрыть",                          "关闭"},
    {"View details",                         "Ver detalhes",                     "Ver detalles",                     "Подробнее",                        "查看详情"},
    {"Copied!",                              "Copiado!",                         "¡Copiado!",                        "Скопировано!",                     "已复制!"},

    /* Info dialog titles */
    {"Technical Details",                    "Detalhamento Técnico",             "Detalles Técnicos",                "Технические детали",               "技术细节"},
    {"Operation failed",                     "Falha na operação",                "Fallo en la operación",            "Ошибка операции",                   "操作失败"},
    {"Device Name copied",                   "Nome completo copiado",            "Nombre completo copiado",          "Имя устройства скопировано",        "设备名称已复制"},
    {"Manufacturer copied",                  "Nome do fabricante copiado",       "Nombre del fabricante copiado",    "Производитель скопирован",          "制造商已复制"},
    {"Hardware ID: %s:%s",                   "ID de Hardware: %s:%s",             "ID de Hardware: %s:%s",             "ID оборудования: %s:%s",            "硬件ID: %s:%s"},
    {"Hardware ID copied",                   "Hardware ID copiado",              "ID de Hardware copiado",           "ID оборудования скопирован",        "硬件ID已复制"},
    {"Device Tech Sheet",                    "Ficha Técnica do Adaptador",       "Ficha Técnica del Adaptador",      "Технические характеристики",        "设备技术参数"},
    {"The possible manufacturer name of the board on the device, not the retail brand.",
                                             "O nome do possível fabricante da placa no dispositivo, não representa a marca de venda na embalagem",
                                                                                 "El nombre del posible fabricante de la placa en el dispositivo, no representa la marca de venta en el empaque.",
                                                                                                                     "Возможное имя производителя платы устройства, не является розничным брендом.",
                                                                                                                                                         "设备主板的可能制造商名称，不代表零售品牌。"},
    {"Bluetooth version is detected via bluetoothctl, btmgmt or hciconfig.",
                                             "A versão Bluetooth é detectada via bluetoothctl, btmgmt ou hciconfig",
                                                                                 "La versión Bluetooth se detecta mediante bluetoothctl, btmgmt o hciconfig.",
                                                                                                                     "Версия Bluetooth определяется через bluetoothctl, btmgmt или hciconfig.",
                                                                                                                                                         "蓝牙版本通过bluetoothctl、btmgmt或hciconfig检测。"},

    /* Toast messages - fixes */
    {"CSR Power config reverted!",           "Correção de energia revertida!",   "¡Configuración CSR revertida!",    "Конфигурация CSR отменена!",        "CSR配置已还原!"},
    {"CSR Power config applied!",            "Correção de energia aplicada!",    "¡Corrección CSR aplicada!",        "Исправление CSR применено!",        "CSR修复已应用!"},
    {"ERTM re-enabled!",                     "ERTM ativado!",                    "¡ERTM reactivado con éxito!",      "ERTM повторно включён!",            "ERTM已重新启用!"},
    {"ERTM disabled!",                       "ERTM desativado!",                 "ERTM desactivado!",                "ERTM отключён!",                    "ERTM已禁用!"},
    {"Realtek firmware removed!",            "Firmware Realtek removido!",       "¡Firmware Realtek eliminado!",     "Прошивка Realtek удалена!",         "Realtek固件已移除!"},
    {"Realtek firmware installed!",          "Firmware Realtek instalado!",      "¡Firmware Realtek instalado!",     "Прошивка Realtek установлена!",     "Realtek固件已安装!"},
    {"Legacy pairing disabled!",             "Pareamento legado desativado!",    "¡Emparejamiento heredado desactivado!",
                                                                                                                    "Устаревшее сопряжение отключено!",  "传统配对已禁用!"},
    {"Legacy pairing enabled!",              "Pareamento legado ativado!",       "¡Emparejamiento heredado activado!",
                                                                                                                    "Устаревшее сопряжение включено!",   "传统配对已启用!"},
    {"Broadcom firmware installed!",         "Firmware Broadcom instalado!",     "¡Firmware Broadcom instalado!",    "Прошивка Broadcom установлена!",    "Broadcom固件已安装!"},
    {"Antenna unlocked!",                    "Antena desbloqueada!",              "¡Antena desbloqueada!",            "Антенна разблокирована!",           "天线已解锁!"},
    {"Service restarted!",                   "Serviço Bluetooth reiniciado!",    "¡Servicio Bluetooth reiniciado!",  "Служба Bluetooth перезапущена!",    "蓝牙服务已重启!"},
    {"Added to lp group! Restart session.",  "Adicionado ao grupo lp! Reinicie a sessão.",
                                                                                "¡Agregado al grupo lp! Reinicie la sesión.",
                                                                                                                    "Добавлен в группу lp! Перезапустите сеанс.",
                                                                                                                                                        "已添加到lp组！请重新启动会话。"},
    {"Device cache cleared!",                "Cache de dispositivos limpo!",     "¡Caché de dispositivos limpiado!", "Кэш устройств очищен!",             "设备缓存已清除!"},

    /* Toast messages - errors */
    {"Error reverting.",                     "Erro ao reverter.",                "Error al revertir.",               "Ошибка отмены.",                    "还原错误。"},
    {"Error applying.",                      "Erro ao aplicar.",                 "Error al aplicar.",                "Ошибка применения.",                "应用错误。"},
    {"Download error.",                      "Erro no download.",                "Error de descarga.",               "Ошибка загрузки.",                  "下载错误。"},
    {"Error removing.",                      "Erro ao remover.",                 "Error al eliminar.",               "Ошибка удаления.",                  "移除错误。"},
    {"Error unlocking.",                     "Erro ao desbloquear.",             "Error al desbloquear.",            "\u041e\u0448\u0438\u0431\u043a\u0430 \u0440\u0430\u0437\u0431\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u043a\u0438.",             "\u89e3\u9501\u9519\u8bef\u3002"},
    {"Error restarting.",                    "Erro ao reiniciar.",               "Error al reiniciar.",              "Ошибка перезапуска.",               "重启错误。"},
    {"Error adding permissions.",            "Erro ao adicionar.",               "Error al agregar.",                "Ошибка добавления.",                "添加错误。"},
    {"Error clearing cache.",                "Erro ao limpar cache.",            "Error al limpiar cach\u00e9.",          "Ошибка очистки кэша.",              "清除缓存错误。"},
    {"Error installing firmware.",           "Erro ao instalar firmware.",       "Error al instalar firmware.",      "Ошибка установки прошивки.",        "固件安装错误。"},
    {"Error disabling ERTM.",                "Erro ao desativar ERTM.",          "Error al desactivar ERTM.",        "\u041e\u0448\u0438\u0431\u043a\u0430 \u043e\u0442\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f ERTM.",        "禁用ERTM错误。"},
    {"Execution error.",                     "Erro ao executar.",               "Error al ejecutar.",               "Ошибка выполнения.",                "执行错误。"},
    {"Enter password for Bluetooth fixes:",   "Digite a senha para correções Bluetooth:",
                                                                                  "Ingrese la contraseña para correcciones Bluetooth:",
                                                                                                                                     "Введите пароль для исправлений Bluetooth:",
                                                                                                                                                                                     "输入蓝牙修复密码:"},
    {"Package manager not recognized.",      "Gerenciador de pacotes não reconhecido.",
                                                                                 "Gestor de paquetes no reconocido.",
                                                                                                                     "Менеджер пакетов не распознан.",    "未识别的包管理器。"},
    {"Checking for updates...",              "Verificando atualizações...",      "Buscando actualizaciones...",
                                                                                                                     "Проверка обновлений...",            "正在检查更新..."},
    {"Website",                              "Site",                             "Sitio web",                        "Веб-сайт",                          "网站"},
    {"Update to version %s",                 "Atualizar para versão %s",         "Actualizar para versión %s",       "Обновить до версии %s",             "更新到版本 %s"},
    {"Could not check for updates",          "Não foi possível verificar atualizações",
                                                                                 "No se pudo buscar actualizaciones",
                                                                                                                     "Не удалось проверить обновления",   "无法检查更新"},

    /* Misc toasts */
    {"No elevation method found (pkexec/sudo). Fixes will not work.",
                                             "Nenhum método de elevação encontrado (pkexec/sudo). As correções não funcionarão.",
                                                                                "No se encontró método de elevación (pkexec/sudo). Las correcciones no funcionarán.",
                                                                                                                    "Не найден метод повышения привилегий (pkexec/sudo). Исправления не будут работать.",
                                                                                                                                                        "未找到提权方法(pkexec/sudo)。修复将无法工作。"},
    {"Please restart the application to apply the language change.",
                                              "Reinicie o aplicativo para aplicar a alteração de idioma.",
                                                                                 "Reinicie la aplicación para aplicar el cambio de idioma.",
                                                                                                                     "Перезапустите приложение, чтобы применить изменение языка.",
                                                                                                                                                         "请重新启动应用程序以应用语言更改。"},
    {"Restart",                              "Reiniciar",                        "Reiniciar",                        "Перезапустить",                    "重启"},
    {"System Language",                      "Idioma do sistema",                "Idioma del sistema",               "Системный язык",                   "系统语言"},
    {"Legal Notice",                         "Aviso legal",                      "Aviso legal",                      "Юридическая информация",           "法律声明"},
    {"Developed by Renan Mayrinck.",         "Desenvolvido por Renan Mayrinck.", "Desarrollado por Renan Mayrinck.", "Разработано Renan Mayrinck.",      "由Renan Mayrinck开发。"},
    {"This software was developed with the assistance of artificial intelligence.",
                                             "Este software foi desenvolvido com auxílio de inteligência artificial.",
                                                                                  "Este software fue desarrollado con ayuda de inteligencia artificial.",
                                                                                                                      "Это программное обеспечение было разработано с помощью искусственного интеллекта.",
                                                                                                                                                          "本软件是在人工智能的辅助下开发的。"},
    {"Theme",                                "Tema",                               "Tema",                                "Тема",                                "主题"},
    {"Follow System Theme",                  "Usar o tema do sistema",             "Usar el tema del sistema",            "Использовать тему системы",           "跟随系统主题"},
    {"Light Theme",                          "Tema claro",                         "Tema claro",                          "Светлая тема",                        "浅色主题"},
    {"Dark Theme",                           "Tema escuro",                        "Tema oscuro",                         "Тёмная тема",                         "深色主题"},

};

static const char* _(const char *en) {
    if (current_lang == LANG_EN || current_lang == LANG_SYS) return en;
    for (size_t i = 0; i < G_N_ELEMENTS(trans_table); i++) {
        if (g_strcmp0(trans_table[i].en, en) == 0) {
            switch (current_lang) {
                case LANG_PT: if (trans_table[i].pt[0]) return trans_table[i].pt; break;
                case LANG_ES: if (trans_table[i].es[0]) return trans_table[i].es; break;
                case LANG_RU: if (trans_table[i].ru[0]) return trans_table[i].ru; break;
                case LANG_ZH: if (trans_table[i].zh[0]) return trans_table[i].zh; break;
                default: break;
            }
            return en;
        }
    }
    return en;
}

static LangId lang_from_str(const char *s) {
    if (!s || s[0] == '\0') return LANG_SYS;
    if (g_strcmp0(s, LANG_CODE_PT) == 0) return LANG_PT;
    if (g_strcmp0(s, LANG_CODE_ES) == 0) return LANG_ES;
    if (g_strcmp0(s, LANG_CODE_RU) == 0) return LANG_RU;
    if (g_strcmp0(s, LANG_CODE_ZH) == 0) return LANG_ZH;
    if (g_strcmp0(s, LANG_CODE_EN) == 0) return LANG_EN;
    return LANG_SYS;
}

static void detect_language(void) {
    current_lang = LANG_SYS;
    const char *env = g_getenv("LANG");
    if (env) {
        if (g_str_has_prefix(env, "pt")) current_lang = LANG_PT;
        else if (g_str_has_prefix(env, "es")) current_lang = LANG_ES;
        else if (g_str_has_prefix(env, "ru")) current_lang = LANG_RU;
        else if (g_str_has_prefix(env, "zh")) current_lang = LANG_ZH;
        else current_lang = LANG_EN;
    }
    g_autofree char *cfg = g_build_filename(g_get_user_config_dir(), "blufixer", "language", NULL);
    g_autofree char *saved = NULL;
    if (g_file_get_contents(cfg, &saved, NULL, NULL)) {
        g_strchomp(saved);
        LangId saved_id = lang_from_str(saved);
        if (saved_id != LANG_SYS)
            current_lang = saved_id;
    }
}

static void detect_theme(void) {
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

static const char* info5(const char *en, const char *pt, const char *es, const char *ru, const char *zh);

/* =========================================================================
   1. ESTADO GLOBAL DA APLICAÇÃO E CONTROLE DE LINHAS DINÂMICAS
    ========================================================================= */
static struct {
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
} app_data;

static void set_language(LangId lang);

static const char* lang_code(LangId id) {
    switch (id) {
        case LANG_EN: return LANG_CODE_EN;
        case LANG_PT: return LANG_CODE_PT;
        case LANG_ES: return LANG_CODE_ES;
        case LANG_RU: return LANG_CODE_RU;
        case LANG_ZH: return LANG_CODE_ZH;
        default: return "";
    }
}

static const char* lang_label(LangId id) {
    switch (id) {
        case LANG_SYS: return _("System Language");
        case LANG_EN:  return "English";
        case LANG_PT:  return "Portugu\u00eas";
        case LANG_ES:  return "Espa\u00f1ol";
        case LANG_RU:  return "\u0420\u0443\u0441\u0441\u043a\u0438\u0439";
        case LANG_ZH:  return "\u4e2d\u6587";
        default: return "?";
    }
}

static void rebuild_language_menu(void) {
    if (!app_data.lang_section) return;
    int n = g_menu_model_get_n_items(G_MENU_MODEL(app_data.lang_section));
    for (int i = n - 1; i >= 0; i--)
        g_menu_remove(app_data.lang_section, i);

    struct { LangId id; const char *action; } entries[] = {
        {LANG_SYS, "win.lang_sys"},
        {LANG_EN,  "win.lang_en"},
        {LANG_PT,  "win.lang_pt"},
        {LANG_ES,  "win.lang_es"},
        {LANG_RU,  "win.lang_ru"},
        {LANG_ZH,  "win.lang_zh"},
    };
    for (size_t i = 0; i < G_N_ELEMENTS(entries); i++) {
        char label[128];
        g_snprintf(label, sizeof(label), "%s %s",
            entries[i].id == current_lang ? "\u2713" : " ", lang_label(entries[i].id));
        g_menu_append(app_data.lang_section, label, entries[i].action);
    }
}

static void set_language(LangId lang) {
    if (current_lang == lang) return;
    current_lang = lang;
    g_autofree char *dir = g_build_filename(g_get_user_config_dir(), "blufixer", NULL);
    g_mkdir_with_parents(dir, 0755);
    g_autofree char *cfg = g_build_filename(dir, "language", NULL);
    g_file_set_contents(cfg, lang_code(lang), -1, NULL);
    rebuild_language_menu();
    AdwToast *toast = adw_toast_new(_("Please restart the application to apply the language change."));
    adw_toast_set_button_label(toast, _("Restart"));
    adw_toast_set_action_name(toast, "win.restart");
    adw_toast_set_priority(toast, ADW_TOAST_PRIORITY_HIGH);
    adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay), toast);
}

static void rebuild_theme_menu(void) {
    if (!app_data.theme_section) return;
    int n = g_menu_model_get_n_items(G_MENU_MODEL(app_data.theme_section));
    for (int i = n - 1; i >= 0; i--)
        g_menu_remove(app_data.theme_section, i);

    struct { ThemeId id; const char *action; } entries[] = {
        {THEME_SYSTEM, "win.theme_system"},
        {THEME_LIGHT,  "win.theme_light"},
        {THEME_DARK,   "win.theme_dark"},
    };
    for (size_t i = 0; i < G_N_ELEMENTS(entries); i++) {
        char label[128];
        const char *name = entries[i].id == THEME_SYSTEM ? _("Follow System Theme") :
                           entries[i].id == THEME_LIGHT  ? _("Light Theme") :
                                                           _("Dark Theme");
        g_snprintf(label, sizeof(label), "%s %s",
            entries[i].id == current_theme ? "\u2713" : " ", name);
        g_menu_append(app_data.theme_section, label, entries[i].action);
    }
}

static void set_theme(ThemeId t) {
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
    rebuild_theme_menu();
}

/* Lista de controle para rastrear e limpar as linhas da interface no scan */
static GList *dynamic_rows = NULL;

/* Linha de loader exibida durante a varredura */
static GtkWidget *scan_loader_row = NULL;

/* Controle de race condition no re-escaneamento */
static gint scanning = FALSE;
static guint scan_timeout_id = 0;

/* Detecção multiplataforma: ferramentas de sistema detectadas em tempo real */
static char priv_cmd[512]  = "pkexec";          /* Comando de elevação (pkexec | sudo -A) */
static char dl_cmd[16]      = "wget";           /* Ferramenta de download (wget | curl) */
static char restart_cmd[64] = "systemctl restart bluetooth";  /* Comando de reinicialização do serviço */
static char stop_cmd[64]    = "systemctl stop bluetooth";     /* Comando de parada do serviço */
static char start_cmd[64]   = "systemctl start bluetooth";    /* Comando de inicio do serviço */
static char fw_path[64]     = "/lib/firmware";  /* Diretório de firmware do kernel */
static gboolean has_elevation = TRUE;            /* FALSE se nenhum método de elevação for detectado */
static gboolean in_flatpak = FALSE;              /* TRUE se executando dentro de Flatpak */
static char askpass_path[128] = "";             /* Caminho do script de askpass */

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
static const char* hci_version_to_string(int hci_ver);
static void detect_system_tools(void);

/* =========================================================================
   2. VERIFICADOR DE VERSÃO (GITHUB RELEASES)
   ========================================================================= */
typedef struct {
    GtkWidget *update_btn;
    gboolean success;
    char latest_version[32];
} UpdateCheckData;

static int compare_versions(const char *v1, const char *v2) {
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

/* =========================================================================
   3. AÇÕES DO MENU SUPERIOR (ATUALIZADO PARA BLUFIXER)
      ========================================================================= */
static void on_about_action(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    AdwAboutDialog *about = ADW_ABOUT_DIALOG(adw_about_dialog_new());
    adw_about_dialog_set_application_name(about, "BluFixer");
    adw_about_dialog_set_version(about, APP_VERSION);
    adw_about_dialog_set_developer_name(about, "Renan Mayrinck");
    adw_about_dialog_set_copyright(about, "©2026 Renan Mayrinck");
    adw_about_dialog_set_application_icon(about, "org.renanmayrinck.blufixer");
    adw_about_dialog_add_link(about, _("GitHub Repository"), "https://github.com/mayrinck/blufixer");
    adw_about_dialog_set_issue_url(about, "https://github.com/mayrinck/blufixer/issues");

    g_autofree char *legal = g_strdup_printf(
        "%s\n\n"
        "MIT License\n"
        "Copyright ©2026 Renan Mayrinck\n\n"
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
    adw_about_dialog_add_legal_section(about, _("Legal Notice"), "©2026 Renan Mayrinck", GTK_LICENSE_CUSTOM, legal);

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

/* =========================================================================
   3. EXPLICADORES TÉCNICOS BALIZADOS
   ========================================================================= */
static void show_info_dialog(const char *body_markup) {
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

static const char* info5(const char *en, const char *pt, const char *es, const char *ru, const char *zh) {
    switch (current_lang) {
        case LANG_PT: return pt;
        case LANG_ES: return es;
        case LANG_RU: return ru;
        case LANG_ZH: return zh;
        default: return en;
    }
}

static void on_info_csr_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>CSR Power Fix (Connection/Reconnection)</b>\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>",
        info5(
            "Bluetooth devices with CSR (Cambridge Silicon Radio) chipsets have a low-power mode that, after initial pairing, keeps the BLE profile active. This prevents new profiles (such as A2DP for audio) from being negotiated correctly, resulting in:",
            "Dispositivos Bluetooth com chipset CSR (Cambridge Silicon Radio) possuem um modo de baixo consumo que, ap\u00f3s o pareamento inicial, mant\u00e9m o perfil de baixa energia (BLE) ativo. Isso impede que novos perfis (como A2DP para \u00e1udio) sejam negociados corretamente, resultando em:",
            "Los dispositivos Bluetooth con chipsets CSR (Cambridge Silicon Radio) tienen un modo de bajo consumo que, tras el emparejamiento inicial, mantiene el perfil BLE activo. Esto impide que nuevos perfiles (como A2DP para audio) se negocien correctamente, resultando en:",
            "\u0423\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0430 Bluetooth \u0441 \u0447\u0438\u043f\u0441\u0435\u0442\u0430\u043c\u0438 CSR (Cambridge Silicon Radio) \u0438\u043c\u0435\u044e\u0442 \u0440\u0435\u0436\u0438\u043c \u043d\u0438\u0437\u043a\u043e\u0433\u043e \u044d\u043d\u0435\u0440\u0433\u043e\u043f\u043e\u0442\u0440\u0435\u0431\u043b\u0435\u043d\u0438\u044f, \u043a\u043e\u0442\u043e\u0440\u044b\u0439 \u043f\u043e\u0441\u043b\u0435 \u043f\u0435\u0440\u0432\u043e\u043d\u0430\u0447\u0430\u043b\u044c\u043d\u043e\u0433\u043e \u0441\u043e\u043f\u0440\u044f\u0436\u0435\u043d\u0438\u044f \u043f\u043e\u0434\u0434\u0435\u0440\u0436\u0438\u0432\u0430\u0435\u0442 \u043f\u0440\u043e\u0444\u0438\u043b\u044c BLE \u0430\u043a\u0442\u0438\u0432\u043d\u044b\u043c. \u042d\u0442\u043e \u043f\u0440\u0435\u043f\u044f\u0442\u0441\u0442\u0432\u0443\u0435\u0442 \u043f\u0440\u0430\u0432\u0438\u043b\u044c\u043d\u043e\u0439 \u043d\u0435\u0433\u043e\u0446\u0438\u0430\u0446\u0438\u0438 \u043d\u043e\u0432\u044b\u0445 \u043f\u0440\u043e\u0444\u0438\u043b\u0435\u0439 (\u043d\u0430\u043f\u0440\u0438\u043c\u0435\u0440, A2DP \u0434\u043b\u044f \u0430\u0443\u0434\u0438\u043e), \u0447\u0442\u043e \u043f\u0440\u0438\u0432\u043e\u0434\u0438\u0442 \u043a:",
            "\u5177\u6709CSR\uff08Cambridge Silicon Radio\uff09\u82af\u7247\u7ec4\u7684\u84dd\u7259\u8bbe\u5907\u5177\u6709\u4f4e\u529f\u8017\u6a21\u5f0f\uff0c\u8be5\u6a21\u5f0f\u5728\u521d\u6b21\u914d\u5bf9\u540e\u4fdd\u6301BLE\u914d\u7f6e\u6587\u4ef6\u4e3b\u52a8\u3002\u8fd9\u4f1a\u963b\u6b62\u65b0\u914d\u7f6e\u6587\u4ef6\uff08\u5982A2DP\u97f3\u9891\uff09\u88ab\u6b63\u786e\u534f\u5546\uff0c\u5bfc\u81f4:"),
        color, info5("\u2022 No audio after reconnecting", "\u2022 Sem \u00e1udio ap\u00f3s reconectar", "\u2022 Sin audio despu\u00e9s de reconectar", "\u2022 \u041d\u0435\u0442 \u0437\u0432\u0443\u043a\u0430 \u043f\u043e\u0441\u043b\u0435 \u043f\u0435\u0440\u0435\u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f", "\u2022 \u91cd\u65b0\u8fde\u63a5\u540e\u65e0\u97f3\u9891"),
        color, info5("\u2022 Device shows \"Connected\" but no service", "\u2022 Dispositivo mostra \"Conectado\" mas sem servi\u00e7o", "\u2022 Dispositivo muestra \"Conectado\" pero sin servicio", "\u2022 \u0423\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u043e \u043f\u043e\u043a\u0430\u0437\u044b\u0432\u0430\u0435\u0442 \"\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u043e\", \u043d\u043e \u0431\u0435\u0437 \u0443\u0441\u043b\u0443\u0433\u0438", "\u2022 \u8bbe\u5907\u663e\u793a\u5df2\u8fde\u63a5\u4f46\u65e0\u670d\u52a1"),
        color, info5("\u2022 Connection drops after seconds", "\u2022 Conex\u00e3o cai ap\u00f3s segundos", "\u2022 La conexi\u00f3n cae despu\u00e9s de segundos", "\u2022 \u0421\u043e\u0435\u0434\u0438\u043d\u0435\u043d\u0438\u0435 \u0440\u0430\u0437\u0440\u044b\u0432\u0430\u0435\u0442\u0441\u044f \u0447\u0435\u0440\u0435\u0437 \u0441\u0435\u043a\u0443\u043d\u0434\u044b", "\u2022 \u8fde\u63a5\u5728\u51e0\u79d2\u540e\u65ad\u5f00"),
        info5(
            "This fix disables the CSR driver's power-saving mode via the btusb.enable_autosuspend=0 parameter, forcing the chip to keep all profiles active.",
            "Esta corre\u00e7\u00e3o desabilita o modo de economia de energia do driver CSR via par\u00e2metro btusb.enable_autosuspend=0, for\u00e7ando o chip a manter todos os perfis ativos.",
            "Esta correcci\u00f3n deshabilita el modo de ahorro de energ\u00eda del controlador CSR mediante el par\u00e1metro btusb.enable_autosuspend=0, forzando al chip a mantener todos los perfiles activos.",
            "\u042d\u0442\u043e \u0438\u0441\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0435 \u043e\u0442\u043a\u043b\u044e\u0447\u0430\u0435\u0442 \u0440\u0435\u0436\u0438\u043c \u044d\u043d\u0435\u0440\u0433\u043e\u0441\u0431\u0435\u0440\u0435\u0436\u0435\u043d\u0438\u044f \u0434\u0440\u0430\u0439\u0432\u0435\u0440\u0430 CSR \u0447\u0435\u0440\u0435\u0437 \u043f\u0430\u0440\u0430\u043c\u0435\u0442\u0440 btusb.enable_autosuspend=0, \u0437\u0430\u0441\u0442\u0430\u0432\u043b\u044f\u044f \u0447\u0438\u043f \u043f\u043e\u0434\u0434\u0435\u0440\u0436\u0438\u0432\u0430\u0442\u044c \u0432\u0441\u0435 \u043f\u0440\u043e\u0444\u0438\u043b\u0438 \u0430\u043a\u0442\u0438\u0432\u043d\u044b\u043c\u0438.",
            "\u6b64\u4fee\u590d\u901a\u8fc7 btusb.enable_autosuspend=0 \u53c2\u6570\u7981\u7528CSR\u9a71\u52a8\u7a0b\u5e8f\u7684\u8282\u80fd\u6a21\u5f0f\uff0c\u5f3a\u5236\u82af\u7247\u4fdd\u6301\u6240\u6709\u914d\u7f6e\u6587\u4ef6\u5904\u4e8e\u6d3b\u8dc3\u72b6\u6001\u3002"),
        info5("Command executed:", "Comando Executado:", "Comando ejecutado:", "\u0412\u044b\u043f\u043e\u043b\u043d\u044f\u0435\u043c\u0430\u044f \u043a\u043e\u043c\u0430\u043d\u0434\u0430:", "\u6267\u884c\u7684\u547d\u4ee4:"),
        color, info5("\u2022 echo options btusb enable_autosuspend=0 > /etc/modprobe.d/btusb.conf",
                      "\u2022 echo options btusb enable_autosuspend=0 > /etc/modprobe.d/btusb.conf",
                      "\u2022 echo options btusb enable_autosuspend=0 > /etc/modprobe.d/btusb.conf",
                      "\u2022 echo options btusb enable_autosuspend=0 > /etc/modprobe.d/btusb.conf",
                      "\u2022 echo options btusb enable_autosuspend=0 > /etc/modprobe.d/btusb.conf"),
        info5("Technical details:", "Detalhes T\u00e9cnicos:", "Detalles t\u00e9cnicos:", "\u0422\u0435\u0445\u043d\u0438\u0447\u0435\u0441\u043a\u0438\u0435 \u0434\u0435\u0442\u0430\u043b\u0438:", "\u6280\u672f\u8be6\u60c5:"),
        color, info5(
            "The enable_autosuspend parameter controls whether the btusb driver allows the hardware to enter D3 (suspended) state when there is no traffic. CSR Bluetooth chips are particularly affected because their firmware does not correctly renegotiate profiles when leaving the suspended state.",
            "O par\u00e2metro enable_autosuspend controla se o driver btusb permite que o hardware entre no estado D3 (suspenso) quando n\u00e3o h\u00e1 tr\u00e1fego. CSR Bluetooth chips s\u00e3o particularmente afetados porque seu firmware n\u00e3o renegocia corretamente os perfis ao sair do estado de suspens\u00e3o.",
            "El par\u00e1metro enable_autosuspend controla si el controlador btusb permite que el hardware entre en estado D3 (suspendido) cuando no hay tr\u00e1fico. Los chips Bluetooth CSR se ven particularmente afectados porque su firmware no renegocia correctamente los perfiles al salir del estado suspendido.",
            "\u041f\u0430\u0440\u0430\u043c\u0435\u0442\u0440 enable_autosuspend \u0443\u043f\u0440\u0430\u0432\u043b\u044f\u0435\u0442, \u0440\u0430\u0437\u0440\u0435\u0448\u0430\u0435\u0442 \u043b\u0438 \u0434\u0440\u0430\u0439\u0432\u0435\u0440 btusb \u043e\u0431\u043e\u0440\u0443\u0434\u043e\u0432\u0430\u043d\u0438\u044e \u0432\u0445\u043e\u0434\u0438\u0442\u044c \u0432 \u0441\u043e\u0441\u0442\u043e\u044f\u043d\u0438\u0435 D3 (\u043f\u0440\u0438\u043e\u0441\u0442\u0430\u043d\u043e\u0432\u043a\u0430) \u043f\u0440\u0438 \u043e\u0442\u0441\u0443\u0442\u0441\u0442\u0432\u0438\u0438 \u0442\u0440\u0430\u0444\u0438\u043a\u0430. \u0427\u0438\u043f\u044b CSR Bluetooth \u043e\u0441\u043e\u0431\u0435\u043d\u043d\u043e \u0443\u044f\u0437\u0432\u0438\u043c\u044b, \u043f\u043e\u0442\u043e\u043c\u0443 \u0447\u0442\u043e \u0438\u0445 \u043f\u0440\u043e\u0448\u0438\u0432\u043a\u0430 \u043d\u0435\u043a\u043e\u0440\u0440\u0435\u043a\u0442\u043d\u043e \u043f\u0435\u0440\u0435\u043d\u0430\u0441\u0442\u0440\u0430\u0438\u0432\u0430\u0435\u0442 \u043f\u0440\u043e\u0444\u0438\u043b\u0438 \u043f\u0440\u0438 \u0432\u044b\u0445\u043e\u0434\u0435 \u0438\u0437 \u043f\u0440\u0438\u043e\u0441\u0442\u0430\u043d\u043e\u0432\u043b\u0435\u043d\u043d\u043e\u0433\u043e \u0441\u043e\u0441\u0442\u043e\u044f\u043d\u0438\u044f.",
            "enable_autosuspend\u53c2\u6570\u63a7\u5236btusb\u9a71\u52a8\u7a0b\u5e8f\u662f\u5426\u5141\u8bb8\u786c\u4ef6\u5728\u65e0\u6d41\u91cf\u65f6\u8fdb\u5165D3\uff08\u6302\u8d77\uff09\u72b6\u6001\u3002CSR\u84dd\u7259\u82af\u7247\u53d7\u5f71\u54cd\u7279\u522b\u5927\uff0c\u56e0\u4e3a\u5176\u56fa\u4ef6\u5728\u79bb\u5f00\u6302\u8d77\u72b6\u6001\u65f6\u65e0\u6cd5\u6b63\u786e\u91cd\u65b0\u534f\u5546\u914d\u7f6e\u6587\u4ef6\u3002"));
    show_info_dialog(markup);
    g_free(markup);
}

static void on_info_ertm_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>ERTM Fix (Enhanced Re-Transmission Mode)</b>\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s",
        info5(
            "ERTM is a Bluetooth protocol feature that ensures reliable data delivery via retransmission. In some chipsets, especially Intel and Broadcom, the ERTM implementation is problematic and causes:",
            "O ERTM \u00e9 um recurso do protocolo Bluetooth que garante entrega confi\u00e1vel de dados via retransmiss\u00e3o. Em alguns chipsets, especialmente Intel e Broadcom, a implementa\u00e7\u00e3o do ERTM \u00e9 problem\u00e1tica e causa:",
            "ERTM es una caracter\u00edstica del protocolo Bluetooth que garantiza la entrega confiable de datos mediante retransmisi\u00f3n. En algunos conjuntos de chips, especialmente Intel y Broadcom, la implementaci\u00f3n de ERTM es problem\u00e1tica y causa:",
            "ERTM \u2014 \u044d\u0442\u043e \u0444\u0443\u043d\u043a\u0446\u0438\u044f \u043f\u0440\u043e\u0442\u043e\u043a\u043e\u043b\u0430 Bluetooth, \u043e\u0431\u0435\u0441\u043f\u0435\u0447\u0438\u0432\u0430\u044e\u0449\u0430\u044f \u043d\u0430\u0434\u0451\u0436\u043d\u0443\u044e \u0434\u043e\u0441\u0442\u0430\u0432\u043a\u0443 \u0434\u0430\u043d\u043d\u044b\u0445 \u0447\u0435\u0440\u0435\u0437 \u043f\u043e\u0432\u0442\u043e\u0440\u043d\u0443\u044e \u043f\u0435\u0440\u0435\u0434\u0430\u0447\u0443. \u0412 \u043d\u0435\u043a\u043e\u0442\u043e\u0440\u044b\u0445 \u0447\u0438\u043f\u0441\u0435\u0442\u0430\u0445, \u043e\u0441\u043e\u0431\u0435\u043d\u043d\u043e Intel \u0438 Broadcom, \u0440\u0435\u0430\u043b\u0438\u0437\u0430\u0446\u0438\u044f ERTM \u043f\u0440\u043e\u0431\u043b\u0435\u043c\u0430\u0442\u0438\u0447\u043d\u0430 \u0438 \u043f\u0440\u0438\u0432\u043e\u0434\u0438\u0442 \u043a:",
            "ERTM\u662f\u84dd\u7259\u534f\u8bae\u7684\u4e00\u9879\u529f\u80fd\uff0c\u901a\u8fc7\u91cd\u4f20\u786e\u4fdd\u53ef\u9760\u7684\u6570\u636e\u4f20\u9012\u3002\u5728\u67d0\u4e9b\u82af\u7247\u7ec4\u4e2d\uff0c\u5c24\u5176\u662fIntel\u548cBroadcom\uff0cERTM\u7684\u5b9e\u73b0\u5b58\u5728\u95ee\u9898\u5e76\u5bfc\u81f4:"),
        color, info5("\u2022 Failure to connect audio devices (A2DP)", "\u2022 Falha ao conectar dispositivos de \u00e1udio (A2DP)", "\u2022 Fallo al conectar dispositivos de audio (A2DP)", "\u2022 \u041d\u0435\u0443\u0434\u0430\u0447\u0430 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f \u0430\u0443\u0434\u0438\u043e\u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432 (A2DP)", "\u2022 \u65e0\u6cd5\u8fde\u63a5\u97f3\u9891\u8bbe\u5907 (A2DP)"),
        color, info5("\u2022 Connection drops when playing media", "\u2022 Queda de conex\u00e3o ao reproduzir m\u00eddia", "\u2022 La conexi\u00f3n cae al reproducir contenido multimedia", "\u2022 \u0421\u043e\u0435\u0434\u0438\u043d\u0435\u043d\u0438\u0435 \u0440\u0430\u0437\u0440\u044b\u0432\u0430\u0435\u0442\u0441\u044f \u043f\u0440\u0438 \u0432\u043e\u0441\u043f\u0440\u043e\u0438\u0437\u0432\u0435\u0434\u0435\u043d\u0438\u0438 \u043c\u0435\u0434\u0438\u0430", "\u2022 \u64ad\u653e\u5a92\u4f53\u65f6\u8fde\u63a5\u65ad\u5f00"),
        color, info5("\u2022 \"Connection Reset by Peer\" error in syslog", "\u2022 Erro \"Connection Reset by Peer\" no syslog", "\u2022 Error \"Connection Reset by Peer\" en syslog", "\u2022 \u041e\u0448\u0438\u0431\u043a\u0430 \"Connection Reset by Peer\" \u0432 syslog", "\u2022 syslog\u4e2d\u7684\u201cConnection Reset by Peer\u201d\u9519\u8bef"),
        info5(
            "This fix disables ERTM globally in the Bluetooth driver, forcing the use of basic L2CAP mode which is more compatible.",
            "Esta corre\u00e7\u00e3o desabilita o ERTM globalmente no driver Bluetooth, for\u00e7ando o uso do modo b\u00e1sico L2CAP que \u00e9 mais compat\u00edvel.",
            "Esta correcci\u00f3n deshabilita ERTM globalmente en el controlador Bluetooth, forzando el uso del modo L2CAP b\u00e1sico que es m\u00e1s compatible.",
            "\u042d\u0442\u043e \u0438\u0441\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0435 \u043e\u0442\u043a\u043b\u044e\u0447\u0430\u0435\u0442 ERTM \u0433\u043b\u043e\u0431\u0430\u043b\u044c\u043d\u043e \u0432 \u0434\u0440\u0430\u0439\u0432\u0435\u0440\u0435 Bluetooth, \u0437\u0430\u0441\u0442\u0430\u0432\u043b\u044f\u044f \u0438\u0441\u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u044c \u0431\u0430\u0437\u043e\u0432\u044b\u0439 \u0440\u0435\u0436\u0438\u043c L2CAP, \u043a\u043e\u0442\u043e\u0440\u044b\u0439 \u0431\u043e\u043b\u0435\u0435 \u0441\u043e\u0432\u043c\u0435\u0441\u0442\u0438\u043c.",
            "\u6b64\u4fee\u590d\u5168\u5c40\u7981\u7528\u84dd\u7259\u9a71\u52a8\u7a0b\u5e8f\u4e2d\u7684ERTM\uff0c\u5f3a\u5236\u4f7f\u7528\u66f4\u517c\u5bb9\u7684\u57fa\u672cL2CAP\u6a21\u5f0f\u3002"),
        info5("Command executed:", "Comando Executado:", "Comando ejecutado:", "\u0412\u044b\u043f\u043e\u043b\u043d\u044f\u0435\u043c\u0430\u044f \u043a\u043e\u043c\u0430\u043d\u0434\u0430:", "\u6267\u884c\u7684\u547d\u4ee4:"),
        color, info5("\u2022 echo options bluetooth disable_ertm=Y > /etc/modprobe.d/bluetooth-ertm.conf",
                      "\u2022 echo options bluetooth disable_ertm=Y > /etc/modprobe.d/bluetooth-ertm.conf",
                      "\u2022 echo options bluetooth disable_ertm=Y > /etc/modprobe.d/bluetooth-ertm.conf",
                      "\u2022 echo options bluetooth disable_ertm=Y > /etc/modprobe.d/bluetooth-ertm.conf",
                      "\u2022 echo options bluetooth disable_ertm=Y > /etc/modprobe.d/bluetooth-ertm.conf"),
        info5("Note: In both cases the bluetooth service is restarted automatically to apply the changes.",
              "Nota: Em ambos os casos o servi\u00e7o bluetooth \u00e9 reiniciado automaticamente para aplicar as mudan\u00e7as.",
              "Nota: En ambos casos, el servicio bluetooth se reinicia autom\u00e1ticamente para aplicar los cambios.",
              "\u041f\u0440\u0438\u043c\u0435\u0447\u0430\u043d\u0438\u0435: \u0412 \u043e\u0431\u043e\u0438\u0445 \u0441\u043b\u0443\u0447\u0430\u044f\u0445 \u0441\u043b\u0443\u0436\u0431\u0430 bluetooth \u0430\u0432\u0442\u043e\u043c\u0430\u0442\u0438\u0447\u0435\u0441\u043a\u0438 \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a\u0430\u0435\u0442\u0441\u044f \u0434\u043b\u044f \u043f\u0440\u0438\u043c\u0435\u043d\u0435\u043d\u0438\u044f \u0438\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u0439.",
              "\u6ce8\u610f\uff1a\u5728\u4e24\u79cd\u60c5\u51b5\u4e0b\uff0c\u84dd\u7259\u670d\u52a1\u90fd\u4f1a\u81ea\u52a8\u91cd\u542f\u4ee5\u5e94\u7528\u66f4\u6539\u3002"));
    show_info_dialog(markup);
    g_free(markup);
}

static void on_info_realtek_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>%s</b>\n\n"
        "%s\n\n"
        "%s\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s",
        info5("Realtek Bluetooth Firmware", "Firmware Realtek Bluetooth",
              "Firmware Bluetooth Realtek",
              "\u041f\u0440\u043e\u0448\u0438\u0432\u043a\u0430 Bluetooth Realtek",
              "Realtek\u84dd\u7259\u56fa\u4ef6"),
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
        color, info5("\u2022 rtl8761b_fw.bin obtained from firmware-realtek package",
                      "\u2022 rtl8761b_fw.bin obtido do pacote firmware-realtek",
                      "\u2022 rtl8761b_fw.bin obtenido del paquete firmware-realtek",
                      "\u2022 rtl8761b_fw.bin \u043f\u043e\u043b\u0443\u0447\u0435\u043d \u0438\u0437 \u043f\u0430\u043a\u0435\u0442\u0430 firmware-realtek",
                      "\u2022 \u4ecefirmware-realtek\u5305\u83b7\u53d6\u7684rtl8761b_fw.bin"),
        info5("Attention: Installation requires administrative access. The firmware will be extracted and loaded automatically.",
              "Aten\u00e7\u00e3o: A instala\u00e7\u00e3o requer acesso administrativo. O firmware ser\u00e1 extra\u00eddo e carregado automaticamente.",
              "Atenci\u00f3n: La instalaci\u00f3n requiere acceso administrativo. El firmware se extraer\u00e1 y cargar\u00e1 autom\u00e1ticamente.",
              "\u0412\u043d\u0438\u043c\u0430\u043d\u0438\u0435: \u0423\u0441\u0442\u0430\u043d\u043e\u0432\u043a\u0430 \u0442\u0440\u0435\u0431\u0443\u0435\u0442 \u0430\u0434\u043c\u0438\u043d\u0438\u0441\u0442\u0440\u0430\u0442\u0438\u0432\u043d\u043e\u0433\u043e \u0434\u043e\u0441\u0442\u0443\u043f\u0430. \u041f\u0440\u043e\u0448\u0438\u0432\u043a\u0430 \u0431\u0443\u0434\u0435\u0442 \u0430\u0432\u0442\u043e\u043c\u0430\u0442\u0438\u0447\u0435\u0441\u043a\u0438 \u0438\u0437\u0432\u043b\u0435\u0447\u0435\u043d\u0430 \u0438 \u0437\u0430\u0433\u0440\u0443\u0436\u0435\u043d\u0430.",
              "\u6ce8\u610f\uff1a\u5b89\u88c5\u9700\u8981\u7ba1\u7406\u5458\u6743\u9650\u3002\u56fa\u4ef6\u5c06\u81ea\u52a8\u63d0\u53d6\u5e76\u52a0\u8f7d\u3002"));
    show_info_dialog(markup);
    g_free(markup);
}

static void on_info_rfkill_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>%s</b>\n\n"
        "%s\n\n"
        "<b>%s</b>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>",
        info5("RFKILL Electrical Unblock (Airplane Mode)",
              "Desbloqueio El\u00e9trico via RFKILL (Modo Avi\u00e3o)",
              "Desbloqueo El\u00e9ctrico v\u00eda RFKILL (Modo Avión)",
              "\u042d\u043b\u0435\u043a\u0442\u0440\u0438\u0447\u0435\u0441\u043a\u043e\u0435 \u0440\u0430\u0437\u0431\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u0430\u043d\u0438\u0435 RFKILL (\u0420\u0435\u0436\u0438\u043c \u043f\u043e\u043b\u0451\u0442\u0430)",
              "\u901a\u8fc7RFKILL\u7535\u6c14\u89e3\u9501\uff08\u98de\u884c\u6a21\u5f0f\uff09"),
        info5("Executes a direct hardware command to break software locks (Soft Blocks) that persist even when the user tries to activate Bluetooth through their desktop environment's native settings.",
              "Executa um comando direto de hardware para quebrar travas de software (Soft Blocks) que persistem mesmo quando o usu\u00e1rio tenta ativar o Bluetooth pelas configura\u00e7\u00f5es nativas da sua interface de desktop.",
              "Ejecuta un comando directo de hardware para romper bloqueos de software (Soft Blocks) que persisten incluso cuando el usuario intenta activar Bluetooth desde la configuraci\u00f3n nativa de su escritorio.",
              "\u0412\u044b\u043f\u043e\u043b\u043d\u044f\u0435\u0442 \u043f\u0440\u044f\u043c\u0443\u044e \u043a\u043e\u043c\u0430\u043d\u0434\u0443 \u043e\u0431\u043e\u0440\u0443\u0434\u043e\u0432\u0430\u043d\u0438\u044f \u0434\u043b\u044f \u0441\u043d\u044f\u0442\u0438\u044f \u043f\u0440\u043e\u0433\u0440\u0430\u043c\u043c\u043d\u044b\u0445 \u0431\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u043e\u043a (Soft Blocks), \u043a\u043e\u0442\u043e\u0440\u044b\u0435 \u0441\u043e\u0445\u0440\u0430\u043d\u044f\u044e\u0442\u0441\u044f \u0434\u0430\u0436\u0435 \u043f\u0440\u0438 \u043f\u043e\u043f\u044b\u0442\u043a\u0435 \u0432\u043a\u043b\u044e\u0447\u0438\u0442\u044c Bluetooth \u0447\u0435\u0440\u0435\u0437 \u0441\u0442\u0430\u043d\u0434\u0430\u0440\u0442\u043d\u044b\u0435 \u043d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438 \u0440\u0430\u0431\u043e\u0447\u0435\u0433\u043e \u0441\u0442\u043e\u043b\u0430.",
              "\u6267\u884c\u786c\u4ef6\u76f4\u63a5\u547d\u4ee4\u4ee5\u6253\u7834\u8f6f\u4ef6\u9501\uff08Soft Blocks\uff09\uff0c\u8be5\u9501\u5728\u7528\u6237\u5c1d\u8bd5\u901a\u8fc7\u684c\u9762\u73af\u5883\u539f\u751f\u8bbe\u7f6e\u542f\u7528\u84dd\u7259\u65f6\u4ecd\u7136\u5b58\u5728\u3002"),
        info5("Command executed:", "Comando Executado:", "Comando ejecutado:", "\u0412\u044b\u043f\u043e\u043b\u043d\u044f\u0435\u043c\u0430\u044f \u043a\u043e\u043c\u0430\u043d\u0434\u0430:", "\u6267\u884c\u7684\u547d\u4ee4:"),
        color,
        info5("• rfkill unblock bluetooth &amp;&amp; bluetoothctl power on",
              "• rfkill unblock bluetooth &amp;&amp; bluetoothctl power on",
              "• rfkill unblock bluetooth &amp;&amp; bluetoothctl power on",
              "• rfkill unblock bluetooth &amp;&amp; bluetoothctl power on",
              "• rfkill unblock bluetooth &amp;&amp; bluetoothctl power on"));
    show_info_dialog(markup);
    g_free(markup);
}

static void on_info_restart_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>%s</b>\n\n"
        "%s\n\n"
        "<b>%s</b>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>",
        info5("Bluetooth Service Restart",
              "Reinicializa\u00e7\u00e3o do Servi\u00e7o Bluetooth",
              "Reinicio del Servicio Bluetooth",
              "\u041f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a \u0441\u043b\u0443\u0436\u0431\u044b Bluetooth",
              "\u91cd\u542f\u84dd\u7259\u670d\u52a1"),
        info5("Forces the system service to tear down and rebuild the entire Bluetooth stack. This clears corrupted memory buffers and forces immediate loading of any configuration file changes made.",
              "For\u00e7a o servi\u00e7o do sistema a derrubar e reerguer a stack inteira de bluetooth do sistema operacional. Isso limpa buffers de mem\u00f3ria corrompidos e for\u00e7a o carregamento imediato de quaisquer modifica\u00e7\u00f5es feitas nos arquivos de configura\u00e7\u00e3o.",
              "Fuerza al servicio del sistema a derribar y reconstruir toda la pila Bluetooth. Esto limpia los b\u00faferes de memoria corruptos y fuerza la carga inmediata de cualquier cambio en los archivos de configuraci\u00f3n.",
              "\u041f\u0440\u0438\u043d\u0443\u0436\u0434\u0430\u0435\u0442 \u0441\u0438\u0441\u0442\u0435\u043c\u043d\u0443\u044e \u0441\u043b\u0443\u0436\u0431\u0443 \u043e\u0441\u0442\u0430\u043d\u043e\u0432\u0438\u0442\u044c \u0438 \u043f\u0435\u0440\u0435\u0441\u0442\u0440\u043e\u0438\u0442\u044c \u0432\u0435\u0441\u044c \u0441\u0442\u0435\u043a Bluetooth. \u042d\u0442\u043e \u043e\u0447\u0438\u0449\u0430\u0435\u0442 \u043f\u043e\u0432\u0440\u0435\u0436\u0434\u0451\u043d\u043d\u044b\u0435 \u0431\u0443\u0444\u0435\u0440\u044b \u043f\u0430\u043c\u044f\u0442\u0438 \u0438 \u043e\u0431\u0435\u0441\u043f\u0435\u0447\u0438\u0432\u0430\u0435\u0442 \u043d\u0435\u043c\u0435\u0434\u043b\u0435\u043d\u043d\u0443\u044e \u0437\u0430\u0433\u0440\u0443\u0437\u043a\u0443 \u043b\u044e\u0431\u044b\u0445 \u0438\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u0439 \u0432 \u043a\u043e\u043d\u0444\u0438\u0433\u0443\u0440\u0430\u0446\u0438\u043e\u043d\u043d\u044b\u0445 \u0444\u0430\u0439\u043b\u0430\u0445.",
              "\u5f3a\u5236\u7cfb\u7edf\u670d\u52a1\u62c6\u9664\u5e76\u91cd\u5efa\u6574\u4e2a\u84dd\u7259\u534f\u8bae\u6808\u3002\u8fd9\u4f1a\u6e05\u9664\u53d7\u635f\u7684\u5185\u5b58\u7f13\u51b2\u533a\uff0c\u5e76\u5f3a\u5236\u7acb\u5373\u52a0\u8f7d\u5bf9\u914d\u7f6e\u6587\u4ef6\u505a\u51fa\u7684\u4efb\u4f55\u4fee\u6539\u3002"),
        info5("Command executed:", "Comando Executado:", "Comando ejecutado:", "\u0412\u044b\u043f\u043e\u043b\u043d\u044f\u0435\u043c\u0430\u044f \u043a\u043e\u043c\u0430\u043d\u0434\u0430:", "\u6267\u884c\u7684\u547d\u4ee4:"),
        color, restart_cmd);
    show_info_dialog(markup);
    g_free(markup);
}

static void on_info_legacy_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>%s</b>\n\n"
        "%s\n\n"
        "%s\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s",
        info5("Legacy Bluetooth Compatibility Mode", "Modo de Compatibilidade Bluetooth Legado",
              "Modo de Compatibilidad Bluetooth Heredado",
              "\u0420\u0435\u0436\u0438\u043c \u0443\u0441\u0442\u0430\u0440\u0435\u0432\u0448\u0435\u0439 \u0441\u043e\u0432\u043c\u0435\u0441\u0442\u0438\u043c\u043e\u0441\u0442\u0438 Bluetooth",
              "\u9057\u7559\u84dd\u7259\u517c\u5bb9\u6a21\u5f0f"),
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
        color, info5("\u2022 echo Security=legacy >> /etc/bluetooth/main.conf",
                      "\u2022 echo Security=legacy >> /etc/bluetooth/main.conf",
                      "\u2022 echo Security=legacy >> /etc/bluetooth/main.conf",
                      "\u2022 echo Security=legacy >> /etc/bluetooth/main.conf",
                      "\u2022 echo Security=legacy >> /etc/bluetooth/main.conf"),
        info5("Note: In both cases the bluetooth service is restarted automatically to apply the changes.",
              "Nota: Em ambos os casos o servi\u00e7o bluetooth \u00e9 reiniciado automaticamente para aplicar as mudan\u00e7as.",
              "Nota: En ambos casos, el servicio bluetooth se reinicia autom\u00e1ticamente para aplicar los cambios.",
              "\u041f\u0440\u0438\u043c\u0435\u0447\u0430\u043d\u0438\u0435: \u0412 \u043e\u0431\u043e\u0438\u0445 \u0441\u043b\u0443\u0447\u0430\u044f\u0445 \u0441\u043b\u0443\u0436\u0431\u0430 bluetooth \u0430\u0432\u0442\u043e\u043c\u0430\u0442\u0438\u0447\u0435\u0441\u043a\u0438 \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a\u0430\u0435\u0442\u0441\u044f \u0434\u043b\u044f \u043f\u0440\u0438\u043c\u0435\u043d\u0435\u043d\u0438\u044f \u0438\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u0439.",
              "\u6ce8\u610f\uff1a\u5728\u4e24\u79cd\u60c5\u51b5\u4e0b\uff0c\u84dd\u7259\u670d\u52a1\u90fd\u4f1a\u81ea\u52a8\u91cd\u542f\u4ee5\u5e94\u7528\u66f4\u6539\u3002"));
    show_info_dialog(markup);
    g_free(markup);
}

static void on_info_broadcom_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>%s</b>\n\n"
        "%s\n\n"
        "%s\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s",
        info5("Broadcom / Cypress Firmware (b43)", "Firmware Broadcom / Cypress (b43)",
              "Firmware Broadcom / Cypress (b43)", "\u041f\u0440\u043e\u0448\u0438\u0432\u043a\u0430 Broadcom / Cypress (b43)",
              "Broadcom/Cypress\u56fa\u4ef6(b43)"),
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
        color, info5("\u2022 [package manager] install b43-fwcutter and dependencies",
                      "\u2022 [gerenciador de pacotes] install b43-fwcutter e depend\u00eancias",
                      "\u2022 [gestor de paquetes] install b43-fwcutter y dependencias",
                      "\u2022 [\u043c\u0435\u043d\u0435\u0434\u0436\u0435\u0440 \u043f\u0430\u043a\u0435\u0442\u043e\u0432] install b43-fwcutter \u0438 \u0437\u0430\u0432\u0438\u0441\u0438\u043c\u043e\u0441\u0442\u0438",
                      "\u2022 [\u5305\u7ba1\u7406\u5668] install b43-fwcutter \u53ca\u4f9d\u8d56\u9879"),
        info5("Attention: Installation requires administrative access. The firmware will be extracted and loaded automatically.",
              "Aten\u00e7\u00e3o: A instala\u00e7\u00e3o requer acesso administrativo. O firmware ser\u00e1 extra\u00eddo e carregado automaticamente.",
              "Atenci\u00f3n: La instalaci\u00f3n requiere acceso administrativo. El firmware se extraer\u00e1 y cargar\u00e1 autom\u00e1ticamente.",
              "\u0412\u043d\u0438\u043c\u0430\u043d\u0438\u0435: \u0423\u0441\u0442\u0430\u043d\u043e\u0432\u043a\u0430 \u0442\u0440\u0435\u0431\u0443\u0435\u0442 \u0430\u0434\u043c\u0438\u043d\u0438\u0441\u0442\u0440\u0430\u0442\u0438\u0432\u043d\u043e\u0433\u043e \u0434\u043e\u0441\u0442\u0443\u043f\u0430. \u041f\u0440\u043e\u0448\u0438\u0432\u043a\u0430 \u0431\u0443\u0434\u0435\u0442 \u0430\u0432\u0442\u043e\u043c\u0430\u0442\u0438\u0447\u0435\u0441\u043a\u0438 \u0438\u0437\u0432\u043b\u0435\u0447\u0435\u043d\u0430 \u0438 \u0437\u0430\u0433\u0440\u0443\u0436\u0435\u043d\u0430.",
              "\u6ce8\u610f\uff1a\u5b89\u88c5\u9700\u8981\u7ba1\u7406\u5458\u6743\u9650\u3002\u56fa\u4ef6\u5c06\u81ea\u52a8\u63d0\u53d6\u5e76\u52a0\u8f7d\u3002"));
    show_info_dialog(markup);
    g_free(markup);
}

static void on_info_perm_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>%s</b>\n\n"
        "%s\n\n"
        "%s\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s",
        info5("Bluetooth Group Permissions", "Permiss\u00f5es de Grupo para Bluetooth",
              "Permisos de Grupo para Bluetooth",
              "\u0413\u0440\u0443\u043f\u043f\u043e\u0432\u044b\u0435 \u0440\u0430\u0437\u0440\u0435\u0448\u0435\u043d\u0438\u044f Bluetooth",
              "\u84dd\u7259\u7ec4\u6743\u9650"),
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
        color, info5("\u2022 usermod -aG lp &lt;current_user&gt;", "\u2022 usermod -aG lp &lt;usuario_atual&gt;",
                      "\u2022 usermod -aG lp &lt;usuario_actual&gt;",
                      "\u2022 usermod -aG lp &lt;\u0442\u0435\u043a\u0443\u0449\u0438\u0439_\u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u0435\u043b\u044c&gt;",
                      "\u2022 usermod -aG lp &lt;\u5f53\u524d\u7528\u6237&gt;"),
        info5("Important: After execution, you need to restart the session (logout and login) for the new permissions to take effect.",
              "Importante: Ap\u00f3s a execu\u00e7\u00e3o, \u00e9 necess\u00e1rio reiniciar a sess\u00e3o (logout e login) para que as novas permiss\u00f5es tenham efeito.",
              "Importante: Despu\u00e9s de la ejecuci\u00f3n, es necesario reiniciar la sesi\u00f3n (cerrar sesi\u00f3n e iniciarla de nuevo) para que los nuevos permisos surtan efecto.",
              "\u0412\u0430\u0436\u043d\u043e: \u041f\u043e\u0441\u043b\u0435 \u0432\u044b\u043f\u043e\u043b\u043d\u0435\u043d\u0438\u044f \u043d\u0435\u043e\u0431\u0445\u043e\u0434\u0438\u043c\u043e \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u044c \u0441\u0435\u0430\u043d\u0441 (\u0432\u044b\u0439\u0442\u0438 \u0438 \u0432\u043e\u0439\u0442\u0438), \u0447\u0442\u043e\u0431\u044b \u043d\u043e\u0432\u044b\u0435 \u0440\u0430\u0437\u0440\u0435\u0448\u0435\u043d\u0438\u044f \u0432\u0441\u0442\u0443\u043f\u0438\u043b\u0438 \u0432 \u0441\u0438\u043b\u0443.",
              "\u91cd\u8981\uff1a\u6267\u884c\u540e\uff0c\u60a8\u9700\u8981\u91cd\u542f\u4f1a\u8bdd\uff08\u6ce8\u9500\u5e76\u91cd\u65b0\u767b\u5f55\uff09\u4ee5\u4f7f\u65b0\u6743\u9650\u751f\u6548\u3002"));
    show_info_dialog(markup);
    g_free(markup);
}

static void on_info_cache_clicked(GtkButton *btn, gpointer user_data) {
    AdwStyleManager *sm = adw_style_manager_get_default();
    const char *color = adw_style_manager_get_dark(sm) ? "#00ffff" : "#00008b";
    char *markup = g_strdup_printf(
        "<b>%s</b>\n\n"
        "%s\n\n"
        "%s\n\n"
        "%s\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n"
        "<span font_family='monospace' foreground='%s'>%s</span>\n\n"
        "%s",
        info5("Bluetooth Device Cache Cleanup", "Limpeza do Cache de Dispositivos Bluetooth",
              "Limpieza de Cach\u00e9 de Dispositivos Bluetooth",
              "\u041e\u0447\u0438\u0441\u0442\u043a\u0430 \u043a\u044d\u0448\u0430 Bluetooth-\u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432",
              "\u84dd\u7259\u8bbe\u5907\u7f13\u5b58\u6e05\u9664"),
        info5(
            "The system stores information about previously paired devices in /var/lib/bluetooth/[MAC]/[device]/. If a device has been reset or its service profile has changed, this old cache can cause errors like \"Protocol not supported\" or \"Connection Refused\".",
            "O sistema armazena informa\u00e7\u00f5es de dispositivos pareados anteriormente em /var/lib/bluetooth/[MAC]/[dispositivo]/. Se um dispositivo foi resetado ou seu perfil de servi\u00e7o mudou, esse cache antigo pode causar erros como \"Protocolo n\u00e3o suportado\" ou \"Connection Refused\".",
            "El sistema almacena informaci\u00f3n de dispositivos previamente emparejados en /var/lib/bluetooth/[MAC]/[dispositivo]/. Si un dispositivo ha sido restablecido o su perfil de servicio ha cambiado, esta cach\u00e9 antigua puede causar errores como \"Protocolo no compatible\" o \"Connection Refused\".",
            "\u0421\u0438\u0441\u0442\u0435\u043c\u0430 \u0445\u0440\u0430\u043d\u0438\u0442 \u0438\u043d\u0444\u043e\u0440\u043c\u0430\u0446\u0438\u044e \u043e \u0440\u0430\u043d\u0435\u0435 \u0441\u043e\u043f\u0440\u044f\u0436\u0451\u043d\u043d\u044b\u0445 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0430\u0445 \u0432 /var/lib/bluetooth/[MAC]/[\u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u043e]/. \u0415\u0441\u043b\u0438 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u043e \u0431\u044b\u043b\u043e \u0441\u0431\u0440\u043e\u0448\u0435\u043d\u043e \u0438\u043b\u0438 \u0435\u0433\u043e \u043f\u0440\u043e\u0444\u0438\u043b\u044c \u0443\u0441\u043b\u0443\u0433\u0438 \u0438\u0437\u043c\u0435\u043d\u0438\u043b\u0441\u044f, \u044d\u0442\u043e\u0442 \u0441\u0442\u0430\u0440\u044b\u0439 \u043a\u044d\u0448 \u043c\u043e\u0436\u0435\u0442 \u0432\u044b\u0437\u044b\u0432\u0430\u0442\u044c \u043e\u0448\u0438\u0431\u043a\u0438, \u0442\u0430\u043a\u0438\u0435 \u043a\u0430\u043a \"\u041f\u0440\u043e\u0442\u043e\u043a\u043e\u043b \u043d\u0435 \u043f\u043e\u0434\u0434\u0435\u0440\u0436\u0438\u0432\u0430\u0435\u0442\u0441\u044f\" \u0438\u043b\u0438 \"Connection Refused\".",
            "\u7cfb\u7edf\u5c06\u4e4b\u524d\u914d\u5bf9\u8fc7\u7684\u8bbe\u5907\u7684\u4fe1\u606f\u5b58\u50a8\u5728/var/lib/bluetooth/[MAC]/[\u8bbe\u5907]/\u3002\u5982\u679c\u8bbe\u5907\u5df2\u91cd\u7f6e\u6216\u5176\u670d\u52a1\u914d\u7f6e\u6587\u4ef6\u5df2\u66f4\u6539\uff0c\u8fd9\u4e2a\u65e7\u7f13\u5b58\u53ef\u80fd\u4f1a\u5bfc\u81f4\u9519\u8bef\uff0c\u5982\u201c\u4e0d\u652f\u6301\u7684\u534f\u8bae\u201d\u6216\u201cConnection Refused\u201d\u3002"),
        info5(
            "This action stops the Bluetooth service, removes all device cache folders, and restarts the service, forcing a clean pairing from scratch.",
            "Esta a\u00e7\u00e3o para o servi\u00e7o Bluetooth, remove todas as pastas de cache de dispositivos e reinicia o servi\u00e7o, for\u00e7ando um pareamento limpo do zero.",
            "Esta acci\u00f3n detiene el servicio Bluetooth, elimina todas las carpetas de cach\u00e9 de dispositivos y reinicia el servicio, forzando un emparejamiento limpio desde cero.",
            "\u042d\u0442\u043e \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u0435 \u043e\u0441\u0442\u0430\u043d\u0430\u0432\u043b\u0438\u0432\u0430\u0435\u0442 \u0441\u043b\u0443\u0436\u0431\u0443 Bluetooth, \u0443\u0434\u0430\u043b\u044f\u0435\u0442 \u0432\u0441\u0435 \u043f\u0430\u043f\u043a\u0438 \u043a\u044d\u0448\u0430 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432 \u0438 \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a\u0430\u0435\u0442 \u0441\u043b\u0443\u0436\u0431\u0443, \u043e\u0431\u0435\u0441\u043f\u0435\u0447\u0438\u0432\u0430\u044f \u0447\u0438\u0441\u0442\u043e\u0435 \u0441\u043e\u043f\u0440\u044f\u0436\u0435\u043d\u0438\u0435 \u0441 \u043d\u0443\u043b\u044f.",
            "\u6b64\u64cd\u4f5c\u505c\u6b62\u84dd\u7259\u670d\u52a1\uff0c\u5220\u9664\u6240\u6709\u8bbe\u5907\u7f13\u5b58\u6587\u4ef6\u5939\uff0c\u5e76\u91cd\u542f\u670d\u52a1\uff0c\u5f3a\u5236\u4ece\u5934\u5f00\u59cb\u6e05\u6d01\u914d\u5bf9\u3002"),
        info5("Commands executed:", "Comandos Executados:", "Comandos ejecutados:", "\u0412\u044b\u043f\u043e\u043b\u043d\u044f\u0435\u043c\u044b\u0435 \u043a\u043e\u043c\u0430\u043d\u0434\u044b:", "\u6267\u884c\u7684\u547d\u4ee4:"),
        color, info5("\u2022 stop bluetooth", "\u2022 stop bluetooth", "\u2022 stop bluetooth", "\u2022 stop bluetooth", "\u2022 stop bluetooth"),
        color, info5("\u2022 rm -rf /var/lib/bluetooth/*/*", "\u2022 rm -rf /var/lib/bluetooth/*/*", "\u2022 rm -rf /var/lib/bluetooth/*/*", "\u2022 rm -rf /var/lib/bluetooth/*/*", "\u2022 rm -rf /var/lib/bluetooth/*/*"),
        color, info5("\u2022 start bluetooth", "\u2022 start bluetooth", "\u2022 start bluetooth", "\u2022 start bluetooth", "\u2022 start bluetooth"),
        info5("Attention: All paired devices will be removed and will need to be paired again.",
              "Aten\u00e7\u00e3o: Todos os dispositivos pareados ser\u00e3o removidos e precisar\u00e3o ser pareados novamente.",
              "Atenci\u00f3n: Todos los dispositivos emparejados se eliminar\u00e1n y ser\u00e1 necesario emparejarlos de nuevo.",
              "\u0412\u043d\u0438\u043c\u0430\u043d\u0438\u0435: \u0412\u0441\u0435 \u0441\u043e\u043f\u0440\u044f\u0436\u0451\u043d\u043d\u044b\u0435 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0430 \u0431\u0443\u0434\u0443\u0442 \u0443\u0434\u0430\u043b\u0435\u043d\u044b, \u0438 \u0438\u0445 \u043f\u0440\u0438\u0434\u0451\u0442\u0441\u044f \u0441\u043e\u043f\u0440\u044f\u0433\u0430\u0442\u044c \u0437\u0430\u043d\u043e\u0432\u043e.",
              "\u6ce8\u610f\uff1a\u6240\u6709\u5df2\u914d\u5bf9\u7684\u8bbe\u5907\u5c06\u88ab\u5220\u9664\uff0c\u5e76\u9700\u8981\u91cd\u65b0\u914d\u5bf9\u3002"));
    show_info_dialog(markup);
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
        gtk_label_set_text(GTK_LABEL(app_data.lbl_csr), _("Revert"));
        gtk_widget_remove_css_class(app_data.btn_csr, "suggested-action");
        gtk_widget_add_css_class(app_data.btn_csr, "destructive-action");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_csr), _("Apply"));
        gtk_widget_remove_css_class(app_data.btn_csr, "destructive-action");
        gtk_widget_add_css_class(app_data.btn_csr, "suggested-action");
    }

    if (g_file_test("/etc/modprobe.d/bluetooth-ertm.conf", G_FILE_TEST_EXISTS)) {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_ertm), _("Revert"));
        gtk_widget_remove_css_class(app_data.btn_ertm, "suggested-action");
        gtk_widget_add_css_class(app_data.btn_ertm, "destructive-action");
    } else {
        gtk_label_set_text(GTK_LABEL(app_data.lbl_ertm), _("Apply"));
        gtk_widget_remove_css_class(app_data.btn_ertm, "destructive-action");
        gtk_widget_add_css_class(app_data.btn_ertm, "suggested-action");
    }

    g_autofree char *fw_check = g_strdup_printf("%s/rtl_bt/rtl8761b_fw.bin", fw_path);
    if (g_file_test(fw_check, G_FILE_TEST_EXISTS)) {
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
    if (!gtk_widget_get_realized(app_data.window)) {
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
        g_snprintf(last_error_detail, sizeof(last_error_detail),
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
        g_warning("command_thread_func: failed to spawn: %s", error ? error->message : "unknown error");
    } else {
        g_subprocess_wait(proc, NULL, &error);
        ctx->status_code = g_subprocess_get_exit_status(proc);
    }
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
    char cmd[4096];
    if (g_file_test("/etc/modprobe.d/btusb.conf", G_FILE_TEST_EXISTS)) {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'rm -f /etc/modprobe.d/btusb.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, _("CSR Power config reverted!"), _("Error reverting."), csr_is_active, FALSE);
    } else {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'echo \"options btusb disable_scatternet=1 force_load_firmware=1 enable_autosuspend=0\" > /etc/modprobe.d/btusb.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, _("CSR Power config applied!"), _("Error applying."), csr_is_active, TRUE);
    }
}

static void on_fix_ertm_clicked(GtkButton *b, gpointer d) {
    char cmd[4096];
    if (g_file_test("/etc/modprobe.d/bluetooth-ertm.conf", G_FILE_TEST_EXISTS)) {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'rm -f /etc/modprobe.d/bluetooth-ertm.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, _("ERTM re-enabled!"), _("Error reverting."), ertm_is_active, FALSE);
    } else {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'echo \"options bluetooth disable_ertm=1\" > /etc/modprobe.d/bluetooth-ertm.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, _("ERTM disabled!"), _("Error disabling ERTM."), ertm_is_active, TRUE);
    }
}

static void on_fix_realtek_clicked(GtkButton *b, gpointer d) {
    char cmd[4096];
    g_autofree char *fw_check = g_strdup_printf("%s/rtl_bt/rtl8761b_fw.bin", fw_path);

    if (g_file_test(fw_check, G_FILE_TEST_EXISTS)) {
        g_snprintf(cmd, sizeof(cmd),
            "%s bash -c 'rm -f %s/rtl_bt/rtl8761b_fw.bin %s/rtl_bt/rtl8761b_config.bin && modprobe -r btusb && modprobe btusb'",
            priv_cmd, fw_path, fw_path);
        launch_async_action(GTK_WIDGET(b), cmd, _("Realtek firmware removed!"), _("Error removing."), realtek_is_active, FALSE);
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
        launch_async_action(GTK_WIDGET(b), cmd, _("Realtek firmware installed!"), _("Download error."), realtek_is_active, TRUE);
    }
}

static void on_fix_legacy_clicked(GtkButton *b, gpointer d) {
    char cmd[4096];
    g_autofree char *contents = NULL;
    gboolean is_legacy = FALSE;

    if (g_file_get_contents("/etc/bluetooth/main.conf", &contents, NULL, NULL))
        is_legacy = g_strrstr(contents, "Security=legacy") != NULL;

    if (is_legacy) {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'sed -i \"s/^Security=legacy/#Security=ssp/\" /etc/bluetooth/main.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, _("Legacy pairing disabled!"), _("Error reverting."), legacy_is_active, FALSE);
    } else {
        g_snprintf(cmd, sizeof(cmd), "%s bash -c 'sed -i \"/^[[:space:]]*Security=/d\" /etc/bluetooth/main.conf && echo \"Security=legacy\" >> /etc/bluetooth/main.conf && %s'", priv_cmd, restart_cmd);
        launch_async_action(GTK_WIDGET(b), cmd, _("Legacy pairing enabled!"), _("Error applying."), legacy_is_active, TRUE);
    }
}

static void on_fix_broadcom_clicked(GtkButton *b, gpointer d) {
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
    launch_async_action(GTK_WIDGET(b), cmd, _("Broadcom firmware installed!"), _("Error installing firmware."), broadcom_fw_is_active, TRUE);
}

static void on_fix_rfkill_clicked(GtkButton *b, gpointer d) {
    char cmd[4096];
    g_snprintf(cmd, sizeof(cmd), "%s bash -c 'rfkill unblock bluetooth && bluetoothctl power on'", priv_cmd);
    launch_async_action(GTK_WIDGET(b), cmd, _("Antenna unlocked!"), _("Execution error."), NULL, FALSE);
}

static void on_restart_service_clicked(GtkButton *b, gpointer d) {
    char cmd[4096];
    g_snprintf(cmd, sizeof(cmd), "%s %s", priv_cmd, restart_cmd);
    launch_async_action(GTK_WIDGET(b), cmd, _("Service restarted!"), _("Error restarting."), NULL, FALSE);
}

static void on_fix_perm_clicked(GtkButton *b, gpointer d) {
    char cmd[4096];
    const char *user = g_get_user_name();
    g_autofree char *quoted_user = g_shell_quote(user);
    g_snprintf(cmd, sizeof(cmd), "%s usermod -aG lp %s", priv_cmd, quoted_user);
    launch_async_action(GTK_WIDGET(b), cmd, _("Added to lp group! Restart session."), _("Error adding permissions."), NULL, FALSE);
}

static void on_fix_cache_clicked(GtkButton *b, gpointer d) {
    char cmd[4096];
    g_snprintf(cmd, sizeof(cmd), "%s bash -c '%s && rm -rf /var/lib/bluetooth/*/* && %s'", priv_cmd, stop_cmd, start_cmd);
    launch_async_action(GTK_WIDGET(b), cmd, _("Device cache cleared!"), _("Error clearing cache."), NULL, FALSE);
}

static gboolean delayed_scan(gpointer user_data) {
    scan_timeout_id = 0;
    if (scan_loader_row) {
        adw_preferences_group_remove(ADW_PREFERENCES_GROUP(app_data.devices_group), scan_loader_row);
        scan_loader_row = NULL;
    }
    scan_bluetooth_devices();
    return G_SOURCE_REMOVE;
}

static void on_scan_clicked(GtkButton *btn, gpointer user_data) {
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

static void on_info_copy_activated(AdwActionRow *row, gpointer user_data) {
    const char *text = adw_action_row_get_subtitle(row);
    const char *msg = user_data ? (const char *)user_data : _("Copied!");
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

static void query_bluetooth_version(void) {
    g_thread_new("bt-version", query_bt_version_thread, NULL);
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

/* Language menu actions */
static void on_lang_en(GSimpleAction *action, GVariant *param, gpointer d) { set_language(LANG_EN); }
static void on_lang_pt(GSimpleAction *action, GVariant *param, gpointer d) { set_language(LANG_PT); }
static void on_lang_es(GSimpleAction *action, GVariant *param, gpointer d) { set_language(LANG_ES); }
static void on_lang_ru(GSimpleAction *action, GVariant *param, gpointer d) { set_language(LANG_RU); }
static void on_lang_zh(GSimpleAction *action, GVariant *param, gpointer d) { set_language(LANG_ZH); }
static void on_lang_sys(GSimpleAction *action, GVariant *param, gpointer d) { set_language(LANG_SYS); }

/* Theme menu actions */
static void on_theme_system(GSimpleAction *a, GVariant *p, gpointer d) { set_theme(THEME_SYSTEM); }
static void on_theme_light(GSimpleAction *a, GVariant *p, gpointer d) { set_theme(THEME_LIGHT); }
static void on_theme_dark(GSimpleAction *a, GVariant *p, gpointer d) { set_theme(THEME_DARK); }

static void on_restart_app(GSimpleAction *action, GVariant *param, gpointer d) {
    const char *prgname = g_get_prgname() ? g_get_prgname() : "blufixer";
    const char *argv[] = {prgname, NULL};
    g_spawn_async(NULL, (char **)argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL);
    gtk_window_destroy(GTK_WINDOW(app_data.window));
    g_application_quit(G_APPLICATION(app_data.app));
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
        ".device-sub    { font-size: 13px; color: alpha(@theme_fg_color, 0.55); }"
        /* Botão de detalhes em toasts de erro (exclui botão fechar ×) */
        "toast button:not(.image-button) {"
        "  background: @error_bg_color;"
        "  color: @error_fg_color;"
        "  border-radius: 6px;"
        "  font-weight: bold;"
        "}"
        "button.update-btn { color: #00CCFE; }";
    
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
    static const struct { const char *name; const char *css; } table[] = {
        {"Realtek",             "badge-realtek"},
        {"Intel",               "badge-intel"},
        {"Qualcomm / Atheros",  "badge-qualcomm"},
        {"Broadcom / Cypress",  "badge-broadcom"},
        {"MediaTek",            "badge-mediatek"},
        {"Ralink",              "badge-ralink"},
        {"CSR",                 "badge-csr"},
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

typedef struct {
    char vendor[8];
    char product[8];
    char *desc;
    char manufacturer[32];
} ScanDevice;

static void scan_device_free(gpointer data) {
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

static void scan_bluetooth_devices(void) {
    if (g_atomic_int_get(&scanning)) return;

    for (GList *l = dynamic_rows; l != NULL; l = l->next) {
        adw_preferences_group_remove(ADW_PREFERENCES_GROUP(app_data.devices_group), GTK_WIDGET(l->data));
    }
    g_list_free(dynamic_rows);
    dynamic_rows = NULL;

    g_atomic_int_set(&scanning, TRUE);
    g_thread_new("bt-scan", scan_bt_devices_thread, NULL);
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
    if (askpass_path[0]) {
        int ret = unlink(askpass_path);
        (void)ret;
    }
}

static void detect_system_tools(void) {
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

/* =========================================================================
   9. CONSTRUÇÃO DA INTERFACE UNIFICADA E MONOLÍTICA
   ========================================================================= */
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
        { "lang_sys", on_lang_sys, NULL, NULL, NULL, {0} },
        { "lang_en", on_lang_en, NULL, NULL, NULL, {0} },
        { "lang_pt", on_lang_pt, NULL, NULL, NULL, {0} },
        { "lang_es", on_lang_es, NULL, NULL, NULL, {0} },
        { "lang_ru", on_lang_ru, NULL, NULL, NULL, {0} },
        { "lang_zh", on_lang_zh, NULL, NULL, NULL, {0} },
        { "theme_system", on_theme_system, NULL, NULL, NULL, {0} },
        { "theme_light", on_theme_light, NULL, NULL, NULL, {0} },
        { "theme_dark", on_theme_dark, NULL, NULL, NULL, {0} },
    };
    g_action_map_add_action_entries(G_ACTION_MAP(actions), entries, G_N_ELEMENTS(entries), NULL);
    gtk_widget_insert_action_group(app_data.window, "win", G_ACTION_GROUP(actions));
    
    app_data.main_menu = g_menu_new();
    app_data.lang_section = g_menu_new();
    rebuild_language_menu();
    app_data.theme_section = g_menu_new();
    rebuild_theme_menu();

    GMenu *lang_menu = g_menu_new();
    g_menu_append_submenu(lang_menu, _("Language"), G_MENU_MODEL(app_data.lang_section));
    GMenu *theme_menu = g_menu_new();
    g_menu_append_submenu(theme_menu, _("Theme"), G_MENU_MODEL(app_data.theme_section));

    g_menu_append_section(app_data.main_menu, NULL, G_MENU_MODEL(lang_menu));
    g_menu_append_section(app_data.main_menu, NULL, G_MENU_MODEL(theme_menu));
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
    gtk_widget_set_size_request(btn_scan, -1, 34);
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
        {_("CSR Energy Fix"), _("Recommended for CSR and Barrot/Generic dongles. Stabilizes the radio to fix detection and connection loops."), G_CALLBACK(on_info_csr_clicked), G_CALLBACK(on_fix_csr_clicked), 0},
        {_("Disable ERTM for Gamepads"), _("Fixes automatic disconnections of modern Bluetooth gamepads."), G_CALLBACK(on_info_ertm_clicked), G_CALLBACK(on_fix_ertm_clicked), 0},
        {_("Force Legacy Pairing Mode"), _("Allows old Bluetooth devices to pair with a fixed PIN."), G_CALLBACK(on_info_legacy_clicked), G_CALLBACK(on_fix_legacy_clicked), 0},
        {_("Install Realtek Firmware (RTL8761B)"), _("Downloads and injects the missing official driver binaries."), G_CALLBACK(on_info_realtek_clicked), G_CALLBACK(on_fix_realtek_clicked), 0},
        {_("Install Broadcom/Cypress Firmware (b43)"), _("Downloads and extracts the missing proprietary firmware for Broadcom chipsets."), G_CALLBACK(on_info_broadcom_clicked), G_CALLBACK(on_fix_broadcom_clicked), 0},
        {_("Unblock Antenna"), _("Forces activation of adapters stuck in Airplane Mode."), G_CALLBACK(on_info_rfkill_clicked), G_CALLBACK(on_fix_rfkill_clicked), 1},
        {_("Add user permissions"), _("Adds the user to the lp group for Bluetooth D-Bus access."), G_CALLBACK(on_info_perm_clicked), G_CALLBACK(on_fix_perm_clicked), 1},
        {_("Clear device cache"), _("Removes all pairing caches to fix connection errors."), G_CALLBACK(on_info_cache_clicked), G_CALLBACK(on_fix_cache_clicked), 1},
        {_("Restart Bluetooth Service"), _("Clears caches and buffers by restarting the system service."), G_CALLBACK(on_info_restart_clicked), G_CALLBACK(on_restart_service_clicked), 1}
    };

    for(size_t i = 0; i < G_N_ELEMENTS(rows); i++) {
        GtkWidget *row = adw_action_row_new();
        adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), rows[i].t);
        adw_action_row_set_subtitle(ADW_ACTION_ROW(row), rows[i].sub);
        
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        GtkWidget *info = gtk_button_new_from_icon_name("dialog-information-symbolic");
        gtk_widget_add_css_class(info, "flat");
        gtk_widget_set_size_request(info, -1, 34);
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

    UpdateCheckData *check = g_new0(UpdateCheckData, 1);
    check->update_btn = app_data.update_btn;
    g_thread_new("check-update", check_update_thread, check);

    gtk_window_present(GTK_WINDOW(app_data.window));
}

int main(int argc, char **argv) {
    g_autoptr(AdwApplication) app = adw_application_new("org.renanmayrinck.blufixer", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}