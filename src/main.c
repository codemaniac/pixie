#include "include/bitscan.h"
#include "include/chess.h"
#include "include/fen.h"
#include "include/perft.h"
#include "include/utils.h"
#include "lib/logc/log.h"
#include <assert.h>
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

    const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Position*   pos = fen_to_position(fen);
    board_display(pos->board);
    printf("%d\n", pos->active_color);
    printf("%d\n", pos->casteling_rights);
    printf("%d\n", pos->enpassant_target);
    printf("%d\n", pos->half_move_clock);
    printf("%d\n", pos->full_move_number);
    printf("%llu\n\n", pos->hash);

    divide(pos, 6);

    board_display(pos->board);

    return EXIT_SUCCESS;
}
