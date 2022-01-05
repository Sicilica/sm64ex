#ifndef BINGO_H
#define BINGO_H

#include "game/save_file.h"

#define BINGO_VERSION "ALPHA 26"
#define BINGO_MAX_PLAYERS 8

extern bool displayFullsizeBingoBoard;
extern bool bingoInitComplete;

extern char bingoDebugBuffer[];

extern unsigned int bingoSeed;

void init_bingo(void);
void handle_bingo_input(void);
void render_bingo_board(void);

void bingo_debug_text(char* buffer, int maxlen);

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
  s32 playerFlags;
};

extern struct BingoBoardCell gBingoBoard[];

void clear_bingo_board(void);
void generate_bingo_board(unsigned int seed);

struct BingoPlayer {
  u8* mappedName;
  s16 course;
  u8 colorR;
  u8 colorG;
  u8 colorB;
};

extern struct BingoPlayer gBingoPlayers[BINGO_MAX_PLAYERS];

#endif
