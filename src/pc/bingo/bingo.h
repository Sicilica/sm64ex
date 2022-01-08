#ifndef BINGO_H
#define BINGO_H

#include <stdbool.h>

#include "game/save_file.h"

#define BINGO_VERSION "ALPHA 37"
#define BINGO_MAX_PLAYERS 8

extern bool displayFullsizeBingoBoard;
extern bool bingoInitComplete;

extern char bingoDebugBuffer[];

void init_bingo(void);
void handle_bingo_input(void);
void render_bingo_board(void);

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

struct BingoBoardCell {
  struct BingoGoal* goal;
  u8* mappedLabel;
};

extern struct BingoBoardCell gBingoBoard[];
extern unsigned int gBingoBoardSeed;

void clear_bingo_board(void);
void generate_bingo_board(unsigned int seed);

#endif
