#ifndef BINGO_NOUVEAU_HANDLERS_H
#define BINGO_NOUVEAU_HANDLERS_H

#include "nouveau.h"

#include "json/cJSON.h"

void handle_board(cJSON *msg);
void handle_player_connected(cJSON *msg);
void handle_player_disconnected(cJSON *msg);
void handle_player_location(cJSON *msg);
void handle_player_completion(cJSON *msg);

#endif
