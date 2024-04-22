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
#include "network.h"
#include "players.h"
#include "text.h"

bool displayFullsizeBingoBoard = FALSE;
bool bingoInitComplete = FALSE;

char bingoMsgBuffer[256];
int bingoMsgTimer;

void init_bingo(void) {
    if (bingoInitComplete) {
        return;
    }

    init_bingo_player_location_details();

    generate_bingo_board(time(NULL), gBingoConfig);

    bingoInitComplete = TRUE;

    // TODO this is unnecessary because nouveau echoes it instead
    BINGO_MSG("BINGO %s", BINGO_VERSION);
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

    // TODO this should absolutely not be here
    bingo_update_timer();

    if (gPlayer1Controller->buttonDown & L_TRIG || gPlayer3Controller->buttonDown & L_TRIG) {
        displayFullsizeBingoBoard = TRUE;
    } else {
        displayFullsizeBingoBoard = FALSE;
    }
}

void bingo_request_new_board(void) {
    BINGO_LOG("requesting new board");
    bingo_network_request_new_board(time(NULL));
    return true;
}
