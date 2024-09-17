#include "include/chess.h"
#include "include/fen.h"
#include "include/perft.h"
#include "include/search.h"
#include "include/utils.h"
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

    initialize();

    const char* fen = "k7/6R1/7R/8/8/r7/r7/4K3 w - - 0 1";
    Position*   pos = fen_to_position(fen);
    position_display(pos);

    Move best_move;
    char best_move_str[10];

    int32_t score = search(pos, 5, &best_move);
    move_to_str(best_move_str, best_move);

    printf("Score = %d\nBest Move = %s\n", score, best_move_str);

    return EXIT_SUCCESS;
}
