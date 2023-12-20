#include "offline.h"

#include "pc/bingo/bingo.h"
#include "pc/bingo/players.h"

void bingo_network_init(void) {
  init_bingo();

  gBingoPlayers[0].connected = true;
  gBingoPlayers[0].color = bingoPlayerColors[0];
  convert_chars_to_dialog("Player", gBingoPlayers[0].dialogName, BINGO_PLAYER_NAME_MAXLEN);
}

void bingo_network_shutdown(void) {
  // noop
}

void bingo_network_update(void) {
  gBingoPlayers[0].completion = get_current_player_bingo_completion();
  gBingoPlayers[0].location = get_current_player_location();
}


void bingo_network_request_new_board(unsigned int seed) {
  generate_bingo_board(seed, gBingoConfig);
}

