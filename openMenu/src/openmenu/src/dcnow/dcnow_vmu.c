#include "dcnow_vmu.h"
#include <string.h>
#include <stdio.h>

#ifdef _arch_dreamcast
#include <dc/maple/vmu.h>
#include <dc/maple.h>
#include <kos.h>
#include <crayon_savefile/peripheral.h>
#include <openmenu_lcd.h>
#endif

/* Track current display state */
static bool dcnow_vmu_active = false;

#ifdef _arch_dreamcast

/* VMU bitmap buffer for DC Now display (48x32 pixels = 192 bytes) */
static unsigned char dcnow_vmu_bitmap[192] __attribute__((aligned(16)));

/* Current frame of the refresh spinner animation (0-3) */
static int dcnow_vmu_refresh_frame = 0;

/* Simple 3x5 font for rendering text on VMU
 * Each character is 3 pixels wide, 5 pixels tall
 * Format: 5 bytes per char (5 rows, 3 pixels per row stored in lower 3 bits) */
static const unsigned char vmu_font_3x5[][5] = {
    {0x0, 0x0, 0x0, 0x0, 0x0},  /* Space (32) */
    {0x2, 0x2, 0x2, 0x0, 0x2},  /* ! */
    {0x5, 0x5, 0x0, 0x0, 0x0},  /* " */
    {0x5, 0x7, 0x5, 0x7, 0x5},  /* # */
    {0x2, 0x6, 0x2, 0x3, 0x2},  /* $ */
    {0x5, 0x1, 0x2, 0x4, 0x5},  /* % */
    {0x4, 0x4, 0x2, 0x5, 0x2},  /* & */
    {0x2, 0x2, 0x0, 0x0, 0x0},  /* ' */
    {0x1, 0x2, 0x2, 0x2, 0x1},  /* ( */
    {0x4, 0x2, 0x2, 0x2, 0x4},  /* ) */
    {0x0, 0x5, 0x2, 0x5, 0x0},  /* * */
    {0x0, 0x2, 0x7, 0x2, 0x0},  /* + */
    {0x0, 0x0, 0x0, 0x2, 0x4},  /* , */
    {0x0, 0x0, 0x7, 0x0, 0x0},  /* - */
    {0x0, 0x0, 0x0, 0x0, 0x2},  /* . */
    {0x0, 0x1, 0x2, 0x4, 0x0},  /* / */
    {0x7, 0x5, 0x5, 0x5, 0x7},  /* 0 (48) */
    {0x2, 0x6, 0x2, 0x2, 0x7},  /* 1 */
    {0x7, 0x1, 0x7, 0x4, 0x7},  /* 2 */
    {0x7, 0x1, 0x7, 0x1, 0x7},  /* 3 */
    {0x5, 0x5, 0x7, 0x1, 0x1},  /* 4 */
    {0x7, 0x4, 0x7, 0x1, 0x7},  /* 5 */
    {0x7, 0x4, 0x7, 0x5, 0x7},  /* 6 */
    {0x7, 0x1, 0x2, 0x2, 0x2},  /* 7 */
    {0x7, 0x5, 0x7, 0x5, 0x7},  /* 8 */
    {0x7, 0x5, 0x7, 0x1, 0x7},  /* 9 */
    {0x0, 0x2, 0x0, 0x2, 0x0},  /* : */
    {0x0, 0x2, 0x0, 0x2, 0x4},  /* ; */
    {0x1, 0x2, 0x4, 0x2, 0x1},  /* < */
    {0x0, 0x7, 0x0, 0x7, 0x0},  /* = */
    {0x4, 0x2, 0x1, 0x2, 0x4},  /* > */
    {0x7, 0x1, 0x2, 0x0, 0x2},  /* ? */
    {0x7, 0x5, 0x5, 0x4, 0x7},  /* @ (64) */
    {0x7, 0x5, 0x7, 0x5, 0x5},  /* A (65) */
    {0x6, 0x5, 0x6, 0x5, 0x6},  /* B */
    {0x7, 0x4, 0x4, 0x4, 0x7},  /* C */
    {0x6, 0x5, 0x5, 0x5, 0x6},  /* D */
    {0x7, 0x4, 0x7, 0x4, 0x7},  /* E */
    {0x7, 0x4, 0x7, 0x4, 0x4},  /* F */
    {0x7, 0x4, 0x5, 0x5, 0x7},  /* G */
    {0x5, 0x5, 0x7, 0x5, 0x5},  /* H */
    {0x7, 0x2, 0x2, 0x2, 0x7},  /* I */
    {0x1, 0x1, 0x1, 0x5, 0x7},  /* J */
    {0x5, 0x5, 0x6, 0x5, 0x5},  /* K */
    {0x4, 0x4, 0x4, 0x4, 0x7},  /* L */
    {0x5, 0x7, 0x7, 0x5, 0x5},  /* M */
    {0x5, 0x7, 0x7, 0x7, 0x5},  /* N */
    {0x7, 0x5, 0x5, 0x5, 0x7},  /* O */
    {0x7, 0x5, 0x7, 0x4, 0x4},  /* P */
    {0x7, 0x5, 0x5, 0x7, 0x3},  /* Q */
    {0x7, 0x5, 0x7, 0x5, 0x5},  /* R */
    {0x7, 0x4, 0x7, 0x1, 0x7},  /* S */
    {0x7, 0x2, 0x2, 0x2, 0x2},  /* T */
    {0x5, 0x5, 0x5, 0x5, 0x7},  /* U */
    {0x5, 0x5, 0x5, 0x5, 0x2},  /* V */
    {0x5, 0x5, 0x7, 0x7, 0x5},  /* W */
    {0x5, 0x5, 0x2, 0x5, 0x5},  /* X */
    {0x5, 0x5, 0x2, 0x2, 0x2},  /* Y */
    {0x7, 0x1, 0x2, 0x4, 0x7},  /* Z */
};

