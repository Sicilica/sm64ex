#include "offline.h"

#include "pc/bingo/bingo.h"
#include "pc/bingo/players.h"

void bingo_network_init(void) {
  init_bingo();

  gBingoPlayers[0].connected = true;
  gBingoPlayers[0].color = bingoPlayerColors[0];
  convert_chars_to_dialog("Player", gBingoPlayers[0].dialogName, BINGO_PLAYER_NAME_MAXLEN);

  // // TMP DEBUG STUFF
  // for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
  //   gBingoPlayers[i].connected = true;
  //   gBingoPlayers[i].color = bingoPlayerColors[i];
  //   convert_chars_to_dialog("Player", gBingoPlayers[i].dialogName, BINGO_PLAYER_NAME_MAXLEN);
  //   gBingoPlayers[i].completion = (u32)i | 0x8000;
  // }
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
