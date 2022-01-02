#include <PR/gbi.h>

#include "sm64.h"
#include "gfx_dimensions.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "bingo.h"

u8 displayFullsizeBingoBoard = 0;

// this is duplicated from ingame menu
#define MENU_MTX_PUSH   1
#define MENU_MTX_NOPUSH 2
void create_dl_translation_matrix(s8 pushOp, f32 x, f32 y, f32 z) {
    Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));

    if (matrix == NULL) {
        return;
    }

    guTranslate(matrix, x, y, z);

    if (pushOp == MENU_MTX_PUSH)
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);

    if (pushOp == MENU_MTX_NOPUSH)
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
}

void draw_flat_rect(s16 x, s16 y, s16 w, s16 h, u8 r, u8 g, u8 b, u8 a) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, r, g, b, a);
    gSPTextureRectangle(gDisplayListHead++, x, y, x + w, y + h, 0, 0, 0, 0, 0);
}

void render_min_bingo_board(void) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetCombineMode(gDisplayListHead++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    create_dl_translation_matrix_AGAIN(MENU_MTX_PUSH, GFX_DIMENSIONS_FROM_LEFT_EDGE(0), 240.0f, 0);

    s16 cell_size = 7;
    s16 padding = 1;
    s16 left = GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(20);
    s16 bottom = 240-20;
    s16 top = bottom - 5 * cell_size - 6 * padding;
    s16 right = left + 5 * cell_size + 6 * padding;

    // TODO be less lazy and draw each part individually
    draw_flat_rect(left, top, right - left, bottom - top, 40, 40, 40, 1);

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
        draw_flat_rect(left + padding + (cell_size + padding) * x, top + padding + (cell_size + padding) * y, cell_size, cell_size, r, g, b, 200);
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_fullsize_bingo_board(void) {
    // TODO
    s16 cell_size = 40;
    s16 left = SCREEN_WIDTH / 2 - 5 * cell_size / 2;
    s16 bottom = 240 - 20;
    s16 top = bottom - 5 * cell_size;
    // TODO
}

void render_bingo_board(void) {
    print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(20), 0, "BINGO 1");

    if (displayFullsizeBingoBoard) {
        render_fullsize_bingo_board();
    } else if (gHudDisplay.flags != HUD_DISPLAY_NONE) {
        render_min_bingo_board();
    }
}

void handle_bingo_input(void) {
    if (gPlayer3Controller->buttonDown & L_TRIG) {
        displayFullsizeBingoBoard = 1;
    } else {
        displayFullsizeBingoBoard = 0;
    }
}
