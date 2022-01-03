#include <PR/gbi.h>

#include "sm64.h"
#include "gfx_dimensions.h"
#include "game/game_init.h"
#include "game/ingame_menu.h"
#include "game/level_update.h"
#include "bingo.h"

u8 displayFullsizeBingoBoard = 0;

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
    
    // draw_flat_rect(0, 0, 20, 20, 255, 255, 255, 150);
    // draw_flat_rect(0, 220, 20, 20, 0, 0, 0, 150);

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

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
}

void render_min_bingo_board(void) {
    s16 cell_size = 7;
    s16 padding = 1;
    s16 left = GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(20);
    s16 bottom = 240-20;

    render_bingo_board_helper(left, bottom, cell_size, padding, 100);
}

void render_fullsize_bingo_board(void) {
    s16 cell_size = 34;
    s16 padding = 5;
    s16 left = SCREEN_WIDTH / 2 - 5 * cell_size / 2;
    s16 bottom = 240 - 20;
    render_bingo_board_helper(left, bottom, cell_size, padding, 200);
    print_generic_string(left, 20, "4 stars in BOB");
}

void render_bingo_board(void) {
    print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(20), 0, "BINGO 8");

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
