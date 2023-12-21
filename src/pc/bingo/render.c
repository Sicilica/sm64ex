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
#include "save.h"

int gBingoBoardFlashTimer;

// " of "
const u8 METRIC_DIVIDER[] = {
    0x9E, 0x32, 0x29, 0x9E,
    0xFF,
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
        } else {
            int completedPlayersCount = 0;
            int completedPlayers[BINGO_MAX_PLAYERS];
            u32 completionMask = 1 << i;
            for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
                if (gBingoPlayers[i].connected && (gBingoPlayers[i].completion & completionMask)) {
                    completedPlayers[completedPlayersCount++] = i;
                }
            }
            if (completedPlayersCount > 0) {
                int displayPlayer = completedPlayers[(gBingoBoardFlashTimer / 30) % completedPlayersCount];
                r = (gBingoPlayers[displayPlayer].color >> 16) & 0xFF;
                g = (gBingoPlayers[displayPlayer].color >> 8) & 0xFF;
                b = gBingoPlayers[displayPlayer].color & 0xFF;
            } else {
                r = 40;
                g = 40;
                b = 40;
            }
        }
        draw_flat_rect(left + padding + (cell_size + padding) * x, top + padding + (cell_size + padding) * y, cell_size, cell_size, r, g, b, alpha);
    }

    gBingoBoardFlashTimer++;

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

void int_to_str_4digit(s32 num, u8* dst) {
    if (num < 1000) {
        return int_to_str(num, dst);
    }

    s32 digit1;
    s32 digit2;
    s32 digit3;
    s32 digit4;

    s8 pos = 0;

    if (num > 9999) {
        dst[0] = 0x00; dst[1] = DIALOG_CHAR_TERMINATOR;
        return;
    }

    digit1 = num / 1000;
    digit2 = (num - digit1 * 1000) / 100;
    digit3 = ((num - digit1 * 1000) - (digit2 * 100)) / 10;
    digit4 = (num - digit1 * 1000) - (digit2 * 100) - (digit3 * 10);

    if (digit1 != 0) {
        dst[pos++] = digit1;
    }

    if (digit2 != 0 || digit1 != 0) {
        dst[pos++] = digit2;
    }

    if (digit3 != 0 || digit2 != 0 || digit1 != 0) {
        dst[pos++] = digit3;
    }

    dst[pos++] = digit4;
    dst[pos] = DIALOG_CHAR_TERMINATOR;
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
        if (gBingoBoard[i].labelLine1.dialog != NULL) {
            // TODO this 16 offset on Y is for the text height
            // and, the 3 is to center it (technically 4 would be perfectly centered)
            print_generic_string(left + padding + (cell_size + padding) * x + (cell_size - gBingoBoard[i].labelLine1.width)/2, 240 - 3 - 16 - (bottom - (padding + cell_size) * (5 - y)), gBingoBoard[i].labelLine1.dialog);
        }
        if (gBingoBoard[i].labelLine2.dialog != NULL) {
            // TODO this 32 offset on Y is for the text height
            // and, the 3 is to center it (technically 4 would be perfectly centered)
            print_generic_string(left + padding + (cell_size + padding) * x + (cell_size - gBingoBoard[i].labelLine2.width)/2, 240 - 3 - 32 - (bottom - (padding + cell_size) * (5 - y)), gBingoBoard[i].labelLine2.dialog);
        }
    }

    s16 right = left + padding * 6 + cell_size * 5;
    s16 top = bottom - padding * 6 - cell_size * 5;

    u8 buffer[8];
    s16 metrics_index = 0;
    bool onTitle = get_current_player_location() == LOCATION_TITLE;
    for (struct BingoGoalMetric** it2 = gBingoMetrics; *it2 != NULL; it2++) {
        struct BingoGoalMetric* it = (*it2);
        if (!it->active) {
            continue;
        }

        print_generic_string(right + 5, top + 30 + 16 * metrics_index, it->mappedLabel);

        s16 count = onTitle ? 0 : it->currentCount();

        s16 strWidth = 0;
        int_to_str_4digit(count, buffer);
        strWidth += get_string_width(buffer);
        if (it->maximum != NULL) {
            strWidth += get_string_width(METRIC_DIVIDER);
            int_to_str(it->maximum, buffer);
            strWidth += get_string_width(buffer);
            int_to_str_4digit(count, buffer);
        }

        s16 offset = 0;
        print_generic_string(right + 5 + 95 + offset - strWidth, top + 30 + 16 * metrics_index, buffer);

        if (it->maximum != NULL) {
            offset += get_string_width(buffer);
            print_generic_string(right + 5 + 95 + offset - strWidth, top + 30 + 16 * metrics_index, METRIC_DIVIDER);
            offset += get_string_width(METRIC_DIVIDER);
            int_to_str(it->maximum, buffer);
            print_generic_string(right + 5 + 95 + offset - strWidth, top + 30 + 16 * metrics_index, buffer);
        }

        metrics_index++;
    }

    for (int i = 0; i < BINGO_MAX_PLAYERS; i++) {
        if (gBingoPlayers[i].connected) {
            gDPSetEnvColor(gDisplayListHead++, (gBingoPlayers[i].color >> 16) & 0xFF, (gBingoPlayers[i].color >> 8) & 0xFF, gBingoPlayers[i].color & 0xFF, 255);
            print_generic_string(left - 100, top + 30 + 16 * i, gBingoPlayers[i].dialogName);
            print_generic_string(left - 40, top + 30 + 16 * i, bingoPlayerLocationDetails[gBingoPlayers[i].location].dialogName);
        }
    }
}

void render_bingo_board(void) {
    // TODO not the best place for this, and we never actually free
    init_bingo();

    if (bingoMsgTimer > 0) {
        bingoMsgTimer--;
        print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(20), 0, bingoMsgBuffer);
    }
    // print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(20), 0, BINGO_VERSION);
    // snprintf(bingoDebugBuffer, 256, "%d %d", gCurrSaveFileNum, gCurrCourseNum);
    // snprintf(bingoDebugBuffer, 256, "%x", gBingoBoardSeed);
    // print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(120), 0, bingoDebugBuffer);

    if (displayFullsizeBingoBoard) {
        render_fullsize_bingo_board();
    } else if (gHudDisplay.flags != HUD_DISPLAY_NONE) {
        render_min_bingo_board();
    }
}
