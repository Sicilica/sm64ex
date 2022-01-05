#ifndef BINGO_NETWORK_H
#define BINGO_NETWORK_H

#include "sm64.h"

extern u8 gDebugNetworkPlayerName[];

void bingo_init_network(void);
void bingo_shutdown_network(void);
void bingo_update_network(void);

#endif
