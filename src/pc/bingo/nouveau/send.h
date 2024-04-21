#ifndef BINGO_NOUVEAU_SEND_H
#define BINGO_NOUVEAU_SEND_H

#include "nouveau.h"

void send_keepalive();
void send_board_request(const char* config);
void send_completion(uint32_t completion);
void send_hello(const char* name);
void send_location(int16_t location);

#endif
