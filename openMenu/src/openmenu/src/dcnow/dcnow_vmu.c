#include "dcnow_vmu.h"
#include <string.h>
#include <stdio.h>
#include <openmenu_settings.h>

#ifdef _arch_dreamcast
#include <dc/maple/vmu.h>
#include <dc/maple.h>
#include <kos.h>
#include <arch/timer.h>
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

/* High-density 5x7 font for rendering text on VMU
 * Each character is 5 pixels wide, 7 pixels tall
 * Format: 5 bytes per char (5 columns), Bit 0 is top pixel, Bit 6 is bottom */
static const uint8_t font_5x7_data[][5] = {
    /* ' ' */ {0x00, 0x00, 0x00, 0x00, 0x00},
    /* '0' */ {0x3E, 0x51, 0x49, 0x45, 0x3E},
    /* '1' */ {0x00, 0x42, 0x7F, 0x40, 0x00},
    /* '2' */ {0x42, 0x61, 0x51, 0x49, 0x46},
    /* '3' */ {0x21, 0x41, 0x45, 0x4B, 0x31},
    /* '4' */ {0x18, 0x14, 0x12, 0x7F, 0x10},
    /* '5' */ {0x27, 0x45, 0x45, 0x45, 0x39},
    /* '6' */ {0x3C, 0x4A, 0x49, 0x49, 0x30},
    /* '7' */ {0x01, 0x71, 0x09, 0x05, 0x03},
    /* '8' */ {0x36, 0x49, 0x49, 0x49, 0x36},
    /* '9' */ {0x06, 0x49, 0x49, 0x29, 0x1E},
    /* ':' */ {0x00, 0x36, 0x36, 0x00, 0x00},
    /* 'A' */ {0x7E, 0x09, 0x09, 0x09, 0x7E},
    /* 'B' */ {0x7F, 0x49, 0x49, 0x49, 0x36},
    /* 'C' */ {0x3E, 0x41, 0x41, 0x41, 0x22},
    /* 'D' */ {0x7F, 0x41, 0x41, 0x22, 0x1C},
    /* 'E' */ {0x7F, 0x49, 0x49, 0x49, 0x41},
    /* 'F' */ {0x7F, 0x09, 0x09, 0x09, 0x01},
    /* 'G' */ {0x3E, 0x41, 0x49, 0x49, 0x7A},
    /* 'H' */ {0x7F, 0x08, 0x08, 0x08, 0x7F},
    /* 'I' */ {0x00, 0x41, 0x7F, 0x41, 0x00},
    /* 'J' */ {0x20, 0x40, 0x41, 0x3F, 0x01},
    /* 'K' */ {0x7F, 0x08, 0x14, 0x22, 0x41},
    /* 'L' */ {0x7F, 0x40, 0x40, 0x40, 0x40},
    /* 'M' */ {0x7F, 0x02, 0x0C, 0x02, 0x7F},
    /* 'N' */ {0x7F, 0x04, 0x08, 0x10, 0x7F},
    /* 'O' */ {0x3E, 0x41, 0x41, 0x41, 0x3E},
    /* 'P' */ {0x7F, 0x09, 0x09, 0x09, 0x06},
    /* 'Q' */ {0x3E, 0x41, 0x51, 0x21, 0x5E},
    /* 'R' */ {0x7F, 0x09, 0x19, 0x29, 0x46},
    /* 'S' */ {0x46, 0x49, 0x49, 0x49, 0x31},
    /* 'T' */ {0x01, 0x01, 0x7F, 0x01, 0x01},
    /* 'U' */ {0x3F, 0x40, 0x40, 0x40, 0x3F},
    /* 'V' */ {0x1F, 0x20, 0x40, 0x20, 0x1F},
    /* 'W' */ {0x3F, 0x40, 0x38, 0x40, 0x3F},
    /* 'X' */ {0x63, 0x14, 0x08, 0x14, 0x63},
    /* 'Y' */ {0x07, 0x08, 0x70, 0x08, 0x07},
    /* 'Z' */ {0x61, 0x51, 0x49, 0x45, 0x43},
};

