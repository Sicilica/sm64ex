#ifndef BINGO_NOUVEAU_H
#define BINGO_NOUVEAU_H

#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>

#include "sm64.h"
#include "pc/bingo/network.h"

#define NOUVEAU_LOG(...) printf("[BINGO/nouveau] ");printf(__VA_ARGS__);printf("\n")
#define NOUVEAU_ERR(...) printf("[BINGO/nouveau] failed to ");printf(__VA_ARGS__);printf(" (%d)", errno);printf("\n")

#define TYPE_HELLO               1
#define TYPE_PLAYER_CONNECTED    2
#define TYPE_PLAYER_DISCONNECTED 3
#define TYPE_BOARD               4
#define TYPE_PLAYER_LOCATION     5
#define TYPE_PLAYER_COMPLETION   6

extern int sock;

#endif
