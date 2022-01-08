#include "players.h"

#define BINGO_PLAYER_COLOR(r, g, b) ((((r << 8) + g) << 8) + b)

struct BingoPlayerState gBingoPlayers[BINGO_MAX_PLAYERS];
BingoPlayerColor bingoPlayerColors[BINGO_MAX_PLAYERS] = {
  BINGO_PLAYER_COLOR(255, 0, 0),    // RED
  BINGO_PLAYER_COLOR(0, 0, 255),    // BLUE
  BINGO_PLAYER_COLOR(0, 255, 0),    // GREEN
  BINGO_PLAYER_COLOR(127, 0, 191),  // PURPLE
  BINGO_PLAYER_COLOR(255, 127, 0),  // ORANGE
  BINGO_PLAYER_COLOR(255, 0, 255),  // PINK
  BINGO_PLAYER_COLOR(0, 127, 127),  // TEAL
  BINGO_PLAYER_COLOR(255, 255, 0),  // YELLOW
};

bool color_in_use(BingoPlayerColor color) {
  for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
    if (gBingoPlayers[i].connected && gBingoPlayers[i].color == color) {
      return TRUE;
    }
  }
  return FALSE;
}

BingoPlayerColor pick_unused_color(BingoPlayerColor preferredColor) {
  // First try to use the preferred color
  if (!color_in_use(preferredColor)) {
    return preferredColor;
  }

  // Find an unused color
  for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
    if (!color_in_use(bingoPlayerColors[i])) {
      return bingoPlayerColors[i];
    }
  }

  // Panic!
  return BINGO_PLAYER_COLOR(255, 255, 255);
}

BingoCompletion get_current_player_bingo_completion(void) {
  BingoCompletion completion = 0;
  for (int i = 0; i < 25; i++) {
    if (gBingoBoard[i].goal != NULL && gBingoBoard[i].goal->fn != NULL && gBingoBoard[i].goal->fn(gBingoBoard[i].goal->args)) {
      completion |= 1 << i;
    }
  }
  return completion;
}

int get_empty_player_index(void) {
  for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
    if (!gBingoPlayers[i].connected) {
      return i;
    }
  }
  // TODO Panic!
  return BINGO_MAX_PLAYERS;
}