/* Font index lookup table: maps ASCII to font_5x7_data index */
static int font_5x7_index(char c) {
    if (c == ' ') return 0;
    if (c >= '0' && c <= '9') return 1 + (c - '0');
    if (c == ':') return 11;
    if (c >= 'A' && c <= 'Z') return 12 + (c - 'A');
    if (c >= 'a' && c <= 'z') return 12 + (c - 'a');  /* Map lowercase to uppercase */
    return 0;  /* Default to space */
}

/* Screen layout constants */
#define VMU_WIDTH       48
#define VMU_HEIGHT      32
#define HEADER_HEIGHT   8       /* Status bar Y: 0-7 */
#define SEPARATOR_Y     8       /* 1px separator line */
#define VIEWPORT_TOP    9       /* Viewport starts at Y=9 */
#define VIEWPORT_HEIGHT 23      /* Viewport Y: 9-31 (23 pixels) */
#define ROW_HEIGHT      8       /* Each game entry is 8 pixels tall */
#define CHAR_WIDTH      6       /* 5px char + 1px spacing */
#define CHAR_HEIGHT     7       /* 5x7 font height */

/* Scroll state */
static int scroll_offset = 0;           /* Current scroll position in pixels */
static int scroll_frame_counter = 0;    /* Frame counter for scroll timing */
static int cached_game_count = 0;       /* Cached number of games for scroll calculation */
static int cached_total_players = 0;    /* Cached total players for header */

/* Cached game data for scrolling display */
#define MAX_CACHED_GAMES 32
static char cached_game_names[MAX_CACHED_GAMES][8];  /* Short game names */
static int cached_player_counts[MAX_CACHED_GAMES];   /* Player counts per game */

/* Timestamp for "last updated" display */
static uint64_t last_update_time_ms = 0;  /* Time of last data update in milliseconds */

/* Set a pixel in the VMU bitmap (raw, no clipping) */
static void vmu_set_pixel_raw(int x, int y, int on) {
    if (x < 0 || x >= VMU_WIDTH || y < 0 || y >= VMU_HEIGHT) return;

    /* Flip both axes to correct 180-degree rotated display */
    x = (VMU_WIDTH - 1) - x;
    y = (VMU_HEIGHT - 1) - y;

    int byte_index = (y * VMU_WIDTH + x) / 8;
    int bit_index = 7 - ((y * VMU_WIDTH + x) % 8);

    if (on) {
        dcnow_vmu_bitmap[byte_index] |= (1 << bit_index);
    } else {
        dcnow_vmu_bitmap[byte_index] &= ~(1 << bit_index);
    }
}

/* Tiny 3x5 font for time indicator (fits in header corner)
 * Each character is 3 pixels wide, 5 pixels tall
 * Format: 5 bytes per char (5 rows), lower 3 bits = pixels */
static const uint8_t font_3x5_data[][5] = {
    /* '0' */ {0x7, 0x5, 0x5, 0x5, 0x7},
    /* '1' */ {0x2, 0x6, 0x2, 0x2, 0x7},
    /* '2' */ {0x7, 0x1, 0x7, 0x4, 0x7},
    /* '3' */ {0x7, 0x1, 0x7, 0x1, 0x7},
    /* '4' */ {0x5, 0x5, 0x7, 0x1, 0x1},
    /* '5' */ {0x7, 0x4, 0x7, 0x1, 0x7},
    /* '6' */ {0x7, 0x4, 0x7, 0x5, 0x7},
    /* '7' */ {0x7, 0x1, 0x2, 0x2, 0x2},
    /* '8' */ {0x7, 0x5, 0x7, 0x5, 0x7},
    /* '9' */ {0x7, 0x5, 0x7, 0x1, 0x7},
    /* 'S' */ {0x7, 0x4, 0x7, 0x1, 0x7},
    /* '+' */ {0x0, 0x2, 0x7, 0x2, 0x0},  /* For overflow indicator */
};

/* Draw a tiny 3x5 character at position (x, y)
 * color: 0 = white pixel (on black header), 1 = black pixel */
