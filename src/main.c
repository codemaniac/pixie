#include "include/game.h"
#include "include/perft.h"
#include "lib/argtable3/argtable3.h"
#include "lib/logc/log.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PROGRAM_NAME "pixie"
#define VERSION "0.1.0-dev"

#ifndef LOG_QUIET
    #define LOG_QUIET_VAL 0
#else
    #define LOG_QUIET_VAL 1
#endif

int main(int argc, char* argv[]) {

    struct arg_lit *verbose, *help, *version;
    struct arg_end* end;
    int             exitcode = EXIT_FAILURE;
    int             nerrors;

    void* argtable[] = {
      help    = arg_litn(NULL, "help", 0, 1, "display help"),
      version = arg_litn(NULL, "version", 0, 1, "display version info"),
      verbose = arg_litn("v", "verbose", 0, 1, "verbose output"),
      end     = arg_end(20),
    };

    nerrors = arg_parse(argc, argv, argtable);

    if (help->count > 0)
    {
        printf("Usage: %s", PROGRAM_NAME);
        arg_print_syntax(stdout, argtable, "\n");
        printf("\n\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        exitcode = EXIT_SUCCESS;
        goto exit;
    }

    if (version->count > 0)
    {
        printf("%s version: %s\n", PROGRAM_NAME, VERSION);
        exitcode = EXIT_SUCCESS;
        goto exit;
    }

    if (nerrors > 0)
    {
        arg_print_errors(stdout, end, PROGRAM_NAME);
        printf("Try '%s --help' for more information.\n", PROGRAM_NAME);
        exitcode = EXIT_FAILURE;
        goto exit;
    }

#ifdef DEBUG
    log_set_quiet(0);
    log_set_level(LOG_TRACE);
#else
    log_set_level(LOG_INFO);
    if (verbose->count > 0)
    {
        log_set_quiet(0);
    }
    else
    {
        log_set_quiet(LOG_QUIET_VAL);
    }
#endif

    log_info("Pixie %s", VERSION);

#ifdef DEBUG
    log_warn("%s is running in DEBUG mode!", PROGRAM_NAME);
#endif

    const char* fen =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    Game* game = initialize_game(fen);

    log_info("Starting Perft");
    uint64_t nodes = perft(game, 2);
    log_info("Perft nodes = %llu", nodes);

    return EXIT_SUCCESS;

exit:
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return exitcode;
}
