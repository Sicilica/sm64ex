#include "network.h"

#include <time.h>
#include "discord/discord_game_sdk.h"
#include "bingo.h"
#include "text.h"

#define DISCORD_APP_ID 928114584416571462

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <winuser.h>
#else
#include <unistd.h>
#define MAX_PATH 1024
#endif
#define MAX_LAUNCH_CMD (MAX_PATH + 12)

int gActivityUpdateTimer;
int64_t gDiscordActivityStartTime;

struct DiscordActivity activityData;
struct DiscordLobby* currentLobby;
int currentLobbySize;

struct {
  struct IDiscordCore* core;

  struct IDiscordActivityManager* activity;
  struct IDiscordApplicationManager* application;
  struct IDiscordLobbyManager* lobby;
  struct IDiscordUserManager* user;
} gDiscord;

const char ACTIVITY_COURSE_NAMES[][8] = {
  "Lobby",
  "BOB",
  "WF",
  "JRB",
  "CCM",
  "BBH",
  "HMC",
  "LLL",
  "SSL",
  "DDD",
  "SL",
  "WDW",
  "TTM",
  "THI",
  "TTC",
  "RR",
  "BITDW",
  "BITFS",
  "BITS",
  "PSS",
  "COTMC",
  "TOTWC",
  "VCUTM",
  "WMOTR",
  "SA",
  "End",
};

// =======
// HELPERS
// =======

void TODO_update_player_name(const char* name) {
  free(gBingoPlayers[0].mappedName);
  gBingoPlayers[0].mappedName = alloc_and_convert_chars_to_dialog(name);
}

// #define DISCORD_REQUIRED(RESULT, LABEL) if (discord_log_error(RESULT, LABEL)) exit(0)
#define DISCORD_OPTIONAL(RESULT, LABEL) if (discord_log_error(RESULT, LABEL)) return

enum EDiscordResult discord_log_error(enum EDiscordResult result, const char* label) {
  if (result) {
    // TODO report this better
    printf("[DISCORD] error %d in %s\n", result, label);
  }
  return result;
}

void discord_log_hook(UNUSED void* hookData, enum EDiscordLogLevel level, const char* msg) {
  printf("[DISCORD/%d] %s\n", level, msg);
}

// ====
// CORE
// ====

void register_launch_command(void) {
  char cmd[MAX_LAUNCH_CMD];
  int rc;
#if defined(_WIN32) || defined(_WIN64)
  HMODULE hModule = GetModuleHandle(NULL);
  if (hModule == NULL) {
    // LOG_ERROR("unable to retrieve absolute path!");
    return;
  }
  GetModuleFileName(hModule, cmd, sizeof(cmd));
#else
  rc = readlink("/proc/self/exe", cmd, sizeof(MAX_LAUNCH_CMD) - 1);
  if (rc) {
    // LOG_ERROR("unable to retrieve absolute path! rc = %d", rc);
    return;
  }
#endif
  // strncat(cmd, " --discord 1", MAX_LAUNCH_CMD - 1);
  DISCORD_OPTIONAL(gDiscord.activity->register_command(gDiscord.activity, cmd), "register_command");
  printf("[DISCORD] registered launch command: %s\n", cmd);
}

void bingo_update_local_user(void) {
  struct DiscordUser user;
  DISCORD_OPTIONAL(gDiscord.user->get_current_user(gDiscord.user, &user), "get_current_user");
  TODO_update_player_name(user.username);
  printf("[DISCORD] updated local user\n");
  // convert_chars_to_dialog(user.username, gDebugNetworkPlayerName, 64);
  // free(gBingoPlayers[0].mappedName);
  // gBingoPlayers[0].mappedName = malloc(sizeof(u8) * 64);
  // memcpy(gBingoPlayers[0].mappedName, gDebugNetworkPlayerName, sizeof(u8) * 64);
}

void on_update_activity(UNUSED void* _data, enum EDiscordResult result) {
  DISCORD_OPTIONAL(result, "update_activity");
  printf("[DISCORD] activity updated\n");

  gActivityUpdateTimer = 30 * 5;
}

void bingo_update_activity(void) {
  // TODO instead of setting all of this every time, maybe only set changed values?
  activityData.type = DiscordActivityType_Playing;
  snprintf(activityData.assets.large_image, 128, "mario1");
  snprintf(activityData.state, 128, ACTIVITY_COURSE_NAMES[gCurrCourseNum]);
  if (currentLobby != NULL) {
    snprintf(activityData.party.id, 128, "%lld", currentLobby->id);
    snprintf(activityData.secrets.join, 128, "%lld:%s", currentLobby->id, currentLobby->secret);
    activityData.party.size.current_size = currentLobbySize;
    activityData.party.size.max_size = currentLobby->capacity;
  } else {
    activityData.party.id[0] = '\0';
    activityData.secrets.join[0] = '\0';
    activityData.party.size.current_size = 0;
    activityData.party.size.max_size = 0;
  }
  activityData.timestamps.start = gDiscordActivityStartTime;
  gDiscord.activity->update_activity(gDiscord.activity, &activityData, NULL, on_update_activity);
}