static void vmu_draw_char_3x5(int x, int y, char c, int color) {
    int idx = -1;
    if (c >= '0' && c <= '9') idx = c - '0';
    else if (c == 'S' || c == 's') idx = 10;
    else if (c == '+') idx = 11;
    else return;  /* Unsupported character */

    for (int row = 0; row < 5; row++) {
        uint8_t row_data = font_3x5_data[idx][row];
        for (int col = 0; col < 3; col++) {
            if (row_data & (1 << (2 - col))) {
                int px = x + col;
                int py = y + row;
                if (py < HEADER_HEIGHT && px >= 0 && px < VMU_WIDTH) {
                    vmu_set_pixel_raw(px, py, color);
                }
            }
        }
    }
}

/* Draw time indicator string using 3x5 font */
static void vmu_draw_time_indicator(int x, int y, const char *str, int color) {
    int cur_x = x;
    while (*str) {
        vmu_draw_char_3x5(cur_x, y, *str, color);
        cur_x += 4;  /* 3 pixels wide + 1 pixel spacing */
        str++;
    }
}

/* Plot a pixel with viewport clipping (for scrolling content)
 * Clips pixels that would be in the header area (y < VIEWPORT_TOP) */
static void vmu_plot(int x, int y, int color) {
    /* Clipping: Do not render scrolling text if y < VIEWPORT_TOP (9) */
    if (y < VIEWPORT_TOP) return;
    vmu_set_pixel_raw(x, y, color);
}

/* Draw a character at position (x, y) using the 5x7 font
 * color: 1 = set pixel, 0 = clear pixel
 * use_clipping: if true, uses vmu_plot (clips at viewport); if false, uses raw */
static void vmu_draw_char_5x7(int x, int y, char c, int color, bool use_clipping) {
    int idx = font_5x7_index(c);

    /* Draw the character column by column (5 columns, 7 rows) */
    for (int col = 0; col < 5; col++) {
        uint8_t col_data = font_5x7_data[idx][col];
        for (int row = 0; row < 7; row++) {
            if (col_data & (1 << row)) {
                int px = x + col;
                int py = y + row;
                if (use_clipping) {
                    vmu_plot(px, py, color);
                } else {
                    vmu_set_pixel_raw(px, py, color);
                }
            }
        }
    }
}

/* Draw a string in the viewport area (with clipping) */
static void vmu_draw_string_viewport(int x, int y, const char *str, int color) {
    int cur_x = x;
    while (*str) {
        vmu_draw_char_5x7(cur_x, y, *str, color, true);
        cur_x += CHAR_WIDTH;  /* 5 pixels wide + 1 pixel spacing */
        str++;
    }
}

/* Draw a string in the header area (inverted: color=1 for white text on black bg) */
static void vmu_draw_string_header(int x, int y, const char *str, int color) {
    int cur_x = x;
    while (*str) {
        /* For header, we don't use clipping since header is drawn after viewport */
        int idx = font_5x7_index(*str);
        for (int col = 0; col < 5; col++) {
            uint8_t col_data = font_5x7_data[idx][col];
            for (int row = 0; row < 7; row++) {
                if (col_data & (1 << row)) {
                    int px = cur_x + col;
                    int py = y + row;
                    if (py < HEADER_HEIGHT && px >= 0 && px < VMU_WIDTH) {
                        vmu_set_pixel_raw(px, py, color);
                    }
                }
            }
        }
        cur_x += CHAR_WIDTH;
        str++;
    }
}

/* Draw the current spinner frame into a 5x5 pixel area at (x, y).
 * Patterns: 0=horizontal, 1=backslash, 2=vertical, 3=forward-slash
 * Used in header area (no clipping needed) */
static void vmu_draw_spinner(int x, int y) {
    switch (dcnow_vmu_refresh_frame) {
        case 0: /* — */
            vmu_set_pixel_raw(x,     y + 2, 0);  /* White pixel on black header */
            vmu_set_pixel_raw(x + 1, y + 2, 0);
            vmu_set_pixel_raw(x + 2, y + 2, 0);
            vmu_set_pixel_raw(x + 3, y + 2, 0);
            vmu_set_pixel_raw(x + 4, y + 2, 0);
            break;
        case 1: /* \ */
            vmu_set_pixel_raw(x,     y,     0);
            vmu_set_pixel_raw(x + 1, y + 1, 0);
            vmu_set_pixel_raw(x + 2, y + 2, 0);
            vmu_set_pixel_raw(x + 3, y + 3, 0);
            vmu_set_pixel_raw(x + 4, y + 4, 0);
            break;
        case 2: /* | */
            vmu_set_pixel_raw(x + 2, y,     0);
            vmu_set_pixel_raw(x + 2, y + 1, 0);
            vmu_set_pixel_raw(x + 2, y + 2, 0);
            vmu_set_pixel_raw(x + 2, y + 3, 0);
            vmu_set_pixel_raw(x + 2, y + 4, 0);
            break;
        case 3: /* / */
            vmu_set_pixel_raw(x + 4, y,     0);
            vmu_set_pixel_raw(x + 3, y + 1, 0);
            vmu_set_pixel_raw(x + 2, y + 2, 0);
            vmu_set_pixel_raw(x + 1, y + 3, 0);
            vmu_set_pixel_raw(x,     y + 4, 0);
            break;
    }
}

