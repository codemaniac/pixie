#include "include/eval.h"
#include "include/constants.h"
#include "include/position.h"

// clang-format off
static const int32_t piece_scores[] = {
    S(293, 92),
    S(310, 418),
    S(342, 446),
    S(625, 553),
    S(1142, 1152),
    S(0, 0)
};

static const int32_t psqt_scores[6][64] = {
    {
        S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
        S(137, 122), S(132, 151), S(83, 60), S(49, 70), S(64, 69), S(48, 92), S(96, 21), S(132, 37),
        S(-28, -57), S(-31, -47), S(-61, 24), S(-88, 31), S(-116, 86), S(-122, 80), S(-62, 5), S(-57, -40),
        S(-125, -34), S(-140, 8), S(-158, 0), S(-171, 13), S(-177, 19), S(-172, 5), S(-155, 12), S(-147, -38),
        S(-148, -48), S(-157, -13), S(-174, -15), S(-183, 7), S(-182, 16), S(-182, -2), S(-173, 5), S(-168, -42),
        S(-160, -52), S(-163, -10), S(-180, -14), S(-170, -17), S(-171, -3), S(-177, -3), S(-180, 33), S(-180, -26),
        S(-145, -60), S(-162, -9), S(-162, -30), S(-158, -34), S(-155, -25), S(-176, 18), S(-175, 37), S(-176, -38),
        S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)
    }, {
        S(-54, -218), S(-9, -144), S(20, -60), S(-6, -66), S(-29, 128), S(10, -153), S(-42, 7), S(-99, -108),
        S(2, -90), S(24, -48), S(2, 67), S(27, 36), S(22, 28), S(-5, 73), S(-3, 12), S(-30, -15),
        S(5, -47), S(-8, 82), S(32, 46), S(39, 69), S(18, 106), S(-1, 194), S(2, 82), S(-20, 42),
        S(2, -4), S(27, 21), S(51, 27), S(57, 61), S(49, 45), S(44, 70), S(34, 27), S(0, 30),
        S(8, -9), S(21, 9), S(42, 20), S(61, 19), S(52, 29), S(51, 16), S(34, 17), S(10, -12),
        S(-1, -20), S(18, -5), S(23, 16), S(45, 15), S(47, 22), S(27, 20), S(9, 31), S(8, -12),
        S(-19, -28), S(7, -57), S(20, -12), S(16, 3), S(22, -3), S(1, 10), S(1, -12), S(-16, -24),
        S(19, -135), S(-23, -19), S(7, -63), S(11, -29), S(0, -7), S(17, -29), S(-23, -16), S(-41, -36)
    }, {
        S(12, -52), S(-9, 3), S(24, -147), S(9, -65), S(20, -47), S(3, -19), S(9, -5), S(3, -34),
        S(8, -32), S(4, 33), S(24, -19), S(8, -31), S(9, 56), S(5, 67), S(7, 20), S(9, -49),
        S(10, -5), S(-2, 49), S(5, 62), S(7, 57), S(6, 43), S(12, 70), S(15, 45), S(24, 5),
        S(12, -6), S(21, 13), S(30, 25), S(20, 69), S(27, 53), S(19, 61), S(13, 16), S(12, 3),
        S(8, -7), S(18, 24), S(23, 19), S(36, 33), S(25, 38), S(23, 19), S(15, 21), S(11, 3),
        S(1, 8), S(10, 25), S(26, 11), S(28, 24), S(32, 20), S(16, 34), S(7, 23), S(2, 16),
        S(10, 11), S(-12, 26), S(1, 26), S(16, 9), S(18, 19), S(2, 0), S(-5, 45), S(-19, 6),
        S(-12, -34), S(7, 3), S(-16, -6), S(16, -26), S(11, -16), S(-16, -9), S(17, -48), S(0, -23)
    }, {
        S(16, 91), S(17, 94), S(38, 58), S(7, 133), S(10, 127), S(29, 37), S(21, 36), S(13, 73),
        S(21, 56), S(23, 46), S(5, 123), S(5, 125), S(-12, 148), S(-11, 142), S(21, 47), S(9, 81),
        S(21, 5), S(20, 38), S(16, 54), S(16, 51), S(13, 34), S(-7, 86), S(-17, 125), S(2, 34),
        S(16, -8), S(20, -1), S(22, 18), S(11, 57), S(16, 38), S(9, 61), S(8, 12), S(17, -15),
        S(24, -38), S(25, -18), S(24, 2), S(21, 7), S(14, 19), S(11, 0), S(4, 29), S(3, -14),
        S(21, -45), S(22, -11), S(13, -4), S(14, 0), S(12, 12), S(-1, 12), S(7, 12), S(6, -21),
        S(17, -40), S(5, -1), S(23, -13), S(21, -4), S(6, 10), S(6, 7), S(-1, 4), S(14, -57),
        S(6, -6), S(23, 0), S(24, 8), S(22, 19), S(13, 26), S(13, 8), S(13, -20), S(-8, -13)
    }, {
        S(63, -15), S(115, 8), S(106, 45), S(101, 38), S(36, 170), S(26, 178), S(61, 83), S(92, 75),
        S(47, 3), S(90, -15), S(108, 19), S(119, 18), S(169, -25), S(53, 121), S(109, 50), S(65, 87),
        S(44, 17), S(80, 13), S(74, 36), S(132, 26), S(130, 47), S(68, 115), S(95, 78), S(68, 90),
        S(75, -3), S(104, -3), S(101, 5), S(132, -2), S(135, 20), S(115, 36), S(153, 14), S(126, 17),
        S(43, 18), S(111, -13), S(106, 7), S(126, 15), S(112, 20), S(126, 16), S(133, 23), S(115, 19),
        S(63, -1), S(31, 32), S(98, 13), S(83, 23), S(97, 20), S(105, 25), S(79, 39), S(75, 26),
        S(54, -17), S(48, 10), S(39, 37), S(53, 21), S(64, 34), S(35, 29), S(32, 19), S(53, 19),
        S(40, 22), S(50, 7), S(42, 15), S(15, 39), S(59, 8), S(86, -49), S(48, -18), S(32, -34)
    },{
        S(-92, -87), S(-86, 227), S(-61, 212), S(-54, 113), S(24, -203), S(40, -121), S(-21, 62), S(-33, 97),
        S(-65, 232), S(-2, 66), S(-1, 81), S(-12, 154), S(5, 66), S(27, 37), S(38, -54), S(51, -200),
        S(-18, 103), S(10, 99), S(2, 120), S(14, 25), S(13, 41), S(23, 135), S(19, 142), S(13, 3),
        S(-17, -13), S(32, -17), S(33, 26), S(42, -63), S(38, -62), S(52, -60), S(35, -6), S(19, -98),
        S(-15, -83), S(-2, 23), S(34, -80), S(58, -157), S(62, -159), S(49, -100), S(21, -66), S(2, -96),
        S(-28, 33), S(-7, -7), S(20, -30), S(38, -69), S(43, -81), S(32, -68), S(11, -6), S(-6, -26),
        S(-34, 14), S(-17, 18), S(10, -19), S(19, -73), S(27, -55), S(10, -34), S(-4, 25), S(-29, 32),
        S(-76, 8), S(-44, 52), S(-27, 22), S(-6, -60), S(-36, 14), S(-12, -29), S(-35, 47), S(-62, 48)
    }
};
// clang-format on

