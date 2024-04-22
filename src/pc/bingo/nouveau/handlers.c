#include "handlers.h"

#include "pc/bingo/bingo.h"
#include "pc/bingo/players.h"

#include "send.h"

void handle_board(cJSON *msg) {
  int seed = cJSON_GetObjectItemCaseSensitive(msg, "s")->valueint;
  const char* config = cJSON_GetObjectItemCaseSensitive(msg, "cfg")->valuestring;

  if (config[0] == '\0') {
    NOUVEAU_LOG("no board detected; requesting new board using our config");
#ifdef _WIN64
  // Windows: we're currently in non-blocking mode, but send expects to be in blocking mode
  // (this is because windows requires us to set state on the socket instead of per-call)
  u_long mode = 1;
  if (ioctlsocket(sock, FIONBIO, &mode) != 0) {
    NOUVEAU_ERR("set blocking");
    return;
  }
#endif
    bingo_network_request_new_board(0);
#ifdef _WIN64
  mode = 1;
  if (ioctlsocket(sock, FIONBIO, &mode) != 0) {
    NOUVEAU_ERR("set non-blocking");
    return;
  }
#endif
  } else {
    NOUVEAU_LOG("got board: cfg=%s s=%d", config, seed);
    struct BingoConfig bingoConfig = bingo_config_from_string(config);
    generate_bingo_board(seed, bingoConfig);
  }
}

void handle_player_connected(cJSON *msg) {
  int remote_slot = cJSON_GetObjectItemCaseSensitive(msg, "p")->valueint;
  const char* name = cJSON_GetObjectItemCaseSensitive(msg, "name")->valuestring;
  uint32_t color = cJSON_GetObjectItemCaseSensitive(msg, "color")->valueint;

  int slot = find_empty_slot();
  if (slot < 0) {
    NOUVEAU_LOG("failed to find local slot for new player (remote slot %d)", remote_slot);
    return;
  }

  gBingoPlayers[slot].color = color;
  gBingoPlayers[slot].completion = 0;
  gBingoPlayers[slot].connected = TRUE;
  gBingoPlayers[slot].dialogName[0] = 0xFF;
  gBingoPlayers[slot].location = 0;
  gBingoPlayers[slot].networkInfo = remote_slot;

  convert_chars_to_dialog(name, gBingoPlayers[slot].dialogName, BINGO_PLAYER_NAME_MAXLEN);
  NOUVEAU_LOG("\"%s\" connected in slot %d->%d", name, slot, remote_slot);
}

void handle_player_disconnected(cJSON *msg) {
  int slot = cJSON_GetObjectItemCaseSensitive(msg, "p")->valueint;

  slot = lookup_slot(slot);
  if (slot < 0) {
    NOUVEAU_LOG("no slot for disconnected player");
    return;
  }

  gBingoPlayers[slot].connected = FALSE;
  NOUVEAU_LOG("player %d disconnected", slot);
}

void handle_player_location(cJSON *msg) {
  int slot = cJSON_GetObjectItemCaseSensitive(msg, "p")->valueint;
  int16_t loc = cJSON_GetObjectItemCaseSensitive(msg, "loc")->valueint;

  slot = lookup_slot(slot);
  if (slot < 0) {
    NOUVEAU_LOG("no slot for player location");
    return;
  }

  gBingoPlayers[slot].location = loc;
  // NOUVEAU_LOG("LOCATION[%d]: %d", slot, loc);
}

void handle_player_completion(cJSON *msg) {
  int slot = cJSON_GetObjectItemCaseSensitive(msg, "p")->valueint;
  uint32_t com = cJSON_GetObjectItemCaseSensitive(msg, "com")->valueint;

  slot = lookup_slot(slot);
  if (slot < 0) {
    NOUVEAU_LOG("no slot for player location");
    return;
  }

  gBingoPlayers[slot].completion = com;
  // NOUVEAU_LOG("COMPLETION[%d]: %d", slot->valueint, com->valueint);
}

int find_empty_slot() {
  for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
    if (gBingoPlayers[i].connected) {
      continue;
    }
    return i;
  }
  return -1;
}

int lookup_slot(int slot) {
  for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
    if (gBingoPlayers[i].connected && gBingoPlayers[i].networkInfo == slot) {
      return i;
    }
  }
  return -1;
}
