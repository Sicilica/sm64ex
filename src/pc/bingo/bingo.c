// TODO cleanup imports
#include <PR/gbi.h>

#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "sm64.h"
#include "gfx_dimensions.h"
#include "game/area.h"
#include "game/game_init.h"
#include "game/ingame_menu.h"
#include "game/level_update.h"
#include "game/segment2.h"
#include "bingo.h"
#include "locations.h"
#include "players.h"
#include "text.h"

bool displayFullsizeBingoBoard = FALSE;
bool bingoInitComplete = FALSE;

char bingoDebugBuffer[256];

void init_bingo(void) {
    if (bingoInitComplete) {
        return;
    }

    init_bingo_player_location_details();

    generate_bingo_board(time(NULL));

    bingoInitComplete = TRUE;
}

void cleanup_bingo(void) {
    if (!bingoInitComplete) {
        return;
    }

    clear_bingo_board();

    bingoInitComplete = FALSE;
}

void handle_bingo_input(void) {
    // here too should be removed probs
    init_bingo();

    if (gPlayer3Controller->buttonDown & L_TRIG) {
        displayFullsizeBingoBoard = TRUE;
    } else {
        displayFullsizeBingoBoard = FALSE;
    }
}
