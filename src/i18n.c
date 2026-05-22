#include "blufixer.h"

static const TransEntry trans_table[] = {
    {"BluFixer",                             "BluFixer",                         "BluFixer",                         "BluFixer",                         "\u84dd\u4fee\u590d"},
    {"About",                                "Sobre",                            "Acerca",                           "\u041e \u043f\u0440\u043e\u0433\u0440\u0430\u043c\u043c\u0435",                      "\u5173\u4e8e"},
    {"GitHub Repository",                    "Reposit\u00f3rio no GitHub",            "Repositorio en GitHub",            "\u0420\u0435\u043f\u043e\u0437\u0438\u0442\u043e\u0440\u0438\u0439 \u043d\u0430 GitHub",            "GitHub\u4ed3\u5e93"},
    {"Donate",                               "Doar ao autor",                    "Donar al autor",                   "\u041f\u043e\u0436\u0435\u0440\u0442\u0432\u043e\u0432\u0430\u0442\u044c",                     "\u6350\u8d60"},
    {"Language",                             "Idioma",                           "Idioma",                           "\u042f\u0437\u044b\u043a",                             "\u8bed\u8a00"},
    {"Scan",                                 "Escanear",                         "Escanear",                         "\u0421\u043a\u0430\u043d\u0438\u0440\u043e\u0432\u0430\u0442\u044c",                      "\u626b\u63cf"},
    {"Scanning devices...",                  "Escaneando dispositivos...",       "Escaneando dispositivos...",       "\u0421\u043a\u0430\u043d\u0438\u0440\u043e\u0432\u0430\u043d\u0438\u0435 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432...",        "\u6b63\u5728\u626b\u63cf\u8bbe\u5907..."},
    {"No Bluetooth devices connected or functional at the moment",
                                             "N\u00e3o h\u00e1 dispositivos bluetooth conectados ou funcionais no momento",
                                                                            "No hay dispositivos Bluetooth conectados o funcionales",
                                                                                                                    "\u041d\u0435\u0442 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u043d\u044b\u0445 \u0438\u043b\u0438 \u0440\u0430\u0431\u043e\u0442\u0430\u044e\u0449\u0438\u0445 Bluetooth-\u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432",
                                                                                                                                                        "\u76ee\u524d\u6ca1\u6709\u8fde\u63a5\u6216\u53ef\u7528\u7684\u84dd\u7259\u8bbe\u5907"},
    {"Select",                               "Selecionar",                       "Seleccionar",                      "\u0412\u044b\u0431\u0440\u0430\u0442\u044c",                          "\u9009\u62e9"},

    {"Device Name",                          "Nome do dispositivo",              "Nombre del dispositivo",           "\u0418\u043c\u044f \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0430",                   "\u8bbe\u5907\u540d\u79f0"},
    {"Possible Manufacturer",                "Poss\u00edvel fabricante",              "Posible fabricante",               "\u0412\u043e\u0437\u043c\u043e\u0436\u043d\u044b\u0439 \u043f\u0440\u043e\u0438\u0437\u0432\u043e\u0434\u0438\u0442\u0435\u043b\u044c",           "\u53ef\u80fd\u7684\u5236\u9020\u5546"},
    {"Hardware ID",                          "Identificador de Hardware (ID)",   "Identificador de Hardware (ID)",   "\u0418\u0434\u0435\u043d\u0442\u0438\u0444\u0438\u043a\u0430\u0442\u043e\u0440 \u043e\u0431\u043e\u0440\u0443\u0434\u043e\u0432\u0430\u043d\u0438\u044f (ID)",  "\u786c\u4ef6ID"},
    {"Bluetooth",                            "Bluetooth",                         "Bluetooth",                         "Bluetooth",                         "\u84dd\u7259"},
    {"Bluetooth Version",                    "Vers\u00e3o do Bluetooth",               "Versi\u00f3n de Bluetooth",              "\u0412\u0435\u0440\u0441\u0438\u044f Bluetooth",                  "\u84dd\u7259\u7248\u672c"},
    {"Unknown",                              "Desconhecida",                     "Desconocida",                      "\u041d\u0435\u0438\u0437\u0432\u0435\u0441\u0442\u043d\u043e",                       "\u672a\u77e5"},

    {"Device List",                          "Lista de adaptadores",             "Lista de adaptadores",             "\u0421\u043f\u0438\u0441\u043e\u043a \u0430\u0434\u0430\u043f\u0442\u0435\u0440\u043e\u0432",                 "\u8bbe\u5907\u5217\u8868"},
    {"Select the adapter you want to modify","Selecione o adaptador que deseja modificar",
                                                                            "Seleccione el adaptador a modificar",
                                                                                                                    "\u0412\u044b\u0431\u0435\u0440\u0438\u0442\u0435 \u0430\u0434\u0430\u043f\u0442\u0435\u0440 \u0434\u043b\u044f \u0438\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u044f",   "\u9009\u62e9\u8981\u4fee\u6539\u7684\u9002\u914d\u5668"},

    {"System Fixes",                         "Corre\u00e7\u00f5es de Sistema",             "Correcciones del Sistema",         "\u0418\u0441\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u044f \u0441\u0438\u0441\u0442\u0435\u043c\u044b",              "\u7cfb\u7edf\u4fee\u590d"},
    {"Immediate Actions",                    "A\u00e7\u00f5es Imediatas",                  "Acciones Inmediatas",              "\u041d\u0435\u043c\u0435\u0434\u043b\u0435\u043d\u043d\u044b\u0435 \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u044f",             "\u7acb\u5373\u64cd\u4f5c"},
    {"Applies configuration file changes or driver/firmware downloads. Some settings are hardware-specific and all can be reverted.",
                                             "Executa modifica\u00e7\u00f5es de arquivos de configura\u00e7\u00e3o ou download de drivers e firmware, se necess\u00e1rio. Em alguns casos as configura\u00e7\u00f5es ser\u00e3o espec\u00edficas para o hardware e todas podem ser revertidas.",
                                                                            "Realiza modificaciones de archivos de configuraci\u00f3n o descarga de controladores y firmware si es necesario. Algunas configuraciones son espec\u00edficas del hardware y todas pueden revertirse.",
                                                                                                                    "\u041f\u0440\u0438\u043c\u0435\u043d\u044f\u0435\u0442 \u0438\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u044f \u043a\u043e\u043d\u0444\u0438\u0433\u0443\u0440\u0430\u0446\u0438\u043e\u043d\u043d\u044b\u0445 \u0444\u0430\u0439\u043b\u043e\u0432 \u0438\u043b\u0438 \u0437\u0430\u0433\u0440\u0443\u0437\u043a\u0443 \u0434\u0440\u0430\u0439\u0432\u0435\u0440\u043e\u0432/\u043f\u0440\u043e\u0448\u0438\u0432\u043e\u043a. \u041d\u0435\u043a\u043e\u0442\u043e\u0440\u044b\u0435 \u043d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438 \u0441\u043f\u0435\u0446\u0438\u0444\u0438\u0447\u043d\u044b \u0434\u043b\u044f \u043e\u0431\u043e\u0440\u0443\u0434\u043e\u0432\u0430\u043d\u0438\u044f \u0438 \u0432\u0441\u0435 \u043c\u043e\u0433\u0443\u0442 \u0431\u044b\u0442\u044c \u043e\u0442\u043c\u0435\u043d\u0435\u043d\u044b.",
                                                                                                                                                        "\u5e94\u7528\u914d\u7f6e\u6587\u4ef6\u4fee\u6539\u6216\u9a71\u52a8/\u56fa\u4ef6\u4e0b\u8f7d\u3002\u67d0\u4e9b\u8bbe\u7f6e\u662f\u786c\u4ef6\u7279\u5b9a\u7684\uff0c\u5e76\u4e14\u5168\u90e8\u53ef\u4ee5\u8fd8\u539f\u3002"},
    {"Direct commands applied to hardware or system services. Cannot be reverted (only repeated).",
                                             "Comandos diretos aplicados ao hardware ou servi\u00e7os do sistema. N\u00e3o podem ser revertidos (apenas repetidos).",
                                                                            "Comandos directos aplicados al hardware o servicios del sistema. No se pueden revertir (solo repetir).",
                                                                                                                    "\u041f\u0440\u044f\u043c\u044b\u0435 \u043a\u043e\u043c\u0430\u043d\u0434\u044b \u043a \u043e\u0431\u043e\u0440\u0443\u0434\u043e\u0432\u0430\u043d\u0438\u044e \u0438\u043b\u0438 \u0441\u0438\u0441\u0442\u0435\u043c\u043d\u044b\u043c \u0441\u043b\u0443\u0436\u0431\u0430\u043c. \u041d\u0435 \u043c\u043e\u0433\u0443\u0442 \u0431\u044b\u0442\u044c \u043e\u0442\u043c\u0435\u043d\u0435\u043d\u044b (\u0442\u043e\u043b\u044c\u043a\u043e \u043f\u043e\u0432\u0442\u043e\u0440\u0435\u043d\u044b).",
                                                                                                                                                        "\u76f4\u63a5\u5e94\u7528\u4e8e\u786c\u4ef6\u6216\u7cfb\u7edf\u670d\u52a1\u7684\u547d\u4ee4\u3002\u4e0d\u80fd\u64a4\u9500\uff08\u53ea\u80fd\u91cd\u590d\uff09\u3002"},

    {"CSR Energy Fix",                       "Corre\u00e7\u00e3o de Energia CSR",           "Correcci\u00f3n de energ\u00eda CSR",            "\u0418\u0441\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0435 \u044d\u043d\u0435\u0440\u0433\u043e\u043f\u043e\u0442\u0440\u0435\u0431\u043b\u0435\u043d\u0438\u044f CSR","CSR\u80fd\u6e90\u4fee\u590d"},
    {"Disable ERTM for Gamepads",           "Desativar ERTM para Gamepads",      "Desactivar ERTM para Gamepads",    "\u041e\u0442\u043a\u043b\u044e\u0447\u0438\u0442\u044c ERTM \u0434\u043b\u044f \u0433\u0435\u0439\u043c\u043f\u0430\u0434\u043e\u0432",     "\u4e3a\u624b\u67c4\u7981\u7528ERTM"},
    {"Force Legacy Pairing Mode",           "For\u00e7ar pareamento em modo legado",  "Forzar emparejamiento en modo legado",
                                                                                                                    "\u041f\u0440\u0438\u043d\u0443\u0434\u0438\u0442\u0435\u043b\u044c\u043d\u044b\u0439 \u0440\u0435\u0436\u0438\u043c \u0443\u0441\u0442\u0430\u0440\u0435\u0432\u0448\u0435\u0433\u043e \u0441\u043e\u043f\u0440\u044f\u0436\u0435\u043d\u0438\u044f",
                                                                                                                                                        "\u5f3a\u5236\u4f20\u7edf\u914d\u5bf9\u6a21\u5f0f"},
    {"Install Realtek Firmware (RTL8761B)",  "Instalar Firmware Realtek (RTL8761B)",
                                                                            "Instalar firmware Realtek (RTL8761B)",
                                                                                                                    "\u0423\u0441\u0442\u0430\u043d\u043e\u0432\u0438\u0442\u044c \u043f\u0440\u043e\u0448\u0438\u0432\u043a\u0443 Realtek (RTL8761B)",
                                                                                                                                                        "\u5b89\u88c5Realtek\u56fa\u4ef6(RTL8761B)"},
    {"Install Broadcom/Cypress Firmware (b43)",
                                             "Instalar Firmware Broadcom/Cypress (b43)",
                                                                            "Instalar firmware Broadcom/Cypress (b43)",
                                                                                                                    "\u0423\u0441\u0442\u0430\u043d\u043e\u0432\u0438\u0442\u044c \u043f\u0440\u043e\u0448\u0438\u0432\u043a\u0443 Broadcom/Cypress (b43)",
                                                                                                                                                        "\u5b89\u88c5Broadcom/Cypress\u56fa\u4ef6(b43)"},
    {"Unblock Antenna",                      "Descongestionar",                  "Desbloquear antena",               "\u0420\u0430\u0437\u0431\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u0430\u0442\u044c \u0430\u043d\u0442\u0435\u043d\u043d\u0443",           "\u89e3\u9501\u5929\u7ebf"},
    {"Add user permissions",                 "Adicionar permiss\u00f5es ao usu\u00e1rio atual",
                                                                            "Agregar permisos al usuario actual",
                                                                                                                    "\u0414\u043e\u0431\u0430\u0432\u0438\u0442\u044c \u0440\u0430\u0437\u0440\u0435\u0448\u0435\u043d\u0438\u044f \u0442\u0435\u043a\u0443\u0449\u0435\u043c\u0443 \u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u0435\u043b\u044e",
                                                                                                                                                        "\u6dfb\u52a0\u5f53\u524d\u7528\u6237\u6743\u9650"},
    {"Clear device cache",                   "Limpar o cache de dispositivos",   "Limpiar cach\u00e9 de dispositivos",    "\u041e\u0447\u0438\u0441\u0442\u0438\u0442\u044c \u043a\u044d\u0448 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432",           "\u6e05\u9664\u8bbe\u5907\u7f13\u5b58"},
    {"Restart Bluetooth Service",           "Reiniciar Servi\u00e7o Bluetooth",       "Reiniciar Servicio Bluetooth",     "\u041f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u044c \u0441\u043b\u0443\u0436\u0431\u0443 Bluetooth",   "\u91cd\u542f\u84dd\u7259\u670d\u52a1"},

    {"Apply",                                "Aplicar",                          "Aplicar",                          "\u041f\u0440\u0438\u043c\u0435\u043d\u0438\u0442\u044c",                        "\u5e94\u7528"},
    {"Revert",                               "Reverter",                         "Revertir",                         "\u041e\u0442\u043c\u0435\u043d\u0438\u0442\u044c",                         "\u8fd8\u539f"},
    {"Install",                              "Instalar",                         "Instalar",                         "\u0423\u0441\u0442\u0430\u043d\u043e\u0432\u0438\u0442\u044c",                       "\u5b89\u88c5"},
    {"Remove",                               "Remover",                          "Remover",                          "\u0423\u0434\u0430\u043b\u0438\u0442\u044c",                          "\u79fb\u9664"},
    {"Installed",                            "Instalado",                        "Instalado",                        "\u0423\u0441\u0442\u0430\u043d\u043e\u0432\u043b\u0435\u043d\u043e",                      "\u5df2\u5b89\u88c5"},
    {"Run",                                  "Executar",                         "Ejecutar",                         "\u0412\u044b\u043f\u043e\u043b\u043d\u0438\u0442\u044c",                        "\u6267\u884c"},
    {"Got it",                               "Entendido",                        "Entendido",                        "\u041f\u043e\u043d\u044f\u0442\u043d\u043e",                          "\u77e5\u9053\u4e86"},
    {"Close",                                "Fechar",                           "Cerrar",                           "\u0417\u0430\u043a\u0440\u044b\u0442\u044c",                          "\u5173\u95ed"},
    {"View details",                         "Ver detalhes",                     "Ver detalles",                     "\u041f\u043e\u0434\u0440\u043e\u0431\u043d\u0435\u0435",                        "\u67e5\u770b\u8be6\u60c5"},
    {"Copied!",                              "Copiado!",                         "\u00a1Copiado!",                        "\u0421\u043a\u043e\u043f\u0438\u0440\u043e\u0432\u0430\u043d\u043e!",                     "\u5df2\u590d\u5236!"},

    {"Technical Details",                    "Detalhamento T\u00e9cnico",             "Detalles T\u00e9cnicos",                "\u0422\u0435\u0445\u043d\u0438\u0447\u0435\u0441\u043a\u0438\u0435 \u0434\u0435\u0442\u0430\u043b\u0438",               "\u6280\u672f\u7ec6\u8282"},
    {"Operation failed",                     "Falha na opera\u00e7\u00e3o",                "Fallo en la operaci\u00f3n",            "\u041e\u0448\u0438\u0431\u043a\u0430 \u043e\u043f\u0435\u0440\u0430\u0446\u0438\u0438",                   "\u64cd\u4f5c\u5931\u8d25"},
    {"Device Name copied",                   "Nome completo copiado",            "Nombre completo copiado",          "\u0418\u043c\u044f \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0430 \u0441\u043a\u043e\u043f\u0438\u0440\u043e\u0432\u0430\u043d\u043e",        "\u8bbe\u5907\u540d\u79f0\u5df2\u590d\u5236"},
    {"Manufacturer copied",                  "Nome do fabricante copiado",       "Nombre del fabricante copiado",    "\u041f\u0440\u043e\u0438\u0437\u0432\u043e\u0434\u0438\u0442\u0435\u043b\u044c \u0441\u043a\u043e\u043f\u0438\u0440\u043e\u0432\u0430\u043d",          "\u5236\u9020\u5546\u5df2\u590d\u5236"},
    {"Hardware ID: %s:%s",                   "ID de Hardware: %s:%s",             "ID de Hardware: %s:%s",             "ID \u043e\u0431\u043e\u0440\u0443\u0434\u043e\u0432\u0430\u043d\u0438\u044f: %s:%s",            "\u786c\u4ef6ID: %s:%s"},
    {"Hardware ID copied",                   "Hardware ID copiado",              "ID de Hardware copiado",           "ID \u043e\u0431\u043e\u0440\u0443\u0434\u043e\u0432\u0430\u043d\u0438\u044f \u0441\u043a\u043e\u043f\u0438\u0440\u043e\u0432\u0430\u043d",        "\u786c\u4ef6ID\u5df2\u590d\u5236"},
    {"Device Tech Sheet",                    "Ficha T\u00e9cnica do Adaptador",       "Ficha T\u00e9cnica del Adaptador",      "\u0422\u0435\u0445\u043d\u0438\u0447\u0435\u0441\u043a\u0438\u0435 \u0445\u0430\u0440\u0430\u043a\u0442\u0435\u0440\u0438\u0441\u0442\u0438\u043a\u0438",        "\u8bbe\u5907\u6280\u672f\u53c2\u6570"},
    {"Bluetooth version is detected via bluetoothctl, btmgmt or hciconfig.",
                                             "A vers\u00e3o Bluetooth \u00e9 detectada via bluetoothctl, btmgmt ou hciconfig",
                                                                            "La versi\u00f3n Bluetooth se detecta mediante bluetoothctl, btmgmt o hciconfig.",
                                                                                                                    "\u0412\u0435\u0440\u0441\u0438\u044f Bluetooth \u043e\u043f\u0440\u0435\u0434\u0435\u043b\u044f\u0435\u0442\u0441\u044f \u0447\u0435\u0440\u0435\u0437 bluetoothctl, btmgmt \u0438\u043b\u0438 hciconfig.",
                                                                                                                                                        "\u84dd\u7259\u7248\u672c\u901a\u8fc7bluetoothctl\u3001btmgmt\u6216hciconfig\u68c0\u6d4b\u3002"},

    {"CSR Power config reverted!",           "Corre\u00e7\u00e3o de energia revertida!",   "\u00a1Configuraci\u00f3n CSR revertida!",    "\u041a\u043e\u043d\u0444\u0438\u0433\u0443\u0440\u0430\u0446\u0438\u044f CSR \u043e\u0442\u043c\u0435\u043d\u0435\u043d\u0430!",        "CSR\u914d\u7f6e\u5df2\u8fd8\u539f!"},
    {"CSR Power config applied!",            "Corre\u00e7\u00e3o de energia aplicada!",    "\u00a1Correcci\u00f3n CSR aplicada!",        "\u0418\u0441\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0435 CSR \u043f\u0440\u0438\u043c\u0435\u043d\u0435\u043d\u043e!",        "CSR\u4fee\u590d\u5df2\u5e94\u7528!"},
    {"ERTM re-enabled!",                     "ERTM ativado!",                    "\u00a1ERTM reactivado con \u00e9xito!",      "ERTM \u043f\u043e\u0432\u0442\u043e\u0440\u043d\u043e \u0432\u043a\u043b\u044e\u0447\u0451\u043d!",            "ERTM\u5df2\u91cd\u65b0\u542f\u7528!"},
    {"ERTM disabled!",                       "ERTM desativado!",                 "ERTM desactivado!",                "ERTM \u043e\u0442\u043a\u043b\u044e\u0447\u0451\u043d!",                            "ERTM\u5df2\u7981\u7528!"},
    {"Realtek firmware removed!",            "Firmware Realtek removido!",       "\u00a1Firmware Realtek eliminado!",     "\u041f\u0440\u043e\u0448\u0438\u0432\u043a\u0430 Realtek \u0443\u0434\u0430\u043b\u0435\u043d\u0430!",         "Realtek\u56fa\u4ef6\u5df2\u79fb\u9664!"},
    {"Realtek firmware installed!",          "Firmware Realtek instalado!",      "\u00a1Firmware Realtek instalado!",     "\u041f\u0440\u043e\u0448\u0438\u0432\u043a\u0430 Realtek \u0443\u0441\u0442\u0430\u043d\u043e\u0432\u043b\u0435\u043d\u0430!",     "Realtek\u56fa\u4ef6\u5df2\u5b89\u88c5!"},
    {"Legacy pairing disabled!",             "Pareamento legado desativado!",    "\u00a1Emparejamiento heredado desactivado!",
                                                                                                                    "\u0423\u0441\u0442\u0430\u0440\u0435\u0432\u0448\u0435\u0435 \u0441\u043e\u043f\u0440\u044f\u0436\u0435\u043d\u0438\u0435 \u043e\u0442\u043a\u043b\u044e\u0447\u0435\u043d\u043e!",  "\u4f20\u7edf\u914d\u5bf9\u5df2\u7981\u7528!"},
    {"Legacy pairing enabled!",              "Pareamento legado ativado!",       "\u00a1Emparejamiento heredado activado!",
                                                                                                                    "\u0423\u0441\u0442\u0430\u0440\u0435\u0432\u0448\u0435\u0435 \u0441\u043e\u043f\u0440\u044f\u0436\u0435\u043d\u0438\u0435 \u0432\u043a\u043b\u044e\u0447\u0435\u043d\u043e!",   "\u4f20\u7edf\u914d\u5bf9\u5df2\u542f\u7528!"},
    {"Broadcom firmware installed!",         "Firmware Broadcom instalado!",     "\u00a1Firmware Broadcom instalado!",    "\u041f\u0440\u043e\u0448\u0438\u0432\u043a\u0430 Broadcom \u0443\u0441\u0442\u0430\u043d\u043e\u0432\u043b\u0435\u043d\u0430!",    "Broadcom\u56fa\u4ef6\u5df2\u5b89\u88c5!"},
    {"Antenna unlocked!",                    "Antena desbloqueada!",              "\u00a1Antena desbloqueada!",            "\u0410\u043d\u0442\u0435\u043d\u043d\u0430 \u0440\u0430\u0437\u0431\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u0430\u043d\u0430!",           "\u5929\u7ebf\u5df2\u89e3\u9501!"},
    {"Service restarted!",                   "Servi\u00e7o Bluetooth reiniciado!",    "\u00a1Servicio Bluetooth reiniciado!",  "\u0421\u043b\u0443\u0436\u0431\u0430 Bluetooth \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0449\u0435\u043d\u0430!",    "\u84dd\u7259\u670d\u52a1\u5df2\u91cd\u542f!"},
    {"Added to lp group! Restart session.",  "Adicionado ao grupo lp! Reinicie a sess\u00e3o.",
                                                                            "\u00a1Agregado al grupo lp! Reinicie la sesi\u00f3n.",
                                                                                                                    "\u0414\u043e\u0431\u0430\u0432\u043b\u0435\u043d \u0432 \u0433\u0440\u0443\u043f\u043f\u0443 lp! \u041f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u0435 \u0441\u0435\u0430\u043d\u0441.",
                                                                                                                                                        "\u5df2\u6dfb\u52a0\u5230lp\u7ec4\uff01\u8bf7\u91cd\u65b0\u542f\u52a8\u4f1a\u8bdd\u3002"},
    {"Device cache cleared!",                "Cache de dispositivos limpo!",     "\u00a1Cach\u00e9 de dispositivos limpiado!", "\u041a\u044d\u0448 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432 \u043e\u0447\u0438\u0449\u0435\u043d!",             "\u8bbe\u5907\u7f13\u5b58\u5df2\u6e05\u9664!"},

    {"Error reverting.",                     "Erro ao reverter.",                "Error al revertir.",               "\u041e\u0448\u0438\u0431\u043a\u0430 \u043e\u0442\u043c\u0435\u043d\u044b.",                            "\u8fd8\u539f\u9519\u8bef\u3002"},
    {"Error applying.",                      "Erro ao aplicar.",                 "Error al aplicar.",                "\u041e\u0448\u0438\u0431\u043a\u0430 \u043f\u0440\u0438\u043c\u0435\u043d\u0435\u043d\u0438\u044f.",                        "\u5e94\u7528\u9519\u8bef\u3002"},
    {"Download error.",                      "Erro no download.",                "Error de descarga.",               "\u041e\u0448\u0438\u0431\u043a\u0430 \u0437\u0430\u0433\u0440\u0443\u0437\u043a\u0438.",                          "\u4e0b\u8f7d\u9519\u8bef\u3002"},
    {"Error removing.",                      "Erro ao remover.",                 "Error al eliminar.",               "\u041e\u0448\u0438\u0431\u043a\u0430 \u0443\u0434\u0430\u043b\u0435\u043d\u0438\u044f.",                          "\u79fb\u9664\u9519\u8bef\u3002"},
    {"Error unlocking.",                     "Erro ao desbloquear.",             "Error al desbloquear.",            "\u041e\u0448\u0438\u0431\u043a\u0430 \u0440\u0430\u0437\u0431\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u043a\u0438.",             "\u89e3\u9501\u9519\u8bef\u3002"},
    {"Error restarting.",                    "Erro ao reiniciar.",               "Error al reiniciar.",              "\u041e\u0448\u0438\u0431\u043a\u0430 \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a\u0430.",               "\u91cd\u542f\u9519\u8bef\u3002"},
    {"Error adding permissions.",            "Erro ao adicionar.",               "Error al agregar.",                "\u041e\u0448\u0438\u0431\u043a\u0430 \u0434\u043e\u0431\u0430\u0432\u043b\u0435\u043d\u0438\u044f.",                     "\u6dfb\u52a0\u9519\u8bef\u3002"},
    {"Error clearing cache.",                "Erro ao limpar cache.",            "Error al limpiar cach\u00e9.",          "\u041e\u0448\u0438\u0431\u043a\u0430 \u043e\u0447\u0438\u0441\u0442\u043a\u0438 \u043a\u044d\u0448\u0430.",              "\u6e05\u9664\u7f13\u5b58\u9519\u8bef\u3002"},
    {"Error installing firmware.",           "Erro ao instalar firmware.",       "Error al instalar firmware.",      "\u041e\u0448\u0438\u0431\u043a\u0430 \u0443\u0441\u0442\u0430\u043d\u043e\u0432\u043a\u0438 \u043f\u0440\u043e\u0448\u0438\u0432\u043a\u0438.",        "\u56fa\u4ef6\u5b89\u88c5\u9519\u8bef\u3002"},
    {"Error disabling ERTM.",                "Erro ao desativar ERTM.",          "Error al desactivar ERTM.",        "\u041e\u0448\u0438\u0431\u043a\u0430 \u043e\u0442\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f ERTM.",        "\u7981\u7528ERTM\u9519\u8bef\u3002"},
    {"Execution error.",                     "Erro ao executar.",               "Error al ejecutar.",               "\u041e\u0448\u0438\u0431\u043a\u0430 \u0432\u044b\u043f\u043e\u043b\u043d\u0435\u043d\u0438\u044f.",                "\u6267\u884c\u9519\u8bef\u3002"},
    {"Package manager not recognized.",      "Gerenciador de pacotes n\u00e3o reconhecido.",
                                                                            "Gestor de paquetes no reconocido.",
                                                                                                                    "\u041c\u0435\u043d\u0435\u0434\u0436\u0435\u0440 \u043f\u0430\u043a\u0435\u0442\u043e\u0432 \u043d\u0435 \u0440\u0430\u0441\u043f\u043e\u0437\u043d\u0430\u043d.",    "\u672a\u8bc6\u522b\u7684\u5305\u7ba1\u7406\u5668\u3002"},
    {"Checking for updates...",              "Verificando atualiza\u00e7\u00f5es...",      "Buscando actualizaciones...",
                                                                                                                    "\u041f\u0440\u043e\u0432\u0435\u0440\u043a\u0430 \u043e\u0431\u043d\u043e\u0432\u043b\u0435\u043d\u0438\u0439...",            "\u6b63\u5728\u68c0\u67e5\u66f4\u65b0..."},
    {"Website",                              "Site",                             "Sitio web",                        "\u0412\u0435\u0431-\u0441\u0430\u0439\u0442",                          "\u7f51\u7ad9"},
    {"Update to version %s",                 "Atualizar para vers\u00e3o %s",         "Actualizar para versi\u00f3n %s",       "\u041e\u0431\u043d\u043e\u0432\u0438\u0442\u044c \u0434\u043e \u0432\u0435\u0440\u0441\u0438\u0438 %s",             "\u66f4\u65b0\u5230\u7248\u672c %s"},
    {"Could not check for updates",          "N\u00e3o foi poss\u00edvel verificar atualiza\u00e7\u00f5es",
                                                                            "No se pudo buscar actualizaciones",
                                                                                                                    "\u041d\u0435 \u0443\u0434\u0430\u043b\u043e\u0441\u044c \u043f\u0440\u043e\u0432\u0435\u0440\u0438\u0442\u044c \u043e\u0431\u043d\u043e\u0432\u043b\u0435\u043d\u0438\u044f",   "\u65e0\u6cd5\u68c0\u67e5\u66f4\u65b0"},
    {"No elevation method found (pkexec/sudo). Fixes will not work.",
                                             "Nenhum m\u00e9todo de eleva\u00e7\u00e3o encontrado (pkexec/sudo). As corre\u00e7\u00f5es n\u00e3o funcionar\u00e3o.",
                                                                            "No se encontr\u00f3 m\u00e9todo de elevaci\u00f3n (pkexec/sudo). Las correcciones no funcionar\u00e1n.",
                                                                                                                    "\u041d\u0435 \u043d\u0430\u0439\u0434\u0435\u043d \u043c\u0435\u0442\u043e\u0434 \u043f\u043e\u0432\u044b\u0448\u0435\u043d\u0438\u044f \u043f\u0440\u0438\u0432\u0438\u043b\u0435\u0433\u0438\u0439 (pkexec/sudo). \u0418\u0441\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u044f \u043d\u0435 \u0431\u0443\u0434\u0443\u0442 \u0440\u0430\u0431\u043e\u0442\u0430\u0442\u044c.",
                                                                                                                                                        "\u672a\u627e\u5230\u63d0\u6743\u65b9\u6cd5(pkexec/sudo)\u3002\u4fee\u590d\u5c06\u65e0\u6cd5\u5de5\u4f5c\u3002"},
    {"Please restart the application to apply the language change.",
                                             "Reinicie o aplicativo para aplicar a altera\u00e7\u00e3o de idioma.",
                                                                            "Reinicie la aplicaci\u00f3n para aplicar el cambio de idioma.",
                                                                                                                    "\u041f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u0435 \u043f\u0440\u0438\u043b\u043e\u0436\u0435\u043d\u0438\u0435, \u0447\u0442\u043e\u0431\u044b \u043f\u0440\u0438\u043c\u0435\u043d\u0438\u0442\u044c \u0438\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u0435 \u044f\u0437\u044b\u043a\u0430.",
                                                                                                                                                        "\u8bf7\u91cd\u65b0\u542f\u52a8\u5e94\u7528\u7a0b\u5e8f\u4ee5\u5e94\u7528\u8bed\u8a00\u66f4\u6539\u3002"},
    {"Restart",                              "Reiniciar",                        "Reiniciar",                        "\u041f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u044c",                    "\u91cd\u542f"},
    {"System Language",                      "Idioma do sistema",                "Idioma del sistema",               "\u0421\u0438\u0441\u0442\u0435\u043c\u043d\u044b\u0439 \u044f\u0437\u044b\u043a",                   "\u7cfb\u7edf\u8bed\u8a00"},
    {"Legal Notice",                         "Aviso legal",                      "Aviso legal",                      "\u042e\u0440\u0438\u0434\u0438\u0447\u0435\u0441\u043a\u0430\u044f \u0438\u043d\u0444\u043e\u0440\u043c\u0430\u0446\u0438\u044f",           "\u6cd5\u5f8b\u58f0\u660e"},
    {"Developed by Renan Mayrinck.",         "Desenvolvido por Renan Mayrinck.", "Desarrollado por Renan Mayrinck.", "\u0420\u0430\u0437\u0440\u0430\u0431\u043e\u0442\u0430\u043d\u043e Renan Mayrinck.",      "\u7531Renan Mayrinck\u5f00\u53d1\u3002"},
    {"This software was developed with the assistance of artificial intelligence.",
                                             "Este software foi desenvolvido com aux\u00edlio de intelig\u00eancia artificial.",
                                                                            "Este software fue desarrollado con ayuda de inteligencia artificial.",
                                                                                                                    "\u042d\u0442\u043e \u043f\u0440\u043e\u0433\u0440\u0430\u043c\u043c\u043d\u043e\u0435 \u043e\u0431\u0435\u0441\u043f\u0435\u0447\u0435\u043d\u0438\u0435 \u0431\u044b\u043b\u043e \u0440\u0430\u0437\u0440\u0430\u0431\u043e\u0442\u0430\u043d\u043e \u0441 \u043f\u043e\u043c\u043e\u0449\u044c\u044e \u0438\u0441\u043a\u0443\u0441\u0441\u0442\u0432\u0435\u043d\u043d\u043e\u0433\u043e \u0438\u043d\u0442\u0435\u043b\u043b\u0435\u043a\u0442\u0430.",
                                                                                                                                                        "\u672c\u8f6f\u4ef6\u662f\u5728\u4eba\u5de5\u667a\u80fd\u7684\u8f85\u52a9\u4e0b\u5f00\u53d1\u7684\u3002"},
    {"Theme",                                "Tema",                               "Tema",                                "\u0422\u0435\u043c\u0430",                                "\u4e3b\u9898"},
    {"Follow System Theme",                  "Usar o tema do sistema",             "Usar el tema del sistema",            "\u0418\u0441\u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u044c \u0442\u0435\u043c\u0443 \u0441\u0438\u0441\u0442\u0435\u043c\u044b",           "\u8ddf\u968f\u7cfb\u7edf\u4e3b\u9898"},
    {"Light Theme",                          "Tema claro",                         "Tema claro",                          "\u0421\u0432\u0435\u0442\u043b\u0430\u044f \u0442\u0435\u043c\u0430",                        "\u6d45\u8272\u4e3b\u9898"},
    {"Dark Theme",                           "Tema escuro",                        "Tema oscuro",                         "\u0422\u0451\u043c\u043d\u0430\u044f \u0442\u0435\u043c\u0430",                         "\u6df1\u8272\u4e3b\u9898"},

    {"Keyboard Shortcuts",                   "Atalhos do Teclado",                 "Atajos del Teclado",                  "\u0413\u043e\u0440\u044f\u0447\u0438\u0435 \u043a\u043b\u0430\u0432\u0438\u0448\u0438",              "\u5feb\u6377\u952e"},
    {"Technical data copied",                "Dados t\u00e9cnicos copiados",         "Datos t\u00e9cnicos copiados",          "\u0422\u0435\u0445\u043d\u0438\u0447\u0435\u0441\u043a\u0438\u0435 \u0434\u0430\u043d\u043d\u044b\u0435 \u0441\u043a\u043e\u043f\u0438\u0440\u043e\u0432\u0430\u043d\u044b", "\u6280\u672f\u6570\u636e\u5df2\u590d\u5236"},
    {"Preferences",                          "Prefer\u00eancias",                      "Preferencias",                        "\u041d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438",                    "\u8bbe\u7f6e"},
    {"Yes",                                  "Sim",                                 "S\u00ed",                                 "\u0414\u0430",                                  "\u662f"},
    {"Yes, for PlayStation\u00ae3",           "Sim, para PlayStation\u00ae3",           "S\u00ed, para PlayStation\u00ae3",           "\u0414\u0430, \u0434\u043b\u044f PlayStation\u00ae3",           "\u662f\uff0c\u9488\u5bf9PlayStation\u00ae3"},
    {"Licensed Playstation Accessory",       "Acess\u00f3rio Playstation Licenciado",   "Accesorio Playstation Licenciado",    "\u041b\u0438\u0446\u0435\u043d\u0437\u0438\u0440\u043e\u0432\u0430\u043d\u043d\u044b\u0439 \u0430\u043a\u0441\u0435\u0441\u0441\u0443\u0430\u0440 Playstation", "\u6388\u6743PlayStation\u914d\u4ef6"},
    {"The possible manufacturer name of the board on the device, not the retail brand.",
                                             "O nome do fabricante do chip Bluetooth no dispositivo, n\u00e3o a marca do produto final.",
                                                                                    "El nombre del fabricante del chip Bluetooth en el dispositivo, no la marca del producto final.",
                                                                                                                                        "\u041d\u0430\u0438\u043c\u0435\u043d\u043e\u0432\u0430\u043d\u0438\u0435 \u043f\u0440\u043e\u0438\u0437\u0432\u043e\u0434\u0438\u0442\u0435\u043b\u044f \u043c\u0438\u043a\u0440\u043e\u0441\u0445\u0435\u043c\u044b Bluetooth \u0432 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0435, \u0430 \u043d\u0435 \u0442\u043e\u0440\u0433\u043e\u0432\u0430\u044f \u043c\u0430\u0440\u043a\u0430.",  "\u8bbe\u5907\u4e0a Bluetooth \u82af\u7247\u7684\u53ef\u80fd\u5236\u9020\u5546\u540d\u79f0\uff0c\u800c\u975e\u96f6\u552e\u54c1\u724c\u3002"},
    {"This device may use Bluetooth technologies licensed by Sony Interactive Entertainment.",
                                             "Este dispositivo usa tecnologias Bluetooth licenciadas pela Sony Interactive Entertainment.",
                                                                                    "Este dispositivo puede usar tecnolog\u00edas Bluetooth licenciadas por Sony Interactive Entertainment.",
                                                                                                                                        "\u042d\u0442\u043e \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u043e \u043c\u043e\u0436\u0435\u0442 \u0438\u0441\u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u044c \u0442\u0435\u0445\u043d\u043e\u043b\u043e\u0433\u0438\u0438 Bluetooth, \u043b\u0438\u0446\u0435\u043d\u0437\u0438\u0440\u043e\u0432\u0430\u043d\u043d\u044b\u0435 Sony Interactive Entertainment.", "\u6b64\u8bbe\u5907\u53ef\u80fd\u4f7f\u7528\u7531Sony Interactive Entertainment\u6388\u6743\u7684\u84dd\u7259\u6280\u672f\u3002"},
    {"Enter password for Bluetooth fixes:",  "Digite a senha para corre\u00e7\u00f5es Bluetooth:",
                                                                                    "Ingrese la contrase\u00f1a para correcciones Bluetooth:",
                                                                                                                                        "\u0412\u0432\u0435\u0434\u0438\u0442\u0435 \u043f\u0430\u0440\u043e\u043b\u044c \u0434\u043b\u044f \u0438\u0441\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0439 Bluetooth:", "\u8bf7\u8f93\u5165\u84dd\u7259\u4fee\u590d\u5bc6\u7801\uff1a"},
    {"Recommended for CSR and Barrot/Generic dongles. Stabilizes the radio to fix detection and connection loops.",
                                             "Recomendado para dongles CSR e Barrot/Generic. Estabiliza o r\u00e1dio Bluetooth para corrigir loops de detec\u00e7\u00e3o e conex\u00e3o.",
                                                                                    "Recomendado para dongles CSR y Barrot/Generic. Estabiliza la radio Bluetooth para corregir bucles de detecci\u00f3n y conexi\u00f3n.",
                                                                                                                                        "\u0420\u0435\u043a\u043e\u043c\u0435\u043d\u0434\u0443\u0435\u0442\u0441\u044f \u0434\u043b\u044f \u0434\u043e\u043d\u0433\u043b\u043e\u0432 CSR \u0438 Barrot/Generic. \u0421\u0442\u0430\u0431\u0438\u043b\u0438\u0437\u0438\u0440\u0443\u0435\u0442 \u0440\u0430\u0431\u043e\u0442\u0443 \u0440\u0430\u0434\u0438\u043e\u043c\u043e\u0434\u0443\u043b\u044f \u0434\u043b\u044f \u0443\u0441\u0442\u0440\u0430\u043d\u0435\u043d\u0438\u044f \u043f\u0440\u043e\u0431\u043b\u0435\u043c \u043e\u0431\u043d\u0430\u0440\u0443\u0436\u0435\u043d\u0438\u044f \u0438 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f.", "\u5efa\u8bae\u7528\u4e8eCSR\u548cBarrot/Generic\u914d\u4ef6\u3002\u7a33\u5b9a\u65e0\u7ebf\u7535\u4ee5\u4fee\u590d\u68c0\u6d4b\u548c\u8fde\u63a5\u95ee\u9898\u3002"},
    {"Fixes automatic disconnections of modern Bluetooth gamepads.",
                                             "Corrige desconex\u00f5es autom\u00e1ticas de gamepads Bluetooth modernos.",
                                                                                    "Corrige desconexiones autom\u00e1ticas de gamepads Bluetooth modernos.",
                                                                                                                                        "\u0418\u0441\u043f\u0440\u0430\u0432\u043b\u044f\u0435\u0442 \u0430\u0432\u0442\u043e\u043c\u0430\u0442\u0438\u0447\u0435\u0441\u043a\u0438\u0435 \u043e\u0442\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f \u0441\u043e\u0432\u0440\u0435\u043c\u0435\u043d\u043d\u044b\u0445 \u0438\u0433\u0440\u043e\u0432\u044b\u0445 \u043a\u043e\u043d\u0442\u0440\u043e\u043b\u043b\u0435\u0440\u043e\u0432 Bluetooth.", "\u4fee\u590d\u73b0\u4ee3\u84dd\u7259\u6e38\u620f\u624b\u67c4\u7684\u81ea\u52a8\u65ad\u5f00\u8fde\u63a5\u95ee\u9898\u3002"},
    {"Allows old Bluetooth devices to pair with a fixed PIN.",
                                             "Permite que dispositivos Bluetooth antigos pareiem com um PIN fixo.",
                                                                                    "Permite que dispositivos Bluetooth antiguos se emparejen con un PIN fijo.",
                                                                                                                                        "\u041f\u043e\u0437\u0432\u043e\u043b\u044f\u0435\u0442 \u0441\u0442\u0430\u0440\u044b\u043c Bluetooth-\u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0430\u043c \u0441\u043e\u043f\u0440\u044f\u0433\u0430\u0442\u044c\u0441\u044f \u0441 \u0444\u0438\u043a\u0441\u0438\u0440\u043e\u0432\u0430\u043d\u043d\u044b\u043c PIN-\u043a\u043e\u0434\u043e\u043c.", "\u5141\u8bb8\u65e7\u84dd\u7259\u8bbe\u5907\u4f7f\u7528\u56fa\u5b9aPIN\u8fdb\u884c\u914d\u5bf9\u3002"},
    {"Downloads and injects the missing official driver binaries.",
                                             "Baixa e injeta os drivers oficiais ausentes.",
                                                                                    "Descarga e inyecta los controladores oficiales faltantes.",
                                                                                                                                        "\u0421\u043a\u0430\u0447\u0438\u0432\u0430\u0435\u0442 \u0438 \u0443\u0441\u0442\u0430\u043d\u0430\u0432\u043b\u0438\u0432\u0430\u0435\u0442 \u043e\u0442\u0441\u0443\u0442\u0441\u0442\u0432\u0443\u044e\u0449\u0438\u0435 \u043e\u0444\u0438\u0446\u0438\u0430\u043b\u044c\u043d\u044b\u0435 \u0434\u0440\u0430\u0439\u0432\u0435\u0440\u044b.", "\u4e0b\u8f7d\u5e76\u6ce8\u5165\u7f3a\u5931\u7684\u5b98\u65b9\u9a71\u52a8\u7a0b\u5e8f\u3002"},
    {"Downloads and extracts the missing proprietary firmware for Broadcom chipsets.",
                                             "Baixa e extrai o firmware propriet\u00e1rio ausente para chipsets Broadcom.",
                                                                                    "Descarga y extrae el firmware propietario faltante para chipsets Broadcom.",
                                                                                                                                        "\u0421\u043a\u0430\u0447\u0438\u0432\u0430\u0435\u0442 \u0438 \u0438\u0437\u0432\u043b\u0435\u043a\u0430\u0435\u0442 \u043e\u0442\u0441\u0443\u0442\u0441\u0442\u0432\u0443\u044e\u0449\u0443\u044e \u043f\u0440\u043e\u043f\u0440\u0438\u0435\u0442\u0430\u0440\u043d\u0443\u044e \u043f\u0440\u043e\u0448\u0438\u0432\u043a\u0443 \u0434\u043b\u044f \u0447\u0438\u043f\u0441\u0435\u0442\u043e\u0432 Broadcom.", "\u4e0b\u8f7d\u5e76\u63d0\u53d6\u7f3a\u5931\u7684Broadcom\u82af\u7247\u7ec4\u4e13\u6709\u56fa\u4ef6\u3002"},
    {"Forces activation of adapters stuck in Airplane Mode.",
                                             "For\u00e7a a ativa\u00e7\u00e3o de adaptadores presos no modo avi\u00e3o.",
                                                                                    "Fuerza la activaci\u00f3n de adaptadores bloqueados en modo avi\u00f3n.",
                                                                                                                                        "\u041f\u0440\u0438\u043d\u0443\u0434\u0438\u0442\u0435\u043b\u044c\u043d\u043e \u0432\u043a\u043b\u044e\u0447\u0430\u0435\u0442 \u0430\u0434\u0430\u043f\u0442\u0435\u0440\u044b, \u0437\u0430\u0441\u0442\u0440\u044f\u0432\u0448\u0438\u0435 \u0432 \u0440\u0435\u0436\u0438\u043c\u0435 \u043f\u043e\u043b\u0451\u0442\u0430.", "\u5f3a\u5236\u6fc0\u6d3b\u5361\u5728\u98de\u884c\u6a21\u5f0f\u4e2d\u7684\u9002\u914d\u5668\u3002"},
    {"Adds the user to the lp group for Bluetooth D-Bus access.",
                                             "Adiciona o usu\u00e1rio ao grupo lp para acesso Bluetooth via D-Bus.",
                                                                                    "Agrega el usuario al grupo lp para acceso Bluetooth v\u00eda D-Bus.",
                                                                                                                                        "\u0414\u043e\u0431\u0430\u0432\u043b\u044f\u0435\u0442 \u043f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u0435\u043b\u044f \u0432 \u0433\u0440\u0443\u043f\u043f\u0443 lp \u0434\u043b\u044f \u0434\u043e\u0441\u0442\u0443\u043f\u0430 \u043a Bluetooth \u0447\u0435\u0440\u0435\u0437 D-Bus.", "\u5c06\u7528\u6237\u6dfb\u52a0\u5230lp\u7ec4\u4ee5\u83b7\u53d6\u901a\u8fc7D-Bus\u8bbf\u95ee\u84dd\u7259\u7684\u6743\u9650\u3002"},
    {"Removes all pairing caches to fix connection errors.",
                                             "Remove todos os caches de pareamento para corrigir erros de conex\u00e3o.",
                                                                                    "Elimina todos los cach\u00e9s de emparejamiento para corregir errores de conexi\u00f3n.",
                                                                                                                                        "\u0423\u0434\u0430\u043b\u044f\u0435\u0442 \u0432\u0441\u0435 \u043a\u044d\u0448\u0438 \u0441\u043e\u043f\u0440\u044f\u0436\u0435\u043d\u0438\u044f \u0434\u043b\u044f \u0438\u0441\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u044f \u043e\u0448\u0438\u0431\u043e\u043a \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f.", "\u5220\u9664\u6240\u6709\u914d\u5bf9\u7f13\u5b58\u4ee5\u4fee\u590d\u8fde\u63a5\u9519\u8bef\u3002"},
    {"Clears caches and buffers by restarting the system service.",
                                             "Limpa caches e buffers reiniciando o servi\u00e7o do sistema.",
                                                                                    "Limpia cach\u00e9s y b\u00faferes reiniciando el servicio del sistema.",
                                                                                                                                        "\u041e\u0447\u0438\u0449\u0430\u0435\u0442 \u043a\u044d\u0448\u0438 \u0438 \u0431\u0443\u0444\u0435\u0440\u044b \u043f\u0443\u0442\u0451\u043c \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a\u0430 \u0441\u043b\u0443\u0436\u0431\u044b \u0441\u0438\u0441\u0442\u0435\u043c\u044b.", "\u901a\u8fc7\u91cd\u542f\u7cfb\u7edf\u670d\u52a1\u6765\u6e05\u9664\u7f13\u5b58\u548c\u7f13\u51b2\u533a\u3002"},
    {"No technical data to copy",            "N\u00e3o h\u00e1 dados t\u00e9cnicos para copiar",
                                                                                   "No hay datos t\u00e9cnicos para copiar",
                                                                                                                           "\u041d\u0435\u0442 \u0442\u0435\u0445\u043d\u0438\u0447\u0435\u0441\u043a\u0438\u0445 \u0434\u0430\u043d\u043d\u044b\u0445 \u0434\u043b\u044f \u043a\u043e\u043f\u0438\u0440\u043e\u0432\u0430\u043d\u0438\u044f", "\u6ca1\u6709\u53ef\u590d\u5236\u7684\u6280\u672f\u6570\u636e"},
    {"/var/lib/bluetooth not found",          "/var/lib/bluetooth n\u00e3o encontrado",
                                                                                    "/var/lib/bluetooth no encontrado",
                                                                                                                            "/var/lib/bluetooth \u043d\u0435 \u043d\u0430\u0439\u0434\u0435\u043d\u0430", "/var/lib/bluetooth \u672a\u627e\u5230"},
    {"Open keyboard shortcuts",               "Abrir atalhos do teclado",
                                                                                   "Abrir atajos del teclado",
                                                                                                                           "\u041e\u0442\u043a\u0440\u044b\u0442\u044c \u0433\u043e\u0440\u044f\u0447\u0438\u0435 \u043a\u043b\u0430\u0432\u0438\u0448\u0438", "\u6253\u5f00\u5feb\u6377\u952e"},
    {"Scan for Bluetooth devices",            "Escanear dispositivos Bluetooth",
                                                                                   "Escanear dispositivos Bluetooth",
                                                                                                                           "\u0421\u043a\u0430\u043d\u0438\u0440\u043e\u0432\u0430\u0442\u044c \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432\u0430 Bluetooth", "\u626b\u63cf\u84dd\u7259\u8bbe\u5907"},
    {"Toggle light/dark theme",               "Alternar tema claro/escuro",
                                                                                   "Alternar tema claro/oscuro",
                                                                                                                           "\u041f\u0435\u0440\u0435\u043a\u043b\u044e\u0447\u0438\u0442\u044c \u0441\u0432\u0435\u0442\u043b\u0443\u044e/\u0442\u0451\u043c\u043d\u0443\u044e \u0442\u0435\u043c\u0443", "\u5207\u6362\u6d45\u8272/\u6df1\u8272\u4e3b\u9898"},
    {"Open preferences",                      "Abrir prefer\u00eancias",
                                                                                   "Abrir preferencias",
                                                                                                                           "\u041e\u0442\u043a\u0440\u044b\u0442\u044c \u043d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438", "\u6253\u5f00\u8bbe\u7f6e"},
    {"Copy tech sheet data",                  "Copiar dados t\u00e9cnicos",
                                                                                   "Copiar datos t\u00e9cnicos",
                                                                                                                           "\u041a\u043e\u043f\u0438\u0440\u043e\u0432\u0430\u0442\u044c \u0442\u0435\u0445\u043d\u0438\u0447\u0435\u0441\u043a\u0438\u0435 \u0434\u0430\u043d\u043d\u044b\u0435", "\u590d\u5236\u6280\u672f\u6570\u636e"},
};

