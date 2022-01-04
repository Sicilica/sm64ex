#ifndef BINGO_SAVE_H
#define BINGO_SAVE_H

#include "bingo.h"

s32 save_file_get_total_cannon_count(s32 fileIndex);
s32 save_file_get_total_coin_count(s32 fileIndex);
s32 bingo_get_star_count_from_set(const struct BingoStarRef* stars);

#endif