/* Draw the static header overlay (black bar with white text) */
static void vmu_draw_header(int total_players, bool show_spinner) {
    /* Step 1: Draw black rectangle for header (Y=0 to Y=7)
     * In VMU bitmap: 1 = black pixel, 0 = white pixel */
    for (int y = 0; y < HEADER_HEIGHT; y++) {
        for (int x = 0; x < VMU_WIDTH; x++) {
            vmu_set_pixel_raw(x, y, 1);  /* Black background */
        }
    }

    /* Step 2: Draw 1px black separator line at Y=8 */
    for (int x = 0; x < VMU_WIDTH; x++) {
        vmu_set_pixel_raw(x, SEPARATOR_Y, 1);  /* Black line */
    }

    /* Step 3: Draw white text "ONL: [Total]" on the black header
     * For inverted display: 0 = white pixel on black background */
    char header_text[16];
    snprintf(header_text, sizeof(header_text), "ONL:%d", total_players);
    vmu_draw_string_header(1, 0, header_text, 0);  /* White text */

    /* Step 4: Draw time indicator or spinner on the right side */
    if (show_spinner) {
        /* Draw spinner when refreshing (takes precedence over time) */
        vmu_draw_spinner(VMU_WIDTH - 7, 1);
    } else if (last_update_time_ms > 0) {
        /* Draw "XXs" time indicator showing seconds since last update
         * in 10-second increments (0s, 10s, 20s, 30s, 40s, 50s)
         * Position: right-aligned in header, using tiny 3x5 font
         * Vertically centered in header: (8 - 5) / 2 = 1.5 → y=1 */
        uint64_t now_ms = timer_ms_gettime64();
        uint64_t elapsed_ms = now_ms - last_update_time_ms;
        int seconds = (int)(elapsed_ms / 1000);
        int tens = (seconds / 10) * 10;  /* Round down to nearest 10 */

        char time_str[8];
        if (tens >= 90) {
            snprintf(time_str, sizeof(time_str), "+90");  /* Cap at 90s */
        } else {
            snprintf(time_str, sizeof(time_str), "%ds", tens);
        }

        /* Calculate position: right-align with 1px margin
         * Each 3x5 char is 4 pixels (3 + 1 spacing), last char no trailing space
         * "0s" = 2 chars = 7 pixels, "50s" = 3 chars = 11 pixels */
        int time_len = strlen(time_str);
        int time_width = (time_len * 4) - 1;  /* No trailing space on last char */
        int time_x = VMU_WIDTH - time_width - 1;  /* 1px right margin */

        vmu_draw_time_indicator(time_x, 1, time_str, 0);  /* White text (0 on black) */
    }
}

/* Draw the scrolling game list in the viewport area */
static void vmu_draw_scrolling_list(void) {
    if (cached_game_count == 0) return;

    int total_list_height = cached_game_count * ROW_HEIGHT;

    /* Draw each game entry with wrap-around scrolling */
    for (int i = 0; i < cached_game_count; i++) {
        /* Calculate Y position with scroll offset */
        int base_y = (i * ROW_HEIGHT) - (scroll_offset % total_list_height);

        /* Wrap logic: if entry scrolled above viewport, wrap to bottom */
        if (base_y < -ROW_HEIGHT) {
            base_y += total_list_height;
        }
        /* Also wrap entries that would be too far below */
        if (base_y >= VIEWPORT_HEIGHT) {
            base_y -= total_list_height;
        }

        /* Convert to screen coordinates (add viewport offset) */
        int screen_y = VIEWPORT_TOP + base_y;

        /* Format game entry: "NAME:##" */
        char entry[16];
        snprintf(entry, sizeof(entry), "%.5s:%d", cached_game_names[i], cached_player_counts[i]);

        /* Draw the entry (vmu_plot will clip at y < VIEWPORT_TOP) */
        vmu_draw_string_viewport(1, screen_y, entry, 1);  /* Black text */
    }
}

