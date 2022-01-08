#include "locations.h"

#include "game/area.h"

#include "bingo.h"
#include "text.h"

#define LOCATION_LOBBY (COURSE_MAX+1)

extern struct BingoPlayerLocationDetails bingoPlayerLocationDetails[28] = {
  { .name = "" },
  { .name = "BOB" },
  { .name = "WF" },
  { .name = "JRB" },
  { .name = "CCM" },
  { .name = "BBH" },
  { .name = "HMC" },
  { .name = "LLL" },
  { .name = "SSL" },
  { .name = "DDD" },
  { .name = "SL" },
  { .name = "WDW" },
  { .name = "TTM" },
  { .name = "THI" },
  { .name = "TTC" },
  { .name = "RR" },
  { .name = "BITDW" },
  { .name = "BITFS" },
  { .name = "BITS" },
  { .name = "PSS" },
  { .name = "COTMC" },
  { .name = "TOTWC" },
  { .name = "VCUTM" },
  { .name = "WMOTR" },
  { .name = "SA" },
  { .name = "Credits" },
  { .name = "Lobby" },
  { .name = NULL },
};

BingoPlayerLocation get_current_player_location(void) {
  // TODO find out if we're on the title screen or in the credits
  if (gCurrCourseNum == 0) {
    return LOCATION_LOBBY;
  }
  return gCurrCourseNum;
}

void init_bingo_player_location_details(void) {
  for (struct BingoPlayerLocationDetails* it = bingoPlayerLocationDetails; it->name != NULL; it++) {
    it->dialogName = alloc_and_convert_chars_to_dialog(it->name);
  }
}
