#ifndef BINGO_H
#define BINGO_H

#include "game/save_file.h"

void handle_bingo_input(void);
void render_bingo_board(void);

void bingo_debug_text(char* buffer);

struct BingoStarRef {
  enum CourseNum course;
  s32 star;
};

struct BingoGoalArgs {
  s32 count;
  s32 count2;
  enum CourseNum course;
  s32 star;
  const struct BingoStarRef* starSet;
};

typedef bool (*BingoGoalFn)(struct BingoGoalArgs);

struct BingoGoal {
  const char* label;
  BingoGoalFn fn;
  struct BingoGoalArgs args;
};

struct BingoGoal* get_possible_bingo_goals(void);

#endif
