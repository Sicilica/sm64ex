#include "lobby.h"

#include <stdlib.h>
#include <string.h>

#include "pc/bingo/bingo.h"
#include "pc/bingo/players.h"
#include "pc/bingo/text.h"

#include "user.h"

#define METADATA_BINGO_VERSION "bingo_version"
#define METADATA_BINGO_SEED "bingo_seed"
#define METADATA_COLOR "color"
#define METADATA_LOCATION "location"
#define METADATA_BINGO_COMPLETION "bingo_completion"

// #define RELIABLE_CHANNEL_INDEX 0
// #define UNRELIABLE_CHANNEL_INDEX 1

DiscordLobbyId gCurrentLobbyID;
char gCurrentLobbySecret[128];
u8 pendingLobbyConnections;

BingoPlayerColor gCurrentPlayerColor;
struct {
  int timeSinceLastUpdate;
  BingoPlayerColor color;
  BingoCompletion completion;
  BingoPlayerLocation location;
} gCachedMemberMetadata;

char metadataReceiveBuf[4096];

void set_current_lobby(struct DiscordLobby* lobby);
void disconnect_lobby_callback(DiscordLobbyId callbackData, enum EDiscordResult result);

void connect_lobby_callback(UNUSED void* callbackData, enum EDiscordResult result, struct DiscordLobby* lobby) {
  pendingLobbyConnections--;
  DISCORD_CHECK(result, "connect_lobby");

  DISCORD_LOG("connected to lobby %lld", lobby->id);

  set_current_lobby(lobby);
}

void create_lobby_callback(UNUSED void* callbackData, enum EDiscordResult result, struct DiscordLobby* lobby) {
  pendingLobbyConnections--;
  DISCORD_CHECK(result, "create_lobby");

  DISCORD_LOG("created a lobby %lld", lobby->id);

  // If we somehow connected to another lobby in the meantime, prefer it over the newly created one
  if (gCurrentLobbyID == 0) {
    set_current_lobby(lobby);
  } else {
    gDiscord.lobby->disconnect_lobby(gDiscord.lobby, lobby->id, NULL, disconnect_lobby_callback);
  }
}

void disconnect_lobby_callback(DiscordLobbyId callbackData, enum EDiscordResult result) {
  DISCORD_CHECK(result, "disconnect_lobby");
  DISCORD_LOG("disconnected from lobby %lld", callbackData);
}

void update_member_callback(UNUSED void* callbackData, enum EDiscordResult result) {
  DISCORD_CHECK(result, "update_member");
  DISCORD_LOG("updated member");
}

void connect_to_existing_lobby(const char* secret) {
  pendingLobbyConnections++;
  if (gCurrentLobbyID != 0) {
    DISCORD_LOG("leaving current lobby before join %lld", gCurrentLobbyID);
    gDiscord.lobby->disconnect_lobby(gDiscord.lobby, gCurrentLobbyID, gCurrentLobbyID, disconnect_lobby_callback);
    gCurrentLobbyID = 0;
  }
  gDiscord.lobby->connect_lobby_with_activity_secret(gDiscord.lobby, secret, NULL, connect_lobby_callback);
}

void create_new_lobby(void) {
  pendingLobbyConnections++;
  struct IDiscordLobbyTransaction* txn;
  DISCORD_CHECK(gDiscord.lobby->get_lobby_create_transaction(gDiscord.lobby, &txn), "get_lobby_create_transaction");
  DISCORD_CHECK(txn->set_capacity(txn, BINGO_MAX_PLAYERS), "set_capacity");
  DISCORD_CHECK(txn->set_type(txn, DiscordLobbyType_Public), "set_type");
  DISCORD_CHECK(txn->set_metadata(txn, METADATA_BINGO_VERSION, BINGO_VERSION), "set_metadata[BINGO_VERSION]");
  char bingoSeed[32];
  snprintf(bingoSeed, 32, "%x", gBingoBoardSeed);
  DISCORD_CHECK(txn->set_metadata(txn, METADATA_BINGO_SEED, bingoSeed), "set_metadata[BINGO_SEED]");
  gDiscord.lobby->create_lobby(gDiscord.lobby, txn, NULL, create_lobby_callback);
}

void update_lobby(void) {
  if (gCurrentDiscordUserID == 0) {
    return;
  }

  if (gCurrentLobbyID == 0 && pendingLobbyConnections == 0) {
    DISCORD_LOG("creating new lobby");
    create_new_lobby();
  }

  if (gCurrentLobbyID != 0) {
    send_member_metadata_if_outdated(gCurrentLobbyID);
  }
}

void receive_bingo_seed(int64_t lobbyID) {
  DISCORD_CHECK(gDiscord.lobby->get_lobby_metadata_value(gDiscord.lobby, lobbyID, METADATA_BINGO_SEED, &metadataReceiveBuf), "get_lobby_metadata[BINGO_SEED]");
  DISCORD_LOG("got bingo seed %s", metadataReceiveBuf);
  generate_bingo_board(strtol(metadataReceiveBuf, NULL, 16));
  for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
    gBingoPlayers[i].completion = 0;
  }
  DISCORD_LOG("synchronized bingo seed to %u", gBingoBoardSeed);
}

