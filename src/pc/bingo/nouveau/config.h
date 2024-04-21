#ifndef BINGO_NOUVEAU_CONFIG_H
#define BINGO_NOUVEAU_CONFIG_H

#include "nouveau.h"

#include "json/cJSON.h"

extern char* host;
extern int port;
extern char* username;
extern int preferred_color;

void load_bingo_config();

#endif