/* Set a pixel in the VMU bitmap */
static void vmu_set_pixel(int x, int y, int on) {
    if (x < 0 || x >= 48 || y < 0 || y >= 32) return;

    /* Flip both axes to correct 180-degree rotated display */
    x = 47 - x;
    y = 31 - y;

    int byte_index = (y * 48 + x) / 8;
    int bit_index = 7 - ((y * 48 + x) % 8);

    if (on) {
        dcnow_vmu_bitmap[byte_index] |= (1 << bit_index);
    } else {
        dcnow_vmu_bitmap[byte_index] &= ~(1 << bit_index);
    }
}

/* Draw a character at position (x, y) using the 3x5 font */
static void vmu_draw_char(int x, int y, char c) {
    /* Convert to uppercase if lowercase */
    if (c >= 'a' && c <= 'z') {
        c = c - 'a' + 'A';
    }

    /* Get font index */
    int font_index = c - 32;  /* Font starts at space (32) */
    if (font_index < 0 || font_index >= 91) {
        font_index = 0;  /* Default to space */
    }

    /* Draw the character pixel by pixel */
    for (int row = 0; row < 5; row++) {
        unsigned char row_data = vmu_font_3x5[font_index][row];
        for (int col = 0; col < 3; col++) {
            if (row_data & (1 << (2 - col))) {
                vmu_set_pixel(x + col, y + row, 1);
            }
        }
    }
}

/* Draw a string at position (x, y) */
static void vmu_draw_string(int x, int y, const char *str) {
    int cur_x = x;
    while (*str) {
        if (*str == '\n') {
            /* Newline */
            cur_x = x;
            y += 6;  /* 5 pixels tall + 1 pixel spacing */
        } else {
            vmu_draw_char(cur_x, y, *str);
            cur_x += 4;  /* 3 pixels wide + 1 pixel spacing */
        }
        str++;
    }
}

/* Draw the current spinner frame into a 3x5 pixel area at (x, y).
 * Patterns: 0=horizontal, 1=backslash, 2=vertical, 3=forward-slash */
static void vmu_draw_spinner(int x, int y) {
    switch (dcnow_vmu_refresh_frame) {
        case 0: /* — */
            vmu_set_pixel(x,     y + 2, 1);
            vmu_set_pixel(x + 1, y + 2, 1);
            vmu_set_pixel(x + 2, y + 2, 1);
            break;
        case 1: /* \ */
            vmu_set_pixel(x,     y,     1);
            vmu_set_pixel(x + 1, y + 2, 1);
            vmu_set_pixel(x + 2, y + 4, 1);
            break;
        case 2: /* | */
            vmu_set_pixel(x + 1, y,     1);
            vmu_set_pixel(x + 1, y + 1, 1);
            vmu_set_pixel(x + 1, y + 2, 1);
            vmu_set_pixel(x + 1, y + 3, 1);
            vmu_set_pixel(x + 1, y + 4, 1);
            break;
        case 3: /* / */
            vmu_set_pixel(x,     y + 4, 1);
            vmu_set_pixel(x + 1, y + 2, 1);
            vmu_set_pixel(x + 2, y,     1);
            break;
    }
}

