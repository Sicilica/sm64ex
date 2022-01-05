#include "network.h"

#include "discord/discord_game_sdk.h"
#include "bingo.h"
#include "text.h"

u8 gDebugNetworkPlayerName[64];

struct {
  struct IDiscordCore* core;

  struct IDiscordActivityManager* activity;
  struct IDiscordApplicationManager* application;
  struct IDiscordLobbyManager* lobby;
  struct IDiscordUserManager* user;
} gDiscord;

// =======
// HELPERS
// =======

void handle_discord_err(enum EDiscordResult result) {
  if (result) {
    // TODO_log_error("[DISCORD] error code %d", result);
    exit(0);
  }
}

// ====
// CORE
// ====

void bingo_network_init(void) {
  init_bingo();

  struct DiscordCreateParams params;
  params.client_id = 928114584416571462;
  params.flags = DiscordCreateFlags_NoRequireDiscord;
  // params.event_data = &gDiscord;  // not actually useful?
  // params.user_events = register_user_events();
  // params.activity_events = register_activity_events();
  // params.lobby_events = register_lobby_events();
  // network_events.on_message, network_events.on_route_update
  // overlay_events.on_toggle
  // TODO we should make this optional and store success somewhere
  // handle_discord_err(DiscordCreate(DISCORD_VERSION, &params, &gDiscord.core));
  if (DiscordCreate(DISCORD_VERSION, &params, &gDiscord.core)) {
    // TODO report
    return;
  }

  gDiscord.activity = gDiscord.core->get_activity_manager(gDiscord.core);
  gDiscord.application = gDiscord.core->get_application_manager(gDiscord.core);
  gDiscord.lobby = gDiscord.core->get_lobby_manager(gDiscord.core);
  gDiscord.user = gDiscord.core->get_user_manager(gDiscord.core);

  // TODO register_launch_command

  // applicationManager->get_oauth2_token(applicationManager, NULL, get_oauth2_token_callback);

  struct DiscordUser user;
  // handle_discord_err(gDiscord.user->get_current_user(gDiscord.user, &user));
  if (gDiscord.user->get_current_user(gDiscord.user, &user)) {
    return;
  }
  convert_chars_to_dialog(user.username, &gDebugNetworkPlayerName, 64);
  free(gBingoPlayers[0].mappedName);
  gBingoPlayers[0].mappedName = malloc(sizeof(u8) * 64);
  memcpy(gBingoPlayers[0].mappedName, gDebugNetworkPlayerName, sizeof(u8) * 64);
}

void bingo_network_shutdown(void) {

}

void bingo_network_update(void) {

}

// void get_oauth2_token_callback(UNUSED void* data, enum EDiscordResult result, struct DiscordOAuth2Token* token) {
//   // noop
// }

// ==========
// ACTIVITIES
// ==========

// // this gets called after we join a lobby, and at no other time atm
// void activity_update(bool hosting) {
//   gCurActivity.type = DiscordActivityType_Playing;
//   // set gCurActivity.state...
//   // update current and max size...

//   // why?
//   snprintf(gCurActivity.details, MAX_VERSION_LENGTH, "%s", get_version());

//   app.activities->update_activity(app.activities, &gCurActivity, NULL, on_activity_update_callback);
// }

// void on_activity_update_callback(UNUSED void* data, enum EDiscordResult result) {
//   handle_discord_err(result);
// }

// void on_activity_join(UNUSED void* data, const char* secret) {
//   app.lobbies->connect_lobby_with_activity_secret(app.lobbies, secret, NULL, on_activity_join_callback);
// }

// // TODO rename this
// void on_activity_join_callback(UNUSED void* data, enum EDiscordResult result, struct DiscordLobby* lobby) {
//   handle_discord_err(result);
  
//   // TODO coop does the following now:
//   // - network_init as client
//   gCurActivity.type = DiscordActivityType_Playing;
//   snprintf(gCurActivity.party.id, 128, "%lld", lobby->id);
//   gCurActivity.party.size.current_size = 2;// wat
//   gCurActivity.party.size.max_size = lobby->capacity;

//   gCurLobbyId = lobby->id;

//   // discord_network_init(lobby->id)
//   // discord_activity_update(false)

//   gNetworkUserIds[0] = lobby->owner_id;
//   network_send_join_request();
// }

// struct IDiscordActivityEvents* register_activity_events(void) {
//   static struct IDiscordActivityEvents events;
//   events.on_activity_join = on_activity_join;
//   return &events;
// }

// =====
// LOBBY
// =====

// void on_lobby_update(UNUSED void* data, int64_t lobbyId) {
//   // noop
// }

// void on_member_connect(UNUSED void* data, int64_t lobbyId, int64_t userId) {
//   // is this even right?
//   gCurActivity.party.size.current_size++;
//   activity_update(true);
// }

// void on_member_update(UNUSED void* data, int64_t lobbyId, int64_t userId) {
//   // noop
// }

// void on_member_disconnect(UNUSED void* data, int64_t lobbyId, int64_t userId) {
//   // TODO
//   u8 localIndex = discord_user_id_to_local_index(userId);
//   if (localIndex != UNKNOWN_LOCAL_INDEX && gNetworkPlayers[localIndex].connected) {
//     network_player_disconnected(gNetworkPlayers[localIndex].globalIndex);
//   }
//   gCurActivity.party.size.current_size--;
//   discord_activity_update(isHosting);
// }

// void on_network_message(UNUSED void* eventData, int64_t lobbyId, int64_t userId, uint8_t channelId, uint8_t* data, uint32_t dataLength) {
//   gNetworkUserIds[0] = userId;

//   u8 localIndex = UNKNOWN_LOCAL_INDEX;
//   for (int i = 1; i < MAX_PLAYERS; i++) {
//     if (gNetworkUserIds[i] == userId) {
//       localIndex = i;
//       break;
//     }
//   }

//   network_receive(localIndex, (u8*)data, (u16)dataLength);
// }

// struct IDiscordLobbyEvents* register_lobby_events(void) {
//   static struct IDiscordLobbyEvents events;
//   events.on_lobby_update = on_lobby_update;
//   events.on_member_connect = on_member_connect;
//   events.on_member_update = on_member_update;
//   events.on_member_disconnect = on_member_disconnect;
//   events.on_network_message = on_network_message;
//   return &events;
// }

// =====
// USERS
// =====

// void on_current_user_update(UNUSED void* data) {
//   // struct DiscordUser user;
//   // app.users->get_current_user(app.users, &user);
//   // app.userId = user.id;
// }

// struct IDiscordUserEvents* register_user_events(void) {
//   static struct IDiscordUserEvents events;
//   events.on_current_user_update = on_current_user_update;
//   return &events;
// }
