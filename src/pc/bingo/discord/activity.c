#include "activity.h"

#include "game/area.h"

#include "lobby.h"
#include "pc/bingo/locations.h"
#include "pc/bingo/players.h"

struct DiscordActivity gDiscordActivity;
struct {
  bool initial;
  bool updateInProgress;
  int timeSinceLastUpdate;
  BingoPlayerLocation location;
  DiscordLobbyId lobbyID;
  int32_t memberCount;
} gCachedActivityData;

void on_activity_join(UNUSED void* eventData, const char* secret) {
  connect_to_existing_lobby(secret);
}

void update_activity_callback(UNUSED void* callbackData, enum EDiscordResult result) {
  gCachedActivityData.updateInProgress = false;
  gCachedActivityData.timeSinceLastUpdate = 0;
  DISCORD_CHECK(result, "update_activity");
  DISCORD_LOG("activity updated");
}

void update_activity(void) {
  // Check if we need an update
  if (gCachedActivityData.updateInProgress) {
    return;
  }
  int32_t memberCount = 0;
  for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
    if (gBingoPlayers[i].connected) {
      memberCount++;
    }
  }
  BingoPlayerLocation location = get_current_player_location();
  if (gCachedActivityData.initial && gCachedActivityData.location == location && gCachedActivityData.lobbyID == gCurrentLobbyID && gCachedActivityData.memberCount == memberCount && ++gCachedActivityData.timeSinceLastUpdate < 30*15) {
    return;
  }

  gCachedActivityData.updateInProgress = true;

  // Update activity fields
  if (!gCachedActivityData.initial) {
    gCachedActivityData.initial = true;
    gDiscordActivity.type = DiscordActivityType_Playing;
    snprintf(gDiscordActivity.assets.large_image, 128, "mario1");
  }
  if (gCachedActivityData.location != location) {
    gCachedActivityData.location = location;
    snprintf(gDiscordActivity.state, 128, bingoPlayerLocationDetails[location].name);
  }
  if (gCachedActivityData.lobbyID != gCurrentLobbyID || memberCount != gCachedActivityData.memberCount) {
    gCachedActivityData.lobbyID = gCurrentLobbyID;
    gCachedActivityData.memberCount = memberCount;
    if (gCachedActivityData.lobbyID == 0) {
      gDiscordActivity.party.id[0] = '\0';
      gDiscordActivity.secrets.join[0] = '\0';
      gDiscordActivity.party.size.current_size = 1;
      gDiscordActivity.party.size.max_size = 1;
    } else {
      snprintf(gDiscordActivity.party.id, 128, "%lld", gCachedActivityData.lobbyID);
      snprintf(gDiscordActivity.secrets.join, 128, "%lld:%s", gCachedActivityData.lobbyID, gCurrentLobbySecret);
      gDiscordActivity.party.size.current_size = memberCount;
      gDiscordActivity.party.size.max_size = BINGO_MAX_PLAYERS;
    }
  }
  // TODO time stuff, which also needs to factor into updates
  gDiscordActivity.timestamps.start = 0;

  // Execute
  gDiscord.activity->update_activity(gDiscord.activity, &gDiscordActivity, NULL, update_activity_callback);
}