void get_oauth2_token_callback(UNUSED void* _data, enum EDiscordResult result, struct DiscordOAuth2Token* token) {
  DISCORD_OPTIONAL(result, "get_oauth2_token");
  printf("[DISCORD] got oauth2 token\n");
}

void on_current_user_update(UNUSED void* _data) {
  bingo_update_local_user();
}

void connect_lobby_callback(UNUSED void* _data, enum EDiscordResult result, struct DiscordLobby* lobby) {
  DISCORD_OPTIONAL(result, "connect_lobby");

  currentLobby = lobby;
  
  printf("[DISCORD] connected to lobby\n");
  // TODO coop does the following now:
  // network_init as client
  // discord_network_init(lobby->id)
  // gNetworkUserIds[0] = lobby->owner_id;
  // network_send_join_request();
}

void on_activity_join(UNUSED void* _data, const char* secret) {
  gDiscord.lobby->connect_lobby_with_activity_secret(gDiscord.lobby, secret, NULL, connect_lobby_callback);
}

void on_member_connect(UNUSED void* _data, int64_t lobbyID, int64_t userID) {
  // TODO
  currentLobbySize++;
  printf("[DISCORD] member connected (%d, %d)\n", lobbyID, userID);
}

void on_member_disconnect(UNUSED void* _data, int64_t lobbyID, int64_t userID) {
  // TODO
  currentLobbySize--;
  printf("[DISCORD] member disconnected (%d, %d)\n", lobbyID, userID);
}

void on_network_message(UNUSED void* _data, int64_t lobbyID, int64_t userID, uint8_t channelID, uint8_t* data, uint32_t len) {
  // TODO
  printf("[DISCORD] network message (%d, %d) ch%d, %d bytes\n", lobbyID, userID, channelID, len);
}

void bingo_network_init(void) {
  init_bingo();

  gDiscordActivityStartTime = time(NULL);

  static struct DiscordCreateParams params;
  params.client_id = DISCORD_APP_ID;
  params.flags = DiscordCreateFlags_NoRequireDiscord;
  static struct IDiscordUserEvents userEvents;
  userEvents.on_current_user_update = on_current_user_update;
  params.user_events = &userEvents;
  static struct IDiscordActivityEvents activityEvents;
  activityEvents.on_activity_join = on_activity_join;
  params.activity_events = &activityEvents;
  static struct IDiscordLobbyEvents lobbyEvents;
  lobbyEvents.on_member_connect = on_member_connect;
  lobbyEvents.on_member_disconnect = on_member_disconnect;
  lobbyEvents.on_network_message = on_network_message;
  // on_member_update, on_lobby_update
  params.lobby_events = &lobbyEvents;
  // network_events.on_message, network_events.on_route_update
  // overlay_events.on_toggle
  // TODO we should make this optional and store success somewhere
  DISCORD_OPTIONAL(DiscordCreate(DISCORD_VERSION, &params, &gDiscord.core), "DiscordCreate");

  gDiscord.activity = gDiscord.core->get_activity_manager(gDiscord.core);
  gDiscord.application = gDiscord.core->get_application_manager(gDiscord.core);
  gDiscord.lobby = gDiscord.core->get_lobby_manager(gDiscord.core);
  gDiscord.user = gDiscord.core->get_user_manager(gDiscord.core);

  gDiscord.core->set_log_hook(gDiscord.core, DiscordLogLevel_Warn, NULL, discord_log_hook);

  register_launch_command();

  gDiscord.application->get_oauth2_token(gDiscord.application, NULL, get_oauth2_token_callback);

  // TODO automatically create a lobby on start up (?), unless you've already joined one super fast
  // (alternatively, explicitly create lobbies in the dynos menu...)

  // TODO instead of using a timer, maybe automatically detect when the activity is dirty (i.e., gCurrLevel is different than expected)
  gActivityUpdateTimer = 0;
}

void bingo_network_shutdown(void) {
  if (gDiscord.core != NULL) {
    gDiscord.core->destroy(gDiscord.core);
    gDiscord.core = NULL;
  }
}

void bingo_network_update(void) {
  if (gDiscord.core != NULL) {
    DISCORD_OPTIONAL(gDiscord.core->run_callbacks(gDiscord.core), "run_callbacks");
    DISCORD_OPTIONAL(gDiscord.lobby->flush_network(gDiscord.lobby), "flush_network");
    
    if (!gActivityUpdateTimer--) {
      bingo_update_activity();
    }
  }
}

// =====
// LOBBY
// =====

// void on_lobby_update(UNUSED void* _data, int64_t lobbyId) {
//   // noop
// }

// void on_member_connect(UNUSED void* _data, int64_t lobbyId, int64_t userId) {
//   // is this even right?
//   gCurActivity.party.size.current_size++;
//   activity_update(true);
// }

// void on_member_update(UNUSED void* _data, int64_t lobbyId, int64_t userId) {
//   // noop
// }

// void on_member_disconnect(UNUSED void* _data, int64_t lobbyId, int64_t userId) {
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