/* Render the complete VMU display frame */
static void vmu_render_frame(bool show_spinner) {
    /* Step 1: Clear the entire 192-byte buffer (white background) */
    memset(dcnow_vmu_bitmap, 0, sizeof(dcnow_vmu_bitmap));

    /* Step 2: Draw the scrolling game list in viewport (gets clipped at y<9) */
    vmu_draw_scrolling_list();

    /* Step 3: Draw the header overlay (overwrites top portion) */
    vmu_draw_header(cached_total_players, show_spinner);

    /* Push to hardware */
    uint8_t vmu_screens = crayon_peripheral_dreamcast_get_screens();
    crayon_peripheral_vmu_display_icon(vmu_screens, dcnow_vmu_bitmap);
}

/* Overlay the refresh spinner onto the current bitmap and push to VMU */
static void vmu_overlay_refresh_indicator(void) {
    if (!dcnow_vmu_active) {
        /* Nothing on VMU yet — set up placeholder data */
        cached_game_count = 0;
        cached_total_players = 0;
    }

    /* Render frame with spinner */
    vmu_render_frame(true);

    /* Advance spinner animation */
    dcnow_vmu_refresh_frame = (dcnow_vmu_refresh_frame + 1) % 4;

    dcnow_vmu_active = true;
}

/* Cache game data for scrolling display */
static void vmu_cache_game_data(const dcnow_data_t *data) {
    cached_game_count = (data->game_count < MAX_CACHED_GAMES) ? data->game_count : MAX_CACHED_GAMES;
    cached_total_players = data->total_players;

    for (int i = 0; i < cached_game_count; i++) {
        /* Use game code if available, otherwise truncate game name */
        const char *name = (data->games[i].game_code[0] != '\0') ?
                           data->games[i].game_code : data->games[i].game_name;

        /* Copy and truncate name to fit display */
        strncpy(cached_game_names[i], name, 7);
        cached_game_names[i][7] = '\0';

        cached_player_counts[i] = data->games[i].player_count;
    }

    /* Reset scroll position when new data arrives */
    scroll_offset = 0;
    scroll_frame_counter = 0;

    /* Update the "last updated" timestamp */
    last_update_time_ms = timer_ms_gettime64();
}

/* Render DC Now games list to VMU bitmap */
static void vmu_render_games_list(const dcnow_data_t *data) {
    /* Cache the game data for scrolling */
    vmu_cache_game_data(data);

    /* Render the frame without spinner */
    vmu_render_frame(false);
}

/* Update VMU with DC Now data - show games list */
static void vmu_update_with_games(const dcnow_data_t *data) {
    /* Render the games list to our bitmap buffer (also pushes to VMU) */
    vmu_render_games_list(data);

    printf("DC Now VMU: Display updated with games list (%d games, %d total players)\n",
           data->game_count, data->total_players);
}

/* Advance scroll by 1 pixel every 9 frames and re-render
 * Also updates the time indicator in the header */
static void vmu_tick_scroll(void) {
    scroll_frame_counter++;

    /* Update every 9 frames (slower scroll for better readability) */
    if (scroll_frame_counter >= 9) {
        scroll_frame_counter = 0;

        /* Only advance scroll if there are 3+ games (viewport fits ~2 rows)
         * Viewport is 23px tall, each row is 8px, so 2 games fit without scroll */
        if (cached_game_count >= 3) {
            scroll_offset++;

            /* Wrap scroll offset to prevent overflow */
            int total_list_height = cached_game_count * ROW_HEIGHT;
            if (scroll_offset >= total_list_height) {
                scroll_offset = 0;
            }
        }

        /* Re-render the frame (updates time indicator in header) */
        vmu_render_frame(false);
    }
}

