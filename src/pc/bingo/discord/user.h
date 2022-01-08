#ifndef DISCORD_USER_H
#define DISCORD_USER_H

#include "discord.h"

extern DiscordUserId gCurrentDiscordUserID;

void on_current_user_update(UNUSED void* eventData);
void update_discord_current_user(void);

#endif
