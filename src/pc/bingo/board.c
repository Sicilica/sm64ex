#include "stdlib.h"
#include "string.h"

#include "bingo.h"
#include "text.h"
#include "timer.h"

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
    if (gBingoBoard[i].labelLine1.dialog != NULL) {
      free(gBingoBoard[i].labelLine1.dialog);
    }
    if (gBingoBoard[i].labelLine2.dialog != NULL) {
      free(gBingoBoard[i].labelLine2.dialog);
    }
    gBingoBoard[i].goal = NULL;
    gBingoBoard[i].labelLine1.dialog = NULL;
    gBingoBoard[i].labelLine2.dialog = NULL;
  }
}

bool can_place_goal(struct BingoGoal* goal, int index) {
  // Check row
  int startOfRow = 5 * (index / 5);
  for (int i = startOfRow + 4; i >= startOfRow; i--) {
    struct BingoBoard* other = gBingoBoard[i].goal;
    if (other != NULL && !can_goals_appear_in_a_row(goal, other)) {
      return false;
    }
  }

  // Check column
  for (int i = index + 20; i > index; i -= 5) {
    struct BingoBoard* other = gBingoBoard[i % 25].goal;
    if (other != NULL && !can_goals_appear_in_a_row(goal, other)) {
      return false;
    }
  }

  // Check diagonals
  if ((index % 6) == 0) {
    for (int i = 0; i <= 24; i += 6) {
      struct BingoBoard* other = gBingoBoard[i].goal;
      if (other != NULL && !can_goals_appear_in_a_row(goal, other)) {
        return false;
      }
    }
  }
  if ((index % 4) == 0 && (index % 24) != 0) {
    for (int i = 4; i <= 20; i += 4) {
      struct BingoBoard* other = gBingoBoard[i].goal;
      if (other != NULL && !can_goals_appear_in_a_row(goal, other)) {
        return false;
      }
    }
  }

  return true;
}

void prepare_goal_label(const char* label, struct BingoBoardCell* out) {
  char* buf = malloc(sizeof(char) * strlen(label) + 1);
  strcpy(buf, label);
  int newline;
  for (newline = 0; buf[newline] != '\0'; newline++) {
    if (buf[newline] == '\n') {
      buf[newline] = '\0';
      newline++;
      break;
    }
  }
  out->labelLine1.dialog = alloc_and_convert_chars_to_dialog(buf);
  out->labelLine2.dialog = alloc_and_convert_chars_to_dialog(buf + newline);
  free(buf);

  out->labelLine1.width = get_string_width(out->labelLine1.dialog);
  out->labelLine2.width = get_string_width(out->labelLine2.dialog);
}

void recalculate_bingo_categories(void) {
  BINGO_LOG("recalculating relevant bingo categories");
  for (struct BingoGoalMetric** it2 = gBingoMetrics; *it2 != NULL; it2++) {
    struct BingoGoalMetric* it = (*it2);
    it->active = FALSE;
    for (int i = 0; i < 25; i++) {
      if (it->includesGoal(gBingoBoard[i].goal)) {
        it->active = TRUE;
        if (it->mappedLabel == NULL) {
          it->mappedLabel = alloc_and_convert_chars_to_dialog(it->label);
          it->labelWidth = get_string_width(it->mappedLabel);
        }
        if (it->maximum == NULL && it->calcMaximum != NULL) {
          it->maximum = it->calcMaximum();
        }
        break;
      }
    }
  }
}

void generate_bingo_board(unsigned int seed, struct BingoConfig config) {
  BINGO_LOG("generating bingo board with seed %u", seed);

  bingo_reset_timer();

  clear_bingo_board();

  struct BingoGoal** possibleGoals = get_possible_bingo_goals(config);

  int numPossibleGoals;
  for (numPossibleGoals = 0; possibleGoals[numPossibleGoals] != NULL; numPossibleGoals++);

  srand(seed);

  int* goalIndices = malloc(sizeof(int) * numPossibleGoals);
  for (int i = 0; i < numPossibleGoals; i++) {
    goalIndices[i] = i;
  }
  shuffle_bingo_goals(goalIndices, numPossibleGoals);

  int boardIndices[25];
  for (int i = 0; i < 25; i++) {
    boardIndices[i] = i;
  }
  shuffle_bingo_goals(boardIndices, 25);

  // NOTE: Currently, generation doesn't account for difficulty at all
  u8 goalsPlaced = 0;
  for (int i = numPossibleGoals - 1; i >= 0 && goalsPlaced < 25; i--) {
    struct BingoGoal* goal = possibleGoals[goalIndices[i]];
    
    for (int j = 0; j < 25; j++) {
      if (gBingoBoard[boardIndices[j]].goal != NULL) {
        continue;
      }

      if (can_place_goal(goal, boardIndices[j])) {
        gBingoBoard[boardIndices[j]].goal = goal;
        prepare_goal_label(goal->label, &gBingoBoard[boardIndices[j]]);
        goalsPlaced++;
        break;
      }
    }
  }
  if (goalsPlaced < 25) {
    BINGO_LOG("failed to fill the board due to conflicts! trying to fill without protections...");
    for (int i = numPossibleGoals - 1; i >= 0 && goalsPlaced < 25; i--) {
      struct BingoGoal* goal = possibleGoals[goalIndices[i]];

      bool goalAlreadyInUse = false;
      int emptySlotIndex = 0;
      for (int j = 0; j < 25; j++) {
        if (gBingoBoard[j].goal == NULL) {
          emptySlotIndex = j;
        } else if (gBingoBoard[j].goal == goal) {
          goalAlreadyInUse = true;
          break;
        }
      }
      if (goalAlreadyInUse) {
        continue;
      }

      gBingoBoard[emptySlotIndex].goal = goal;
      prepare_goal_label(goal->label, &gBingoBoard[emptySlotIndex]);
      goalsPlaced++;
    }
    if (goalsPlaced < 25) {
      BINGO_LOG("failed to fill the board even without protections!");
    }
  }

  free(goalIndices);
  free(possibleGoals);

  gBingoBoardSeed = seed;

  recalculate_bingo_categories();
}