static int32_t PSQT[6][64][2];

void eval_init() {

    for (int p = 0; p < 6; p++)
    {
        for (int sq = 0; sq < 64; sq++)
        {
            PSQT[p][sq][MG] = mg_score(piece_scores[p]) + mg_score(psqt_scores[p][sq]);
            PSQT[p][sq][EG] = eg_score(piece_scores[p]) + eg_score(psqt_scores[p][sq]);
        }
    }
}

int32_t eval_position(Position& position) {
    const uint8_t wQ = position.get_piece_count(WHITE_QUEEN);
    const uint8_t bQ = position.get_piece_count(BLACK_QUEEN);

    const uint8_t wR = position.get_piece_count(WHITE_ROOK);
    const uint8_t bR = position.get_piece_count(BLACK_ROOK);

    const uint8_t wB = position.get_piece_count(WHITE_BISHOP);
    const uint8_t bB = position.get_piece_count(BLACK_BISHOP);

    const uint8_t wN = position.get_piece_count(WHITE_KNIGHT);
    const uint8_t bN = position.get_piece_count(BLACK_KNIGHT);

    const uint8_t q = wQ + bQ;
    const uint8_t r = wR + bR;
    const uint8_t b = wB + bB;
    const uint8_t n = wN + bN;

    int32_t phase = 24 - (4 * q) - (2 * r) - (1 * b) - (1 * n);

    int32_t eval_mg = 0;
    int32_t eval_eg = 0;

    for (int32_t sq = A1; sq <= H8; sq++)
    {
        const Piece piece = position.get_piece(static_cast<Square>(sq));

        if (piece == NO_PIECE)
        {
            continue;
        }

        const PieceType ptype  = PIECE_GET_TYPE(piece);
        const Color     pcolor = PIECE_GET_COLOR(piece);

        switch (pcolor)
        {
            case WHITE :
                eval_mg += PSQT[ptype - 1][SQUARES_MIRRORED[sq]][MG];
                eval_eg += PSQT[ptype - 1][SQUARES_MIRRORED[sq]][EG];
                break;
            case BLACK :
                eval_mg -= PSQT[ptype - 1][sq][MG];
                eval_eg -= PSQT[ptype - 1][sq][EG];
                break;
            default :
                break;
        }
    }

    int32_t eval = (eval_mg * phase + eval_eg * (24 - phase)) / 24;

    const int8_t side_to_move = 1 - (2 * position.get_active_color());
    eval *= side_to_move;

    return eval;
}

bool eval_is_end_game(Position& position) {
    const uint8_t wQ = position.get_piece_count(WHITE_QUEEN);
    const uint8_t bQ = position.get_piece_count(BLACK_QUEEN);

    const uint8_t wR = position.get_piece_count(WHITE_ROOK);
    const uint8_t bR = position.get_piece_count(BLACK_ROOK);

    const uint8_t wB = position.get_piece_count(WHITE_BISHOP);
    const uint8_t bB = position.get_piece_count(BLACK_BISHOP);

    const uint8_t wN = position.get_piece_count(WHITE_KNIGHT);
    const uint8_t bN = position.get_piece_count(BLACK_KNIGHT);

    bool is_end_game = false;

    if (wQ == 0 && bQ == 0)
        is_end_game = true;
    else if (wQ == 1 && bQ == 1 && wR == 0 && bR == 0)
    {
        if ((wN + wB) <= 1 && (bN + bB) <= 1)
            is_end_game = true;
    }

    return is_end_game;
}
