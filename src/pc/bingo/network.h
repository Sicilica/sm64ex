#ifndef BINGO_NETWORK_H
#define BINGO_NETWORK_H

#include "sm64.h"

void bingo_network_init(void);
void bingo_network_shutdown(void);
void bingo_network_update(void);

void bingo_network_request_new_board(unsigned int seed);

#endif