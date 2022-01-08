#include "user.h"

DiscordUserId gCurrentDiscordUserID;

void on_current_user_update(UNUSED void* eventData) {
  update_discord_current_user();
}

void update_discord_current_user(void) {
  struct DiscordUser user;
  DISCORD_CHECK(gDiscord.user->get_current_user(gDiscord.user, &user), "get_current_user");
  gCurrentDiscordUserID = user.id;
  DISCORD_LOG("updated local user");
}
