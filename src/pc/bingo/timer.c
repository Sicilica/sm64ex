#include "timer.h"

#include "locations.h"

struct BingoTimer gBingoTimer;

bool wasOnTitle;

void bingo_reset_timer(void) {
  gBingoTimer.running = FALSE;
  gBingoTimer.startTime = 0;
}

void bingo_update_timer(void) {
  BingoPlayerLocation loc = get_current_player_location();

  if (gBingoTimer.running) {
    if (loc == LOCATION_CREDITS) {
      gBingoTimer.running = FALSE;

      u32 elapsed = time(NULL) - gBingoTimer.startTime;

      const char* SEP = " ";
      char timeBuf[32];
      char* timeBufPtr = timeBuf;
      if (elapsed > 60*60) {
        sprintf(timeBufPtr, "%u%s", elapsed / 3600, SEP);
        timeBufPtr += strlen(timeBufPtr);
        if ((elapsed / 60) % 60 < 10) {
          sprintf(timeBufPtr, "0");
          timeBufPtr++;
        }
      }
      sprintf(timeBufPtr, "%u%s", (elapsed / 60) % 60, SEP);
      timeBufPtr += strlen(timeBufPtr);
      if (elapsed % 60 < 10) {
        sprintf(timeBufPtr, "0");
        timeBufPtr++;
      }
      sprintf(timeBufPtr, "%u", elapsed % 60);
      BINGO_MSG("FINISHED IN %s", timeBuf);
    }
  } else {
    if (wasOnTitle && loc != LOCATION_TITLE) {
      gBingoTimer.running = TRUE;
      gBingoTimer.startTime = time(NULL);
    }
  }

  wasOnTitle = loc == LOCATION_TITLE;
}