/* Overlay the refresh spinner onto the current bitmap and push to VMU */
static void vmu_overlay_refresh_indicator(void) {
    if (!dcnow_vmu_active) {
        /* Nothing on VMU yet — render a base placeholder */
        memset(dcnow_vmu_bitmap, 0, sizeof(dcnow_vmu_bitmap));
        vmu_draw_string(2, 1, "DCNOW");
        vmu_draw_string(2, 7, "FETCHING");
    }
    /* else: bitmap still holds the last game-list frame; leave it intact */

    /* Clear the 3x5 spinner cell next to the DCNOW title (x=24, y=1) */
    for (int dy = 0; dy < 5; dy++) {
        for (int dx = 0; dx < 3; dx++) {
            vmu_set_pixel(24 + dx, 1 + dy, 0);
        }
    }

    /* Draw current spinner frame, then advance */
    vmu_draw_spinner(24, 1);
    dcnow_vmu_refresh_frame = (dcnow_vmu_refresh_frame + 1) % 4;

    /* Push to hardware */
    uint8_t vmu_screens = crayon_peripheral_dreamcast_get_screens();
    crayon_peripheral_vmu_display_icon(vmu_screens, dcnow_vmu_bitmap);
    dcnow_vmu_active = true;
}

/* Render DC Now games list to VMU bitmap */
static void vmu_render_games_list(const dcnow_data_t *data) {
    /* Clear the bitmap */
    memset(dcnow_vmu_bitmap, 0, sizeof(dcnow_vmu_bitmap));

    char line[16];
    int y = 1;  /* Start 1 pixel from top */

    /* Line 1: "DCNOW" */
    vmu_draw_string(2, y, "DCNOW");
    y += 6;

    /* Line 2: Total players (e.g., "TOT:15") */
    snprintf(line, sizeof(line), "TOT:%d", data->total_players);
    vmu_draw_string(2, y, line);
    y += 6;

    /* Lines 3-5: Show up to 3 games with player counts */
    int max_games = (data->game_count < 3) ? data->game_count : 3;
    for (int i = 0; i < max_games; i++) {
        /* Use game code if available, otherwise truncate game name */
        const char *name = (data->games[i].game_code[0] != '\0') ?
                           data->games[i].game_code : data->games[i].game_name;

        /* Format: "NAME:##" - truncate to fit (48 pixels / 4 pixels per char = 12 chars max) */
        snprintf(line, sizeof(line), "%.7s:%d", name, data->games[i].player_count);
        vmu_draw_string(2, y, line);
        y += 6;

        if (y > 26) break;  /* Don't overflow the 32-pixel height */
    }
}

/* Update VMU with DC Now data - show games list */
static void vmu_update_with_games(const dcnow_data_t *data) {
    /* Render the games list to our bitmap buffer */
    vmu_render_games_list(data);

    /* Display on all VMUs */
    uint8_t vmu_screens = crayon_peripheral_dreamcast_get_screens();
    crayon_peripheral_vmu_display_icon(vmu_screens, dcnow_vmu_bitmap);

    printf("DC Now VMU: Display updated with games list (%d games, %d total players)\n",
           data->game_count, data->total_players);
}

/* Restore OpenMenu logo to all VMUs */
static void vmu_restore_openmenu_logo(void) {
    uint8_t vmu_screens = crayon_peripheral_dreamcast_get_screens();
    crayon_peripheral_vmu_display_icon(vmu_screens, openmenu_lcd);
}

#endif /* _arch_dreamcast */

void dcnow_vmu_update_display(const dcnow_data_t *data) {
#ifdef _arch_dreamcast
    if (!data || !data->data_valid) {
        /* No valid data, restore logo */
        dcnow_vmu_restore_logo();
        return;
    }

    /* Update VMU with games list */
    vmu_update_with_games(data);
    dcnow_vmu_active = true;

    printf("DC Now VMU: Updated display with %d games\n", data->game_count);
#else
    (void)data;  /* Unused on non-Dreamcast */
#endif
}

void dcnow_vmu_restore_logo(void) {
#ifdef _arch_dreamcast
    if (!dcnow_vmu_active) {
        /* Already showing logo, nothing to do */
        return;
    }

    vmu_restore_openmenu_logo();
    dcnow_vmu_active = false;

    printf("DC Now VMU: Restored OpenMenu logo\n");
#endif
}

bool dcnow_vmu_is_active(void) {
    return dcnow_vmu_active;
}

void dcnow_vmu_show_refreshing(void) {
#ifdef _arch_dreamcast
    vmu_overlay_refresh_indicator();
#endif
}
