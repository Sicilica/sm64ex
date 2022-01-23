#include "locations.h"

#include "game/area.h"
#include "game/level_update.h"

#include "bingo.h"
#include "text.h"

bool gOnTitleScreen;

struct BingoPlayerLocationDetails bingoPlayerLocationDetails[28] = {
  { .name = "Castle" },
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
  // Above: actual course nums defined in game
  // Below: virtual locations
  { .name = "Title" },
  { .name = NULL },
};

BingoPlayerLocation get_current_player_location(void) {
  if (gOnTitleScreen) {
    // this is used for both the title screen and file select
    return LOCATION_TITLE;
  }
  if (gMarioState->action == ACT_INTRO_CUTSCENE) {
    // listening to peach letter; we actually already know that we're in course 0 in this case
    return LOCATION_CASTLE;
  }
  if (gMarioState->action == ACT_JUMBO_STAR_CUTSCENE || gMarioState->action == ACT_END_PEACH_CUTSCENE || gMarioState->action == ACT_CREDITS_CUTSCENE || gMarioState->action == ACT_END_WAVING_CUTSCENE) {
    // grand star cutscene, both peach cutscenes, credit sequence, cake screen
    return LOCATION_CREDITS;
  }
  return gCurrCourseNum;
}

void init_bingo_player_location_details(void) {
  for (struct BingoPlayerLocationDetails* it = bingoPlayerLocationDetails; it->name != NULL; it++) {
    it->dialogName = alloc_and_convert_chars_to_dialog(it->name);
  }
}

void bingo_set_player_on_title(bool state) {
  gOnTitleScreen = state;
}
