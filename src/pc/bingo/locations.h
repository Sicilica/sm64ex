#ifndef BINGO_LOCATIONS_H
#define BINGO_LOCATIONS_H

#include "sm64.h"

#define COURSE_NUM_TO_LOCATION(x) (x)
#define LOCATION_TITLE (COURSE_MAX+1)
#define LOCATION_CASTLE (0)
#define LOCATION_CREDITS (COURSE_MAX)

typedef s16 BingoPlayerLocation;

struct BingoPlayerLocationDetails {
  const char* name;
  const u8* dialogName;
};

extern struct BingoPlayerLocationDetails bingoPlayerLocationDetails[28];

BingoPlayerLocation get_current_player_location(void);
void init_bingo_player_location_details(void);

#endif
