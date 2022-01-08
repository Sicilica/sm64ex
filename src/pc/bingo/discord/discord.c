#include "discord.h"

#include "pc/bingo/bingo.h"
#include "activity.h"
#include "lobby.h"
#include "user.h"

struct GDiscord gDiscord;

enum EDiscordResult discord_log_error(enum EDiscordResult result, const char* label) {
  if (result) {
    printf("[DISCORD/err] error code %d in %s\n", result, label);
  }
  return result;
}

void discord_log_hook(UNUSED void* hookData, enum EDiscordLogLevel level, const char* msg) {
  printf("[DISCORD/%d] %s\n", level, msg);
}

void get_oauth2_token_callback(UNUSED void* callbackData, enum EDiscordResult result, struct DiscordOAuth2Token* token) {
  DISCORD_CHECK(result, "get_oauth2_token");
  DISCORD_LOG("got oauth2 token");
}

void bingo_network_init(void) {
  init_bingo();
  
  if (gDiscord.core == NULL) {
    // Set up params and event hooks
    static struct DiscordCreateParams params;
    params.client_id = DISCORD_APP_ID;
    params.flags = DiscordCreateFlags_NoRequireDiscord;
    static struct IDiscordActivityEvents activityEvents;
    activityEvents.on_activity_join = on_activity_join;
    params.activity_events = &activityEvents;
    static struct IDiscordLobbyEvents lobbyEvents;
    lobbyEvents.on_lobby_update = on_lobby_update;
    lobbyEvents.on_member_connect = on_member_connect;
    lobbyEvents.on_member_disconnect = on_member_disconnect;
    lobbyEvents.on_member_update = on_member_update;
    // lobbyEvents.on_network_message = on_network_message;
    params.lobby_events = &lobbyEvents;
    static struct IDiscordUserEvents userEvents;
    userEvents.on_current_user_update = on_current_user_update;
    params.user_events = &userEvents;
    DISCORD_CHECK(DiscordCreate(DISCORD_VERSION, &params, &gDiscord.core), "DiscordCreate");
    
    // Store pointers to Discord subsystems
    gDiscord.activity = gDiscord.core->get_activity_manager(gDiscord.core);
    gDiscord.application = gDiscord.core->get_application_manager(gDiscord.core);
    gDiscord.lobby = gDiscord.core->get_lobby_manager(gDiscord.core);
    gDiscord.user = gDiscord.core->get_user_manager(gDiscord.core);

    // Do standard init stuff
    gDiscord.core->set_log_hook(gDiscord.core, DiscordLogLevel_Warn, NULL, discord_log_hook);
    register_launch_command();
    gDiscord.application->get_oauth2_token(gDiscord.application, NULL, get_oauth2_token_callback);

    // TODO figure out the best way to do this (we want to make sure we're always in a lobby no matter what happens)
  }
}

void bingo_network_shutdown(void) {
  if (gDiscord.core != NULL) {
    gDiscord.core->destroy(gDiscord.core);
    gDiscord.core = NULL;
  }
}

void bingo_network_update(void) {
  if (gDiscord.core != NULL) {
    DISCORD_CHECK(gDiscord.core->run_callbacks(gDiscord.core), "run_callbacks");
    DISCORD_CHECK(gDiscord.lobby->flush_network(gDiscord.lobby), "flush_network");
    update_activity();
    update_lobby();
  }
}
