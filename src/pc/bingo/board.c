#include "stdlib.h"
#include "string.h"

#include "bingo.h"
#include "text.h"

#ifndef max
#define max(a,b) (((a) (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

struct BingoBoardCell gBingoBoard[25];

void shuffle_bingo_goals(struct BingoGoal* goals, int len) {
  for (len--; len > 0; len--) {
    int j = rand() % (len + 1);
    struct BingoGoal tmp = goals[len];
    goals[len] = goals[j];
    goals[j] = tmp;
  }
}

void clear_bingo_board(void) {
  for (int i = 0; i < 25; i++) {
    if (gBingoBoard[i].mappedLabel != NULL) {
      free(gBingoBoard[i].mappedLabel);
    }
    gBingoBoard[i].goal = NULL;
    gBingoBoard[i].mappedLabel = NULL;
    gBingoBoard[i].playerFlags = 0;
  }
}

void generate_bingo_board(unsigned int seed) {
  clear_bingo_board();

  struct BingoGoal* possibleGoals = get_possible_bingo_goals();

  // TODO factor this out
  int numPossibleGoals;
  for (numPossibleGoals = 0; possibleGoals[numPossibleGoals].label != NULL; numPossibleGoals++);

  srand(seed);
  shuffle_bingo_goals(possibleGoals, numPossibleGoals);

  for (int i = min(25, numPossibleGoals) - 1; i >= 0; i--) {
    // TODO this by-ref isn't great
    gBingoBoard[i].goal = &possibleGoals[i];
    gBingoBoard[i].mappedLabel = convert_raw_str_to_charmap(gBingoBoard[i].goal->label);
  }
}
