#include "stdlib.h"
#include "string.h"

#include "bingo.h"
#include "text.h"

#ifndef max
#define max(a,b) (((a) (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

struct BingoBoardCell gBingoBoard[25];
unsigned int gBingoBoardSeed;

void shuffle_bingo_goals(int* goals, int len) {
  for (len--; len > 0; len--) {
    int j = rand() % (len + 1);
    int tmp = goals[len];
    goals[len] = goals[j];
    goals[j] = tmp;
  }
}

void clear_bingo_board(void) {
  gBingoBoardSeed = 0;
  for (int i = 0; i < 25; i++) {
    if (gBingoBoard[i].mappedLabel != NULL) {
      free(gBingoBoard[i].mappedLabel);
    }
    gBingoBoard[i].goal = NULL;
    gBingoBoard[i].mappedLabel = NULL;
  }
}

void generate_bingo_board(unsigned int seed) {
  clear_bingo_board();

  struct BingoGoal* possibleGoals = get_possible_bingo_goals();

  // TODO factor this out
  int numPossibleGoals;
  for (numPossibleGoals = 0; possibleGoals[numPossibleGoals].label != NULL; numPossibleGoals++);
  
  int* indices = malloc(sizeof(int) * numPossibleGoals);
  for (int i = 0; i < numPossibleGoals; i++) {
    indices[i] = i;
  }

  srand(seed);
  shuffle_bingo_goals(indices, numPossibleGoals);

  for (int i = min(25, numPossibleGoals) - 1; i >= 0; i--) {
    int index = indices[i];
    // TODO this by-ref isn't great and makes a lot of assumptions about get_possible_bingo_goals
    gBingoBoard[i].goal = &possibleGoals[index];
    gBingoBoard[i].mappedLabel = alloc_and_convert_chars_to_dialog(gBingoBoard[i].goal->label);
  }

  free(indices);

  gBingoBoardSeed = seed;
}