/* Restore OpenMenu logo to all VMUs */
static void vmu_restore_openmenu_logo(void) {
    uint8_t vmu_screens = crayon_peripheral_dreamcast_get_screens();
    crayon_peripheral_vmu_display_icon(vmu_screens, openmenu_lcd);
}

#endif /* _arch_dreamcast */

void dcnow_vmu_update_display(const dcnow_data_t *data) {
#ifdef _arch_dreamcast
    /* Check if DC Now VMU display is disabled in settings */
    if (sf_dcnow_vmu[0] == DCNOW_VMU_OFF) {
        /* If currently active, restore logo */
        if (dcnow_vmu_active) {
            dcnow_vmu_restore_logo();
        }
        return;
    }

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
    /* Check if DC Now VMU display is disabled in settings */
    if (sf_dcnow_vmu[0] == DCNOW_VMU_OFF) {
        return;
    }
    vmu_overlay_refresh_indicator();
#endif
}

void dcnow_vmu_tick_scroll(void) {
#ifdef _arch_dreamcast
    /* Check if DC Now VMU display is disabled in settings */
    if (sf_dcnow_vmu[0] == DCNOW_VMU_OFF) {
        return;
    }
    if (!dcnow_vmu_active) return;
    vmu_tick_scroll();
#endif
}

void dcnow_vmu_reset_scroll(void) {
#ifdef _arch_dreamcast
    scroll_offset = 0;
    scroll_frame_counter = 0;
    if (dcnow_vmu_active && cached_game_count > 0) {
        vmu_render_frame(false);
    }
#endif
}

void dcnow_vmu_show_status(const char* status) {
#ifdef _arch_dreamcast
    /* Check if DC Now VMU display is disabled in settings */
    if (sf_dcnow_vmu[0] == DCNOW_VMU_OFF) {
        return;
    }

    if (!status) return;

    /* Clear the bitmap (white background) */
    memset(dcnow_vmu_bitmap, 0, sizeof(dcnow_vmu_bitmap));

    /* Draw header bar (black background) */
    for (int y = 0; y < HEADER_HEIGHT; y++) {
        for (int x = 0; x < VMU_WIDTH; x++) {
            vmu_set_pixel_raw(x, y, 1);  /* Black background */
        }
    }

    /* Draw separator line */
    for (int x = 0; x < VMU_WIDTH; x++) {
        vmu_set_pixel_raw(x, SEPARATOR_Y, 1);
    }

    /* Draw "DCNOW" title in header (white text on black) */
    vmu_draw_string_header(1, 0, "DCNOW", 0);

    /* Draw spinner in header */
    vmu_draw_spinner(VMU_WIDTH - 7, 1);
    dcnow_vmu_refresh_frame = (dcnow_vmu_refresh_frame + 1) % 4;

    /* Draw status message in viewport area (centered vertically) */
    /* Convert status to uppercase for display */
    char upper_status[16];
    int i;
    for (i = 0; i < 15 && status[i]; i++) {
        char c = status[i];
        if (c >= 'a' && c <= 'z') {
            upper_status[i] = c - 32;  /* Convert to uppercase */
        } else {
            upper_status[i] = c;
        }
    }
    upper_status[i] = '\0';

    /* Calculate centered position */
    int text_len = i;
    int text_width = text_len * CHAR_WIDTH;
    int x_pos = (VMU_WIDTH - text_width) / 2;
    if (x_pos < 0) x_pos = 0;

    /* Draw centered in viewport (Y=16 is roughly center of viewport) */
    int y_pos = VIEWPORT_TOP + (VIEWPORT_HEIGHT - CHAR_HEIGHT) / 2;

    /* Draw status text (black text on white background) */
    int cur_x = x_pos;
    for (int j = 0; upper_status[j]; j++) {
        vmu_draw_char_5x7(cur_x, y_pos, upper_status[j], 1, false);
        cur_x += CHAR_WIDTH;
    }

    /* Send to VMU */
    uint8_t vmu_screens = crayon_peripheral_dreamcast_get_screens();
    crayon_peripheral_vmu_display_icon(vmu_screens, dcnow_vmu_bitmap);

    dcnow_vmu_active = true;
#else
    (void)status;
#endif
}
