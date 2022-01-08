#ifndef BINGO_PLAYERS_H
#define BINGO_PLAYERS_H

#include "bingo.h"
#include "locations.h"

#define BINGO_PLAYER_NAME_MAXLEN 12

typedef u32 BingoPlayerColor;
typedef u32 BingoCompletion;

struct BingoPlayerState {
  BingoPlayerColor color;
  BingoCompletion completion;
  bool connected;
  u8 dialogName[BINGO_PLAYER_NAME_MAXLEN];
  BingoPlayerLocation location;
  void* networkInfo;
};

extern struct BingoPlayerState gBingoPlayers[BINGO_MAX_PLAYERS];
extern BingoPlayerColor bingoPlayerColors[BINGO_MAX_PLAYERS];

BingoCompletion get_current_player_bingo_completion(void);
int get_empty_player_index(void);
BingoPlayerColor pick_unused_color(BingoPlayerColor preferredColor);

#endif