void receive_member_meta(int64_t lobbyID, int64_t userID, int index) {
  DISCORD_CHECK(gDiscord.lobby->get_member_metadata_value(gDiscord.lobby, lobbyID, userID, METADATA_COLOR, &metadataReceiveBuf), "get_member_metadata[COLOR]");
  gBingoPlayers[index].color = strtol(metadataReceiveBuf, NULL, 16);
  DISCORD_CHECK(gDiscord.lobby->get_member_metadata_value(gDiscord.lobby, lobbyID, userID, METADATA_LOCATION, &metadataReceiveBuf), "get_member_metadata[LOCATION]");
  gBingoPlayers[index].location = strtol(metadataReceiveBuf, NULL, 16);
  DISCORD_CHECK(gDiscord.lobby->get_member_metadata_value(gDiscord.lobby, lobbyID, userID, METADATA_BINGO_COMPLETION, &metadataReceiveBuf), "get_member_metadata[BINGO_COMPLETION]");
  gBingoPlayers[index].completion = strtol(metadataReceiveBuf, NULL, 16);
  DISCORD_LOG("received meta for user %lld", userID);
}

void receive_new_member(int64_t lobbyID, int64_t userID, bool fetchMetadata) {
  DISCORD_LOG("member connected %lld)", userID);

  int index = get_empty_player_index();
  if (index >= BINGO_MAX_PLAYERS || index < 0) {
    DISCORD_LOG("no member slots available!");
    return;
  }

  int64_t* userIDPtr = malloc(sizeof(int64_t));
  *userIDPtr = userID;
  gBingoPlayers[index].color = 0;
  gBingoPlayers[index].completion = 0;
  gBingoPlayers[index].connected = TRUE;
  gBingoPlayers[index].dialogName[0] = 0xFF;
  gBingoPlayers[index].location = 0;
  gBingoPlayers[index].networkInfo = userIDPtr;

  if (fetchMetadata) {
    receive_member_meta(lobbyID, userID, index);
  }

  struct DiscordUser user;
  DISCORD_CHECK(gDiscord.lobby->get_member_user(gDiscord.lobby, lobbyID, userID, &user), "get_member_user");
  convert_chars_to_dialog(user.username, gBingoPlayers[index].dialogName, BINGO_PLAYER_NAME_MAXLEN);
  DISCORD_LOG("finished connecting player %s", user.username);
}

void send_member_metadata(int64_t lobbyID) {
  gCachedMemberMetadata.timeSinceLastUpdate = 0;
  struct IDiscordLobbyMemberTransaction* txn;
  DISCORD_CHECK(gDiscord.lobby->get_member_update_transaction(gDiscord.lobby, lobbyID, gCurrentDiscordUserID, &txn), "get_member_update_transaction");
  char color[16];
  snprintf(color, 16, "%x", gCurrentPlayerColor);
  DISCORD_CHECK(txn->set_metadata(txn, METADATA_COLOR, color), "set_metadata[COLOR]");
  char location[16];
  snprintf(location, 6, "%x", get_current_player_location());
  DISCORD_CHECK(txn->set_metadata(txn, METADATA_LOCATION, location), "set_metadata[LOCATION]");
  char bingoCompletion[16];
  snprintf(bingoCompletion, 16, "%x", get_current_player_bingo_completion());
  DISCORD_CHECK(txn->set_metadata(txn, METADATA_BINGO_COMPLETION, bingoCompletion), "set_metadata[BINGO_COMPLETION]");
  gDiscord.lobby->update_member(gDiscord.lobby, lobbyID, gCurrentDiscordUserID, txn, NULL, update_member_callback);
}

void send_member_metadata_if_outdated(int64_t lobbyID) {
  // if (gCachedMemberMetadata.updateInProgress) {
  //   return;
  // }
  BingoCompletion completion = get_current_player_bingo_completion();
  BingoPlayerLocation location = get_current_player_location();
  if (gCachedMemberMetadata.color == gCurrentPlayerColor && gCachedMemberMetadata.completion == completion && gCachedMemberMetadata.location == location && ++gCachedMemberMetadata.timeSinceLastUpdate < 30*15) {
    return;
  }
  gCachedMemberMetadata.color = gCurrentPlayerColor;
  gCachedMemberMetadata.completion = completion;
  gCachedMemberMetadata.location = location;
  send_member_metadata(lobbyID);
}

