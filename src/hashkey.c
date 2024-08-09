#include "include/hashkey.h"
#include "include/piece.h"

static uint8_t SQ64TO120[64] = {
  21, 22, 23, 24, 25, 26, 27, 28, 31, 32, 33, 34, 35, 36, 37, 38,
  41, 42, 43, 44, 45, 46, 47, 48, 51, 52, 53, 54, 55, 56, 57, 58,
  61, 62, 63, 64, 65, 66, 67, 68, 71, 72, 73, 74, 75, 76, 77, 78,
  81, 82, 83, 84, 85, 86, 87, 88, 91, 92, 93, 94, 95, 96, 97, 98,
};

static uint64_t HASHTABLE[12][64];
static uint64_t HASH_BLACK_TO_MOVE;
static uint64_t HASH_WKCA;
static uint64_t HASH_WQCA;
static uint64_t HASH_BKCA;
static uint64_t HASH_BQCA;

void hashkey_init(void) {
    for (uint8_t i = 0; i < 12; i++)
    {
        for (uint8_t j = 0; j < 64; j++)
        {
            HASHTABLE[i][j] = RAND_64;
        }
    }
    HASH_BLACK_TO_MOVE = RAND_64;
    HASH_WKCA          = RAND_64;
    HASH_WQCA          = RAND_64;
    HASH_BKCA          = RAND_64;
    HASH_BQCA          = RAND_64;
}

uint64_t hashkey_position(Position* position) {
    uint64_t hash = 0;
    uint8_t  sq;
    Piece    piece;

    if (position->active_color == BLACK)
    {
        hash ^= HASH_BLACK_TO_MOVE;
    }
    if (position->casteling_rights & WKCA)
    {
        hash ^= HASH_WKCA;
    }
    if (position->casteling_rights & WQCA)
    {
        hash ^= HASH_WQCA;
    }
    if (position->casteling_rights & BKCA)
    {
        hash ^= HASH_BKCA;
    }
    if (position->casteling_rights & BQCA)
    {
        hash ^= HASH_BQCA;
    }

    for (uint8_t i = 0; i < 12; i++)
    {
        for (uint8_t j = 0; j < 64; j++)
        {
            sq = SQ64TO120[j];

            if (POS_IS_SQ_INVALID(position, sq))
            {
                // TODO: Raise exception - should not occur
                exit(1);
            }
            else if (POS_IS_SQ_EMPTY(position, sq))
            {
                continue;
            }

            piece = POS_PIECE(position, sq);
            hash ^= HASHTABLE[PIECE_TO_HASH_ID(piece)][j];
        }
    }


    return hash;
}
