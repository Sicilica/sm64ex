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
#include "save.h"

// "Total Coins:"
const u8 TOTAL_COINS_TEXT[] = {
  0x1D, 0x32, 0x37, 0x24, 0x2F,
  0x9E,
  0x0C, 0x32, 0x2C, 0x31, 0x36,
  0xE6,
  0xFF,
};

// "Cannons:"
const u8 CANNONS_TEXT[] = {
  0x0C, 0x24, 0x31, 0x31, 0x32, 0x31, 0x36,
  0xE6,
  0xFF,
};

const u8 COURSE_NAMES[][8] = {
  { 0x15, 0x32, 0x25, 0x25, 0x3C, 0xFF }, // Lobby
  { 0x0B, 0x18, 0x0B, 0xFF }, // BOB
  { 0x20, 0x0F, 0xFF }, // WF
  { 0x13, 0x1B, 0x0B, 0xFF }, // JRB
  { 0x0C, 0x0C, 0x16, 0xFF }, // CCM
  { 0x0B, 0x0B, 0x11, 0xFF }, // BBH
  { 0x11, 0x16, 0x0C, 0xFF }, // HMC
  { 0x15, 0x15, 0x15, 0xFF }, // LLL
  { 0x1C, 0x1C, 0x15, 0xFF }, // SSL
  { 0x0D, 0x0D, 0x0D, 0xFF }, // DDD
  { 0x1C, 0x15, 0xFF }, // SL
  { 0x20, 0x0D, 0x20, 0xFF }, // WDW
  { 0x1D, 0x1D, 0x16, 0xFF }, // TTM
  { 0x1D, 0x11, 0x12, 0xFF }, // THI
  { 0x1D, 0x1D, 0x0C, 0xFF }, // TTC
  { 0x1B, 0x1B, 0xFF }, // RR
  { 0x0B, 0x12, 0x1D, 0x0D, 0x20, 0xFF }, // BITDW
  { 0x0B, 0x12, 0x1D, 0x0F, 0x1C, 0xFF }, // BITFS
  { 0x0B, 0x12, 0x1D, 0x1C, 0xFF }, // BITS
  { 0x19, 0x1C, 0x1C, 0xFF }, // PSS
  { 0x0C, 0x18, 0x1D, 0x16, 0x0C, 0xFF }, // COTMC
  { 0x1D, 0x1c, 0x1D, 0x20, 0x0C, 0xFF }, // TOTWC
  { 0x1F, 0x0C, 0x1E, 0x1D, 0x16, 0xFF }, // VCUTM
  { 0x20, 0x16, 0x18, 0x1D, 0x1B, 0xFF }, // WMOTR
  { 0x1C, 0x0A, 0xFF }, // SA
  { 0x0E, 0x31, 0x27, 0xFF }, // End
};

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
            draw_flat_rect(left, top + (cell_size + padding) * i, right-left, padding, 20, 20, 20, alpha);
        }
        for (u8 i = 0; i < 5; i++) {
            for (u8 j = 0; j < 6; j++) {
                draw_flat_rect(left + (cell_size + padding) * j, top + padding + (cell_size + padding) * i, padding, cell_size, 20, 20, 20, alpha);
            }
        }
    }

    for (u8 i = 0; i < 25; i++) {
        u8 x = i % 5;
        u8 y = i / 5;
        u8 r, g, b;
        if (gBingoBoard[i].goal == NULL || gBingoBoard[i].goal->fn == NULL) {
            r = 0;
            g = 0;
            b = 0;
        } else if (gBingoBoard[i].goal->fn(gBingoBoard[i].goal->args)) {
            // TODO use playerFlags instead and don't eval this here
            r = gBingoPlayers[0].colorR;
            g = gBingoPlayers[0].colorG;
            b = gBingoPlayers[0].colorB;
        } else {
            r = 40;
            g = 40;
            b = 40;
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
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    for (u8 i = 0; i < 25; i++) {
        u8 x = i % 5;
        u8 y = i / 5;
        if (gBingoBoard[i].mappedLabel != NULL) {
            // TODO this comes from ingame_menu.c, but I could extern it...
            // gDialogCharWidths[gBingoBoard[i].mappedLabel[0]];
            // TODO this 16 offset on Y is for the text height
            print_generic_string(left + padding + (cell_size + padding) * x, 240 - 16 - (bottom - (padding + cell_size) * (5 - y)), gBingoBoard[i].mappedLabel);
        }
    }

    s16 right = left + padding * 6 + cell_size * 5;
    s16 top = bottom - padding * 6 - cell_size * 5;

    u8 buffer[8];
    int_to_str(save_file_get_total_coin_count(gCurrSaveFileNum - 1), buffer);
    print_generic_string(right + 10, top + 30, TOTAL_COINS_TEXT);
    print_generic_string(right + 10 + 60, top + 30, buffer);
    int_to_str(save_file_get_total_cannon_count(gCurrSaveFileNum - 1), buffer);
    print_generic_string(right + 10, top + 30 + 16, CANNONS_TEXT);
    print_generic_string(right + 10 + 60, top + 30 + 16, buffer);

    for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
        if (gBingoPlayers[i].mappedName != NULL) {
            gDPSetEnvColor(gDisplayListHead++, gBingoPlayers[i].colorR, gBingoPlayers[i].colorG, gBingoPlayers[i].colorB, 255);
            print_generic_string(left - 100, top + 30 + 16 * i, gBingoPlayers[i].mappedName);
            print_generic_string(left - 40, top + 30 + 16 * i, COURSE_NAMES[gCurrCourseNum]);
        }
    }
}

void render_bingo_board(void) {
    // TODO not the best place for this, and we never actually free
    init_bingo();

    print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(20), 0, BINGO_VERSION);
    // sprintf(bingoDebugBuffer, "%d %d", gCurrSaveFileNum, gCurrCourseNum);
    // bingo_debug_text(bingoDebugBuffer);
    sprintf(bingoDebugBuffer, "%x", bingoSeed);
    print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(120), 0, bingoDebugBuffer);

    if (displayFullsizeBingoBoard) {
        render_fullsize_bingo_board();
    } else if (gHudDisplay.flags != HUD_DISPLAY_NONE) {
        render_min_bingo_board();
    }
}
