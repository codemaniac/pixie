#include "include/uci.h"
#include "lib/logc/log.h"
#include <stdio.h>
#include <stdlib.h>

#define PROGRAM_NAME "pixie"
#define VERSION "0.1.0-dev"

#ifndef LOG_QUIET
    #define LOG_QUIET_VAL 0
#else
    #define LOG_QUIET_VAL 1
#endif

int main(void) {

#ifdef DEBUG
    log_set_quiet(0);
    log_set_level(LOG_TRACE);
#else
    log_set_quiet(LOG_QUIET_VAL);
    log_set_level(LOG_INFO);
#endif

    printf("\n");
    printf("   ___ _      _");
    printf("\n");
    printf("  / _ (_)_  _(_) ___");
    printf("\n");
    printf(" / /_)/ \\ \\/ / |/ _ \\");
    printf("\n");
    printf("/ ___/| |>  <| |  __/");
    printf("\n");
    printf("\\/    |_/_/\\_\\_|\\___|");
    printf("\n\n");

    log_info("Starting Pixie %s", VERSION);

#ifdef DEBUG
    log_warn("%s is running in DEBUG mode!", PROGRAM_NAME);
#endif

    uci_loop();

    return EXIT_SUCCESS;
}