LangId current_lang = LANG_SYS;

const char* _(const char *en) {
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

const char* info5(const char *en, const char *pt, const char *es, const char *ru, const char *zh) {
    switch (current_lang) {
        case LANG_PT: return pt;
        case LANG_ES: return es;
        case LANG_RU: return ru;
        case LANG_ZH: return zh;
        default: return en;
    }
}

LangId lang_from_str(const char *s) {
    if (!s || s[0] == '\0') return LANG_SYS;
    if (g_strcmp0(s, LANG_CODE_PT) == 0) return LANG_PT;
    if (g_strcmp0(s, LANG_CODE_ES) == 0) return LANG_ES;
    if (g_strcmp0(s, LANG_CODE_RU) == 0) return LANG_RU;
    if (g_strcmp0(s, LANG_CODE_ZH) == 0) return LANG_ZH;
    if (g_strcmp0(s, LANG_CODE_EN) == 0) return LANG_EN;
    return LANG_SYS;
}

void detect_language(void) {
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

const char* lang_code(LangId id) {
    switch (id) {
        case LANG_EN: return LANG_CODE_EN;
        case LANG_PT: return LANG_CODE_PT;
        case LANG_ES: return LANG_CODE_ES;
        case LANG_RU: return LANG_CODE_RU;
        case LANG_ZH: return LANG_CODE_ZH;
        default: return "";
    }
}

const char* lang_label(LangId id) {
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

void set_language(LangId lang) {
    if (current_lang == lang) return;
    current_lang = lang;
    g_autofree char *dir = g_build_filename(g_get_user_config_dir(), "blufixer", NULL);
    g_mkdir_with_parents(dir, 0755);
    g_autofree char *cfg = g_build_filename(dir, "language", NULL);
    g_file_set_contents(cfg, lang_code(lang), -1, NULL);
    AdwToast *toast = adw_toast_new(_("Please restart the application to apply the language change."));
    adw_toast_set_button_label(toast, _("Restart"));
    adw_toast_set_action_name(toast, "win.restart");
    adw_toast_set_priority(toast, ADW_TOAST_PRIORITY_HIGH);
    adw_toast_overlay_add_toast(ADW_TOAST_OVERLAY(app_data.toast_overlay), toast);
}
