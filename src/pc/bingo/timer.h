#ifndef BINGO_TIMER_H
#define BINGO_TIMER_H

#include "bingo.h"

struct BingoTimer {
  bool running;
  u32 startTime;
};

extern struct BingoTimer gBingoTimer;

void bingo_update_timer(void);
void bingo_reset_timer(void);

#endif
