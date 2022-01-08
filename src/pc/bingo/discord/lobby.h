#ifndef DISCORD_LOBBY_H
#define DISCORD_LOBBY_H

#include "discord.h"

extern DiscordLobbyId gCurrentLobbyID;
extern char gCurrentLobbySecret[128];

void connect_to_existing_lobby(const char* secret);
void update_lobby(void);
void on_lobby_update(UNUSED void* eventData, int64_t lobbyID);
void on_member_connect(UNUSED void* eventData, int64_t lobbyID, int64_t userID);
void on_member_disconnect(UNUSED void* eventData, int64_t lobbyID, int64_t userID);
void on_member_update(UNUSED void* eventData, int64_t lobbyID, int64_t userID);
// void on_network_message(UNUSED void* eventData, int64_t lobbyID, int64_t userID, uint8_t channelID, uint8_t* data, uint32_t len);

#endif
