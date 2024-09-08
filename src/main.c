#include "include/chess.h"
#include "include/eval.h"
#include "include/fen.h"
#include "include/perft.h"
#include "include/search.h"
#include "include/utils.h"
#include "lib/logc/log.h"
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

    const char* fen = "4k3/4p3/8/8/8/8/4P3/4K3 w - - 0 1";
    Position*   pos = fen_to_position(fen);
    board_display(pos->board);
    printf("%d\n", pos->active_color);
    printf("%d\n", pos->casteling_rights);
    printf("%d\n", pos->enpassant_target);
    printf("%d\n", pos->half_move_clock);
    printf("%d\n", pos->full_move_number);
    printf("%d\n", pos->ply_count);
    printf("%llu\n\n", pos->hash);

    uint64_t start = utils_curr_time_ms();
    divide(pos, 6);
    printf("Completed in %llums\n", utils_curr_time_ms() - start);

    uint32_t score = search(pos, 2);
    printf("Score = %d\n", score);

    return EXIT_SUCCESS;
}
