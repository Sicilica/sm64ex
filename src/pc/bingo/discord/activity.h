#ifndef DISCORD_ACTIVITY_H
#define DISCORD_ACTIVITY_H

#include "discord.h"

void on_activity_join(UNUSED void* eventData, const char* secret);
void update_activity(void);

#endif