void set_current_lobby(struct DiscordLobby* lobby) {
  DISCORD_LOG("setting current lobby %lld", lobby->id);
  if (gCurrentLobbyID == lobby->id) {
    DISCORD_LOG("new and old lobby are the same!");
  } else if (gCurrentLobbyID != 0) {
    DISCORD_LOG("in two lobbies; leaving %lld", gCurrentLobbyID);
    gDiscord.lobby->disconnect_lobby(gDiscord.lobby, gCurrentLobbyID, gCurrentLobbyID, disconnect_lobby_callback);
  }

  gCurrentLobbyID = lobby->id;
  memcpy(gCurrentLobbySecret, lobby->secret, 128);

  // Validate bingo version
  DISCORD_CHECK(gDiscord.lobby->get_lobby_metadata_value(gDiscord.lobby, lobby->id, METADATA_BINGO_VERSION, &metadataReceiveBuf), "get_lobby_metadata[BINGO_VERSION]");
  if (strcmp(metadataReceiveBuf, BINGO_VERSION)) {
    DISCORD_LOG("bingo version mismatch!");
    gDiscord.lobby->disconnect_lobby(gDiscord.lobby, lobby->id, lobby->id, disconnect_lobby_callback);
    return;
  }

  // Sync bingo board
  receive_bingo_seed(lobby->id);

  // Initialize all previously connected members
  int32_t memberCount;
  DISCORD_CHECK(gDiscord.lobby->member_count(gDiscord.lobby, lobby->id, &memberCount), "member_count");
  for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
    gBingoPlayers[i].connected = FALSE;
    if (gBingoPlayers[i].networkInfo != NULL) {
      free(gBingoPlayers[i].networkInfo);
    }
    if (i < memberCount) {
      DiscordUserId userID;
      DISCORD_CHECK(gDiscord.lobby->get_member_user_id(gDiscord.lobby, lobby->id, i, &userID), "get_member_user_id");
      receive_new_member(lobby->id, userID, userID != gCurrentDiscordUserID);
    }
  }
  
  // Pick own color and send to lobby
  gCurrentPlayerColor = pick_unused_color(bingoPlayerColors[0]);
  send_member_metadata(lobby->id);

  DISCORD_LOG("done joining lobby");

  // gDiscord.lobby->open_network_channel(gDiscord.lobby, lobby->id, RELIABLE_CHANNEL_INDEX, true);
  // gDiscord.lobby->open_network_channel(gDiscord.lobby, lobby->id, UNRELIABLE_CHANNEL_INDEX, false);

  // int32_t memberCount;
  // DISCORD_CHECK(gDiscord.lobby->member_count(gDiscord.lobby, lobby->id, &memberCount), "member_count");
  // // TODO how about we don't send messages to ourselves tho
  // for (int i = 0; i < memberCount; i++) {
  //   // gDiscord.lobby->send_lobby_message(gDiscord.lobby, lobby->id, TODO, TODO, NULL, send_lobby_message_callback);
  //   DiscordUserId userID;
  //   DISCORD_CHECK(gDiscord.lobby->get_member_user_id(gDiscord.lobby, lobby->id, i, &userID), "get_member_user_id");
  //   DISCORD_LOG("sending greeting to user %lld", userID);
  //   const char* msg = "yo waddup ya highness";
  //   gDiscord.lobby->send_network_message(gDiscord.lobby, lobby->id, userID, RELIABLE_CHANNEL_INDEX, msg, strlen(msg));
  // }

  // gDiscord.lobby->get_lobby_activity_secret(gDiscord.lobby, lobby->id, &lobby->secret);
  
  // network_init as client
  // discord_network_init(lobby->id)
  // gNetworkUserIds[0] = lobby->owner_id
  // network_send_join_request
}

int get_player_index_from_user_id(int64_t userID) {
  for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
    if (gBingoPlayers[i].connected && gBingoPlayers[i].networkInfo != NULL && *(int64_t*)(gBingoPlayers[i].networkInfo) == userID) {
      return i;
    }
  }
  // TODO Panic!
  return BINGO_MAX_PLAYERS;
}

void on_lobby_update(UNUSED void* eventData, int64_t lobbyID) {
  receive_bingo_seed(lobbyID);
}

void on_member_connect(UNUSED void* eventData, int64_t lobbyID, int64_t userID) {
  receive_new_member(lobbyID, userID, true);
}

void on_member_disconnect(UNUSED void* eventData, int64_t lobbyID, int64_t userID) {
  int index = get_player_index_from_user_id(userID);
  if (index >= BINGO_MAX_PLAYERS || index < 0) {
    DISCORD_LOG("got member disconnect for unknown player %lld", userID);
    return;
  }
  gBingoPlayers[index].connected = FALSE;
  if (gBingoPlayers[index].networkInfo != NULL) {
    free(gBingoPlayers[index].networkInfo);
  }
  DISCORD_LOG("member disconnected %lld", userID);
}

void on_member_update(UNUSED void* eventData, int64_t lobbyID, int64_t userID) {
  int index = get_player_index_from_user_id(userID);
  if (index >= BINGO_MAX_PLAYERS || index < 0) {
    DISCORD_LOG("got member update for unknown player %lld", userID);
    return;
  }
  receive_member_meta(lobbyID, userID, index);
}

// void on_network_message(UNUSED void* eventData, int64_t lobbyID, int64_t userID, uint8_t channelID, uint8_t* data, uint32_t len) {
//   // TODO
//   DISCORD_LOG("received network message (L=%lld, U=%lld, ch=%d, len=%d)", lobbyID, userID, channelID, len);
// }
