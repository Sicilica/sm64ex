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
#include "text.h"

bool displayFullsizeBingoBoard = FALSE;
bool bingoInitComplete = FALSE;

char bingoDebugBuffer[256];

unsigned int bingoSeed;

struct BingoPlayer gBingoPlayers[BINGO_MAX_PLAYERS];

void init_bingo(void) {
    if (bingoInitComplete) {
        return;
    }

    bingoSeed = time(NULL);
    generate_bingo_board(bingoSeed);

    gBingoPlayers[0].mappedName = convert_raw_str_to_charmap("Player1");
    gBingoPlayers[0].colorR = 255;
    gBingoPlayers[0].colorG = 80;
    gBingoPlayers[0].colorB = 40;

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
