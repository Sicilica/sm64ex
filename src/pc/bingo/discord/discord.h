#ifndef BINGO_DISCORD_H
#define BINGO_DISCORD_H

#include <stdio.h>
#include "discord/discord_game_sdk.h"
#include "sm64.h"
#include "pc/bingo/network.h"

#define DISCORD_APP_ID 928114584416571462

#define DISCORD_CHECK(RESULT, LABEL) if (discord_log_error(RESULT, LABEL)) return
#define DISCORD_LOG(...) printf("[DISCORD/log] ");printf(__VA_ARGS__);printf("\n")

struct GDiscord {
  struct IDiscordActivityManager* activity;
  struct IDiscordApplicationManager* application;
  struct IDiscordCore* core;
  struct IDiscordLobbyManager* lobby;
  struct IDiscordUserManager* user;
};
extern struct GDiscord gDiscord;

enum EDiscordResult discord_log_error(enum EDiscordResult result, const char* label);

#endif
