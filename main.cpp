#define EINA_LOG_DOMAIN_DEFAULT _log_dom
static int _log_dom = -1; // must come before inclusion of Eina.h!
#include <Eina.h>
#include <Elementary.h>
#include <Ecore_Getopt.h>
#include <expat_config.h>
#include "data/sqlite_file.h"
#include "data_ui.h"

static const Ecore_Getopt optdesc = {
        "gka-data-e",
        "%prog [options] [file]",
        PACKAGE_VERSION,
        "Copyright © 2017 Adam Boardman",
        "GPLv3",
        "A Simple Database",
        EINA_TRUE,
        {
                ECORE_GETOPT_LICENSE('L', "license"),
                ECORE_GETOPT_COPYRIGHT('C', "copyright"),
                ECORE_GETOPT_VERSION('V', "version"),
                ECORE_GETOPT_HELP('h', "help"),
                {0, NULL, NULL, NULL, ECORE_GETOPT_ACTION_STORE, {.dummy = NULL}}//ECORE_GETOPT_SENTINEL
        }
};

EAPI_MAIN int
elm_main(int argc, char **argv)
{
    Eina_Bool quit_option = EINA_FALSE;
    const char *project_path = NULL;

    Ecore_Getopt_Value values[] = {
            ECORE_GETOPT_VALUE_BOOL(quit_option),
            ECORE_GETOPT_VALUE_BOOL(quit_option),
            ECORE_GETOPT_VALUE_BOOL(quit_option),
            ECORE_GETOPT_VALUE_BOOL(quit_option),
            ECORE_GETOPT_VALUE_NONE
    };

    eina_init();
    _log_dom = eina_log_domain_register("gka-data", EINA_COLOR_GREEN);

    elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

    int args = ecore_getopt_parse(&optdesc, values, argc, argv);
    if (args < 0)
    {
        EINA_LOG_CRIT("Could not parse arguments.");
        return -1;
    }
    else if (quit_option)
    {
        return 0;
    }
    std::string filename;
    if (args < argc)
    {
        filename = argv[args];
    }

    ui.init();
    if (filename.empty()) {
        ui.setNewFile();
    } else {
        ui.setFile(filename);
    }

    elm_run();

    elm_shutdown();

    return 0;
}
ELM_MAIN()

