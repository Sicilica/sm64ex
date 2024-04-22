#ifndef BINGO_NOUVEAU_H
#define BINGO_NOUVEAU_H

#ifdef _WIN64
#include <winsock2.h>
#define errno WSAGetLastError()
#else
#include <errno.h>
#include <sys/socket.h>
#endif

#include <stdio.h>

#include "sm64.h"
#include "pc/bingo/network.h"

#define NOUVEAU_LOG(...) printf("[BINGO/network] ");printf(__VA_ARGS__);printf("\n")
#define NOUVEAU_ERR(...) printf("[BINGO/network] failed to ");printf(__VA_ARGS__);printf(" (%d)", errno);printf("\n")

#define TYPE_HELLO               1
#define TYPE_PLAYER_CONNECTED    2
#define TYPE_PLAYER_DISCONNECTED 3
#define TYPE_BOARD               4
#define TYPE_PLAYER_LOCATION     5
#define TYPE_PLAYER_COMPLETION   6

extern int sock;

void bingo_network_disconnect();

#endif
