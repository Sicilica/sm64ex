#include <PR/gbi.h>

#include <stdbool.h>
#include <stdlib.h>
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

bool displayFullsizeBingoBoard = FALSE;
u8* exampleBingoText;
bool bingoInitComplete = FALSE;

char bingoDebugBuffer[256];

// This all comes from charmap.txt
// Note: one problem with this routine is that kana with dakuten actually
// convert to two bytes each, and some macros (like copyright mark) convert multiple
// chars into one byte output. But we don't care for now.
u8* convert_raw_str_to_charmap(const char* raw) {
    u8* out = malloc(sizeof(u8) * strlen(raw) + 1);
    out[strlen(raw)] = 0xFF;

    char* i = raw;
    u8* j = out;
    while (*i != '\0') {
        char c = *i;
        if (c >= 'A' && c <= 'Z') {
            *j = (c - 'A') + 0x0A;
        } else if (c >= 'a' && c <= 'z') {
            *j = (c - 'a') + 0x24;
        } else if (c >= '0' && c <= '9') {
            *j = (c - '0') + 0x00;
        } else if (c == ' ') {
            *j = 0x9E;
        } else if (c == '\n') {
            *j = 0xFE;
        } else {
            *j = 0xF4; // '?'
        }

        i++;
        j++;
    }

    return out;
}

void init_bingo(void) {
    if (bingoInitComplete) {
        return;
    }

    srand(time(NULL));

    struct BingoGoal* goals = get_possible_bingo_goals();
    int goalCount;
    for (goalCount = 0; goals[goalCount].label != NULL; goalCount++);

    // exampleBingoText = convert_raw_str_to_charmap("4 stars in BOB");
    exampleBingoText = convert_raw_str_to_charmap(goals[rand() % goalCount].label);

    bingoInitComplete = TRUE;
}

void cleanup_bingo(void) {
    if (!bingoInitComplete) {
        return;
    }

    free(exampleBingoText);

    bingoInitComplete = FALSE;
}

void draw_flat_rect(s16 x, s16 y, s16 w, s16 h, u8 r, u8 g, u8 b, u8 a) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, r, g, b, a);
    gSPTextureRectangle(gDisplayListHead++, x << 2, y << 2, (x + w) << 2, (y + h) << 2, 0, 0, 0, 0, 0);
}

void render_bingo_board_helper(s16 left, s16 bottom, s16 cell_size, s16 padding, u8 alpha) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetCombineMode(gDisplayListHead++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    create_dl_ortho_matrix();

    s16 top = bottom - 5 * cell_size - 6 * padding;
    s16 right = left + 5 * cell_size + 6 * padding;

    if (padding > 0) {
        for (u8 i = 0; i < 6; i++) {
            draw_flat_rect(left, top + (cell_size + padding) * i, right-left, padding, 40, 40, 40, alpha);
        }
        for (u8 i = 0; i < 5; i++) {
            for (u8 j = 0; j < 6; j++) {
                draw_flat_rect(left + (cell_size + padding) * j, top + padding + (cell_size + padding) * i, padding, cell_size, 40, 40, 40, alpha);
            }
        }
    }

    for (u8 i = 0; i < 25; i++) {
        u8 x = i / 5;
        u8 y = i % 5;
        u8 r, g, b;
        if (i % 2) {
            r = 255;
            g = 80;
            b = 40;
        } else {
            r = 40;
            g = 80;
            b = 255;
        }
        draw_flat_rect(left + padding + (cell_size + padding) * x, top + padding + (cell_size + padding) * y, cell_size, cell_size, r, g, b, alpha);
    }

    // gDPPipeSync(gDisplayListHead++);
    // gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
}

void render_min_bingo_board(void) {
    s16 cell_size = 7;
    s16 padding = 1;
    s16 left = GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(20);
    s16 bottom = 240-20;

    render_bingo_board_helper(left, bottom, cell_size, padding, 100);
}

void render_fullsize_bingo_board(void) {
    s16 cell_size = 40;
    s16 padding = 2;
    s16 left = SCREEN_WIDTH / 2 - 5 * cell_size / 2;
    s16 bottom = 240 - 14;
    render_bingo_board_helper(left, bottom, cell_size, padding, 200);
    
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 200);
    print_generic_string(left, 20, exampleBingoText);
}

void render_bingo_board(void) {
    // TODO not the best place for this, and we never actually free
    init_bingo();

    print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(20), 0, "ALPHA 21");
    // sprintf(bingoDebugBuffer, "%d %d", gCurrSaveFileNum, gCurrCourseNum);
    bingo_debug_text(bingoDebugBuffer);
    print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(100), 0, bingoDebugBuffer);

    if (displayFullsizeBingoBoard) {
        render_fullsize_bingo_board();
    } else if (gHudDisplay.flags != HUD_DISPLAY_NONE) {
        render_min_bingo_board();
    }
}

void handle_bingo_input(void) {
    if (gPlayer3Controller->buttonDown & L_TRIG) {
        displayFullsizeBingoBoard = TRUE;
    } else {
        displayFullsizeBingoBoard = FALSE;
    }
}
