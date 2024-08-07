#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "move.h"
#include "position.h"
#include <stdint.h>

typedef struct {
    Move                  move;
    struct MovesListNode* next;
} MovesListNode;

MovesListNode* generate_pseudo_legal_moves(const Position* position);
void           print_moves_list(MovesListNode* head);
void           delete_moves_list(MovesListNode* head);

#endif
