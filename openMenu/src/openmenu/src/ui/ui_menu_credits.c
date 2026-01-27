/*
 * File: ui_menu_credits.c
 * Project: ui
 * File Created: Monday, 12th July 2021 11:34:23 pm
 * Author: Hayden Kowalchuk
 * -----
 * Copyright (c) 2021 Hayden Kowalchuk, Hayden Kowalchuk
 * License: BSD 3-clause "New" or "Revised" License,
 * http://www.opensource.org/licenses/BSD-3-Clause
 */

#include <string.h>

#include <backend/db_item.h>
#include <backend/gd_item.h>
#include <backend/gd_list.h>
#include <backend/gdemu_sdk.h>
#include <crayon_savefile/savefile.h>
#include <openmenu_savefile.h>
#include <openmenu_settings.h>

#include "ui/draw_kos.h"
#include "ui/draw_prototypes.h"
#include "ui/font_prototypes.h"
#include "ui/ui_common.h"
#include "ui/dc/input.h"

#include "ui/ui_menu_credits.h"

/* External declaration for VM2/VMUPro/USB4Maple detection */
#include <dc/maple.h>
extern int vm2_device_count;
extern void vm2_rescan(void);

#pragma region Exit_Menu

/* Exit to BIOS menu option strings */
static const char* exit_option_text[] = {
    "Send Game ID + Mount disc + Exit to BIOS",
    "Send Game ID + Exit to BIOS",
    "Mount disc + Exit to BIOS",
    "Exit to BIOS",
    "Close"
};

static int exit_menu_choice = 0;
static int exit_menu_num_options = 0;
static int exit_menu_is_folder = 0;

/* Exit menu option indices - set dynamically based on context */
typedef enum EXIT_OPTION {
    EXIT_OPT_SENDID_MOUNT = 0,
    EXIT_OPT_SENDID_ONLY,
    EXIT_OPT_MOUNT_ONLY,
    EXIT_OPT_EXIT_ONLY,
    EXIT_OPT_CLOSE,
    EXIT_OPT_MAX
} EXIT_OPTION;

/* Dynamic option list for current context */
static EXIT_OPTION exit_options[EXIT_OPT_MAX];

/* Build exit options list based on context
 * - is_folder: true if a folder is selected (only Exit to BIOS + Close)
 * - has_vm2: true if VM2/VMUPro/USB4MAPLE is detected
 * - is_game: true if type != "other" (game, psx, etc.)
 * - Also checks if VMU Game ID transmission is enabled (not set to Off)
 */
static void
exit_menu_build_options(int is_folder, int has_vm2, int is_game) {
    exit_menu_num_options = 0;
    exit_menu_is_folder = is_folder;

    if (is_folder) {
        /* Folder selected: only Exit to BIOS and Close */
        exit_options[exit_menu_num_options++] = EXIT_OPT_EXIT_ONLY;
        exit_options[exit_menu_num_options++] = EXIT_OPT_CLOSE;
    } else if (has_vm2 && is_game && sf_vm2_send_all[0] != VM2_SEND_OFF) {
        /* VM2 detected + type != "other" + transmission enabled: all options */
        exit_options[exit_menu_num_options++] = EXIT_OPT_SENDID_MOUNT;
        exit_options[exit_menu_num_options++] = EXIT_OPT_SENDID_ONLY;
        exit_options[exit_menu_num_options++] = EXIT_OPT_MOUNT_ONLY;
        exit_options[exit_menu_num_options++] = EXIT_OPT_EXIT_ONLY;
        exit_options[exit_menu_num_options++] = EXIT_OPT_CLOSE;
    } else {
        /* No VM2 or type == "other": mount, exit, close */
        exit_options[exit_menu_num_options++] = EXIT_OPT_MOUNT_ONLY;
        exit_options[exit_menu_num_options++] = EXIT_OPT_EXIT_ONLY;
        exit_options[exit_menu_num_options++] = EXIT_OPT_CLOSE;
    }
}

#pragma endregion Exit_Menu

#pragma region CodeBreaker_Menu

/* CodeBreaker menu option strings */
static const char* cb_option_text[] = {
    "Launch selected disc with CodeBreaker",
    "Close"
};

static int cb_menu_choice = 0;
#define CB_MENU_NUM_OPTIONS 2

typedef enum CB_OPTION {
    CB_OPT_LAUNCH = 0,
    CB_OPT_CLOSE
} CB_OPTION;

#pragma endregion CodeBreaker_Menu

#pragma region Settings_Menu

static const char* menu_choice_text[] = {"Style", "Theme", "Aspect", "Beep", "Exit to 3D BIOS", "Sort", "Filter", "Multi-Disc", "Multi-Disc Grouping", "Artwork", "Display Index Numbers", "Disc Details", "Artwork", "Item Details", "Clock", "Marquee Speed", "VMU Game ID", "Boot Mode"};
static const char* theme_choice_text[] = {"LineDesc", "Grid3", "Scroll", "Folders"};
static const char* region_choice_text[] = {"NTSC-U", "NTSC-J", "PAL"};
static const char* region_choice_text_scroll[] = {"GDMENU"};
static const char* region_choice_text_folders[] = {"FoldersDefault"};
static const char* aspect_choice_text[] = {"4:3", "16:9"};
static const char* beep_choice_text[] = {"Off", "On"}; /* Hidden from UI but kept for array sizing */
static const char* bios_3d_choice_text[] = {"Off", "On"};
static const char* sort_choice_text[] = {"Alphabetical", "Name", "Region", "Genre", "SD Card Order"};
static const char* sort_choice_text_folders[] = {"Alphabetical", "SD Card Order"};
#define SORT_CHOICES_FOLDERS 2
static const char* filter_choice_text[] = {"All",      "Action",   "Racing",   "Simulation", "Sports",     "Lightgun",
                                           "Fighting", "Shooter",  "Survival", "Adventure",  "Platformer", "RPG",
                                           "Shmup",    "Strategy", "Puzzle",   "Arcade",     "Music"};
static const char* multidisc_choice_text[] = {"Show All", "Compact"};
static const char* multidisc_grouping_choice_text[] = {"Anywhere", "Same Folder Only"};
static const char* scroll_art_choice_text[] = {"Off", "On"};
static const char* scroll_index_choice_text[] = {"Off", "On"};
static const char* disc_details_choice_text[] = {"Show", "Hide"};
static const char* folders_art_choice_text[] = {"Off", "On"};
static const char* folders_item_details_choice_text[] = {"Off", "On"};
static const char* marquee_speed_choice_text[] = {"Slow", "Medium", "Fast"};
static const char* clock_choice_text[] = {"On (12-Hour)", "On (24-Hour)", "Off"};
static const char* vm2_send_all_choice_text[] = {"Send to All", "Send to First", "Off"};
static const char* boot_mode_choice_text[] = {"Full Boot", "License Only", "Animation Only", "Fast Boot"};
static const char* save_choice_text[] = {"Save", "Apply"};
static const char* credits_text[] = {"Credits"};

const char* custom_theme_text[10] = {0};
static theme_custom* custom_themes;
static theme_scroll* custom_scroll;
static int num_custom_themes;
int cb_multidisc = 0;
int start_cb = 0;
static int psx_launcher_choice = 0;  /* 0 = Bleem!, 1 = Bloom */
static const gd_item* cur_game_item = NULL;

#define MENU_OPTIONS  ((int)(sizeof(menu_choice_text) / sizeof(menu_choice_text)[0]))
#define MENU_CHOICES  (MENU_OPTIONS)
#define THEME_CHOICES (sizeof(theme_choice_text) / sizeof(theme_choice_text)[0])
static int REGION_CHOICES = (sizeof(region_choice_text) / sizeof(region_choice_text)[0]);
#define ASPECT_CHOICES     (sizeof(aspect_choice_text) / sizeof(aspect_choice_text)[0])
#define BEEP_CHOICES       (sizeof(beep_choice_text) / sizeof(beep_choice_text)[0]) /* Hidden from UI */
#define BIOS_3D_CHOICES    (sizeof(bios_3d_choice_text) / sizeof(bios_3d_choice_text)[0])
#define SORT_CHOICES       (sizeof(sort_choice_text) / sizeof(sort_choice_text)[0])
#define FILTER_CHOICES     (sizeof(filter_choice_text) / sizeof(filter_choice_text)[0])
#define MULTIDISC_CHOICES  (sizeof(multidisc_choice_text) / sizeof(multidisc_choice_text)[0])
#define MULTIDISC_GROUPING_CHOICES (sizeof(multidisc_grouping_choice_text) / sizeof(multidisc_grouping_choice_text)[0])
#define SCROLL_ART_CHOICES (sizeof(scroll_art_choice_text) / sizeof(scroll_art_choice_text)[0])
#define SCROLL_INDEX_CHOICES (sizeof(scroll_index_choice_text) / sizeof(scroll_index_choice_text)[0])
#define DISC_DETAILS_CHOICES (sizeof(disc_details_choice_text) / sizeof(disc_details_choice_text)[0])
#define FOLDERS_ART_CHOICES (sizeof(folders_art_choice_text) / sizeof(folders_art_choice_text)[0])
#define FOLDERS_ITEM_DETAILS_CHOICES (sizeof(folders_item_details_choice_text) / sizeof(folders_item_details_choice_text)[0])
#define MARQUEE_SPEED_CHOICES (sizeof(marquee_speed_choice_text) / sizeof(marquee_speed_choice_text)[0])
#define CLOCK_CHOICES (sizeof(clock_choice_text) / sizeof(clock_choice_text)[0])
#define VM2_SEND_ALL_CHOICES (sizeof(vm2_send_all_choice_text) / sizeof(vm2_send_all_choice_text)[0])
#define BOOT_MODE_CHOICES (sizeof(boot_mode_choice_text) / sizeof(boot_mode_choice_text)[0])

typedef enum MENU_CHOICE {
    CHOICE_START,
    CHOICE_THEME = CHOICE_START,
    CHOICE_REGION,
    CHOICE_ASPECT,
    CHOICE_BEEP,
    CHOICE_BIOS_3D,
    CHOICE_SORT,
    CHOICE_FILTER,
    CHOICE_MULTIDISC,
    CHOICE_MULTIDISC_GROUPING,
    CHOICE_SCROLL_ART,
    CHOICE_SCROLL_INDEX,
    CHOICE_DISC_DETAILS,
    CHOICE_FOLDERS_ART,
    CHOICE_FOLDERS_ITEM_DETAILS,
    CHOICE_CLOCK,
    CHOICE_MARQUEE_SPEED,
    CHOICE_VM2_SEND_ALL,
    CHOICE_BOOT_MODE,
    CHOICE_SAVE,
    CHOICE_DCNOW,
    CHOICE_CREDITS,
    CHOICE_END = CHOICE_CREDITS
} MENU_CHOICE;

#define INPUT_TIMEOUT (10)

static int choices[MENU_CHOICES + 1];
static int choices_max[MENU_CHOICES + 1] = {
    THEME_CHOICES,     3, ASPECT_CHOICES, BEEP_CHOICES, BIOS_3D_CHOICES, SORT_CHOICES, FILTER_CHOICES,
    MULTIDISC_CHOICES, MULTIDISC_GROUPING_CHOICES, SCROLL_ART_CHOICES, SCROLL_INDEX_CHOICES, DISC_DETAILS_CHOICES, FOLDERS_ART_CHOICES, FOLDERS_ITEM_DETAILS_CHOICES, CLOCK_CHOICES, MARQUEE_SPEED_CHOICES, VM2_SEND_ALL_CHOICES, BOOT_MODE_CHOICES, 2 /* Apply/Save */};
static const char** menu_choice_array[MENU_CHOICES] = {theme_choice_text,       region_choice_text,   aspect_choice_text,
                                                       beep_choice_text,        bios_3d_choice_text,  sort_choice_text,
                                                       filter_choice_text,      multidisc_choice_text, multidisc_grouping_choice_text,
                                                       scroll_art_choice_text,  scroll_index_choice_text, disc_details_choice_text,
                                                       folders_art_choice_text, folders_item_details_choice_text, clock_choice_text, marquee_speed_choice_text, vm2_send_all_choice_text,
                                                       boot_mode_choice_text};
static int current_choice = CHOICE_START;
static int* input_timeout_ptr = NULL;

#pragma endregion Settings_Menu

#pragma region Credits_Menu

typedef struct credit_pair {
    const char* contributor;
    const char* role;
} credit_pair;

static const credit_pair credits[] = {
    (credit_pair){"ateam", "Folders, Updates/Fixes"},
    (credit_pair){"megavolt85", "gdemu sdk, coder"},
    (credit_pair){"u/westhinksdifferent/", "UI Mockups"},
    (credit_pair){"FlorreW", "Metadata DB"},
    (credit_pair){"hasnopants", "Metadata DB"},
    (credit_pair){"Roareye", "Metadata DB"},
    (credit_pair){"sonik-br", "GDMENUCardManager"},
    (credit_pair){"protofall", "Crayon_VMU"},
    (credit_pair){"TheLegendOfXela", "Boxart (Customs)"},
    (credit_pair){"marky-b-1986", "Theming Ideas"},
    (credit_pair){"Various Testers", "Breaking Things"},
    (credit_pair){"Kofi Supporters", "Coffee+Hardware"},
    (credit_pair){"mrneo240", "Author"},
};
static const int num_credits = sizeof(credits) / sizeof(credit_pair);

#pragma endregion Credits_Menu

static enum draw_state* state_ptr = NULL;
static theme_color* stored_colors = NULL;
static uint32_t text_color;
static uint32_t highlight_color;
static uint32_t menu_bkg_color;
static uint32_t menu_bkg_border_color;
static uint32_t menu_title_color;

/* Build version string (compiled in from VERSION.TXT at build time) */
#ifndef OPENMENU_BUILD_VERSION
#define OPENMENU_BUILD_VERSION "Unknown"
#endif

void
set_cur_game_item(const gd_item* id) {
    cur_game_item = id;
}

const gd_item*
get_cur_game_item() {
    return cur_game_item;
}

static void
common_setup(enum draw_state* state, theme_color* _colors, int* timeout_ptr) {
    /* Ensure color themeing is consistent */
    text_color = _colors->menu_text_color;
    highlight_color = _colors->menu_highlight_color;
    menu_bkg_color = _colors->menu_bkg_color;
    menu_bkg_border_color = _colors->menu_bkg_border_color;

    /* So we can modify the shared state and input timeout */
    state_ptr = state;
    stored_colors = _colors;
    input_timeout_ptr = timeout_ptr;
    *input_timeout_ptr = (30 * 1) /* half a second */;
}

void
menu_setup(enum draw_state* state, theme_color* _colors, int* timeout_ptr, uint32_t title_color) {
    common_setup(state, _colors, timeout_ptr);
    menu_title_color = title_color;

    /* Rescan for VM2 devices (detect hot-swapped devices) */
    vm2_rescan();

    choices[CHOICE_THEME] = sf_ui[0];
    choices[CHOICE_REGION] = sf_region[0];
    choices[CHOICE_ASPECT] = sf_aspect[0];
    choices[CHOICE_SORT] = sf_sort[0];
    /* In Folders mode, clamp Sort to valid range (0-1) */
    if (sf_ui[0] == UI_FOLDERS && choices[CHOICE_SORT] >= SORT_CHOICES_FOLDERS) {
        choices[CHOICE_SORT] = 0;  /* Default to Alphabetical */
    }
    choices[CHOICE_FILTER] = sf_filter[0];
    choices[CHOICE_BEEP] = sf_beep[0]; /* Hidden from UI */
    choices[CHOICE_BIOS_3D] = sf_bios_3d[0];
    choices[CHOICE_MULTIDISC] = sf_multidisc[0];
    choices[CHOICE_MULTIDISC_GROUPING] = sf_multidisc_grouping[0];
    choices[CHOICE_SCROLL_ART] = sf_scroll_art[0];
    choices[CHOICE_SCROLL_INDEX] = sf_scroll_index[0];
    choices[CHOICE_DISC_DETAILS] = sf_disc_details[0];
    choices[CHOICE_FOLDERS_ART] = sf_folders_art[0];
    choices[CHOICE_FOLDERS_ITEM_DETAILS] = sf_folders_item_details[0];
    choices[CHOICE_MARQUEE_SPEED] = sf_marquee_speed[0];
    choices[CHOICE_CLOCK] = sf_clock[0];
    choices[CHOICE_VM2_SEND_ALL] = sf_vm2_send_all[0];
    choices[CHOICE_BOOT_MODE] = sf_boot_mode[0];

    if (choices[CHOICE_THEME] != UI_SCROLL && choices[CHOICE_THEME] != UI_FOLDERS) {
        menu_choice_array[CHOICE_REGION] = region_choice_text;
        REGION_CHOICES = (sizeof(region_choice_text) / sizeof(region_choice_text)[0]);
        choices_max[CHOICE_REGION] = REGION_CHOICES;
        /* Grab custom themes if we have them */
        custom_themes = theme_get_custom(&num_custom_themes);
        if (num_custom_themes > 0) {
            for (int i = 0; i < num_custom_themes; i++) {
                choices_max[CHOICE_REGION]++;
                custom_theme_text[i] = custom_themes[i].name;
            }
        }
    } else {
        /* Assign appropriate default theme name based on UI mode */
        if (sf_ui[0] == UI_FOLDERS) {
            menu_choice_array[CHOICE_REGION] = region_choice_text_folders;
        } else {
            menu_choice_array[CHOICE_REGION] = region_choice_text_scroll;
        }
        REGION_CHOICES = 1;
        choices_max[CHOICE_REGION] = 1;
        /* Load appropriate themes based on UI mode */
        if (sf_ui[0] == UI_FOLDERS) {
            custom_scroll = theme_get_folder(&num_custom_themes);
        } else {
            custom_scroll = theme_get_scroll(&num_custom_themes);
        }
        if (num_custom_themes > 0) {
            for (int i = 0; i < num_custom_themes; i++) {
                choices_max[CHOICE_REGION]++;
                custom_theme_text[i] = custom_scroll[i].name;
            }
            if (sf_custom_theme[0] == THEME_ON) {
                choices[CHOICE_REGION] = sf_custom_theme_num[0] + 1;
            }
        }
    }

    if (choices[CHOICE_REGION] >= choices_max[CHOICE_REGION]) {
        choices[CHOICE_REGION] = choices_max[CHOICE_REGION] - 1;
    }
}

void
popup_setup(enum draw_state* state, theme_color* _colors, int* timeout_ptr, uint32_t title_color) {
    common_setup(state, _colors, timeout_ptr);
    menu_title_color = title_color;

    current_choice = CHOICE_START;
    psx_launcher_choice = 0;  /* Reset to Bleem! as default */
}

void
exit_menu_setup(enum draw_state* state, theme_color* _colors, int* timeout_ptr, uint32_t title_color, int is_folder) {
    common_setup(state, _colors, timeout_ptr);
    menu_title_color = title_color;

    /* Rescan for VM2 devices (detect hot-swapped devices) */
    vm2_rescan();

    /* Reset selection to first option */
    exit_menu_choice = 0;

    /* Determine if VM2 is present */
    int has_vm2 = (vm2_device_count > 0);

    /* Determine if current item is a game (type != "other") */
    int is_game = 0;
    if (!is_folder && cur_game_item != NULL && cur_game_item->type[0] != '\0') {
        is_game = (strcmp(cur_game_item->type, "other") != 0);
    }

    /* Build the options list */
    exit_menu_build_options(is_folder, has_vm2, is_game);
}

static void
menu_leave(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    *state_ptr = DRAW_UI;
    *input_timeout_ptr = (30 * 1) /* half a second */;
}

static void
credits_leave(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    *state_ptr = DRAW_MENU;
    *input_timeout_ptr = (20);
}

static void
menu_accept(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    if (current_choice == CHOICE_SAVE) {
        /* update Global Settings */
        sf_ui[0] = choices[CHOICE_THEME];
        sf_region[0] = choices[CHOICE_REGION];
        sf_aspect[0] = choices[CHOICE_ASPECT];
        sf_sort[0] = choices[CHOICE_SORT];
        sf_filter[0] = choices[CHOICE_FILTER];
        sf_beep[0] = choices[CHOICE_BEEP]; /* Hidden from UI */
        sf_bios_3d[0] = choices[CHOICE_BIOS_3D];
        sf_multidisc[0] = choices[CHOICE_MULTIDISC];
        sf_multidisc_grouping[0] = choices[CHOICE_MULTIDISC_GROUPING];
        sf_scroll_art[0] = choices[CHOICE_SCROLL_ART];
        sf_scroll_index[0] = choices[CHOICE_SCROLL_INDEX];
        sf_disc_details[0] = choices[CHOICE_DISC_DETAILS];
        sf_folders_art[0] = choices[CHOICE_FOLDERS_ART];
        sf_folders_item_details[0] = choices[CHOICE_FOLDERS_ITEM_DETAILS];
        sf_marquee_speed[0] = choices[CHOICE_MARQUEE_SPEED];
        sf_clock[0] = choices[CHOICE_CLOCK];
        sf_vm2_send_all[0] = choices[CHOICE_VM2_SEND_ALL];
        sf_boot_mode[0] = choices[CHOICE_BOOT_MODE];
        if (choices[CHOICE_THEME] != UI_SCROLL && choices[CHOICE_THEME] != UI_FOLDERS && sf_region[0] > REGION_END) {
            sf_custom_theme[0] = THEME_ON;
            int num_default_themes = 0;
            theme_get_default(sf_aspect[0], &num_default_themes);
            sf_custom_theme_num[0] = sf_region[0] - num_default_themes;
        } else if ((choices[CHOICE_THEME] == UI_SCROLL || choices[CHOICE_THEME] == UI_FOLDERS) && sf_region[0] > 0) {
            sf_custom_theme[0] = THEME_ON;
            sf_custom_theme_num[0] = sf_region[0] - 1;
        } else {
            sf_custom_theme[0] = THEME_OFF;
        }

        /* If not filtering, then plain sort */
        if (!choices[CHOICE_FILTER]) {
            switch ((CFG_SORT)choices[CHOICE_SORT]) {
                case SORT_NAME: list_set_sort_name(); break;
                case SORT_DATE: list_set_sort_region(); break;
                case SORT_PRODUCT: list_set_sort_genre(); break;
                case SORT_SD_CARD: list_set_sort_default(); break;
                default:
                case SORT_DEFAULT: list_set_sort_alphabetical(); break;
            }
        } else {
            /* If filtering, filter down to only genre then sort */
            list_set_genre_sort((FLAGS_GENRE)choices[CHOICE_FILTER] - 1, choices[CHOICE_SORT]);
        }

        if (choices[CHOICE_SAVE] == 0 /* Save */) {
            savefile_save();
        }
        extern void reload_ui(void);
        reload_ui();
    }
    if (current_choice == CHOICE_DCNOW) {
        /* Call dcnow_setup() to initialize the DC Now popup */
        dcnow_setup(state_ptr, stored_colors, input_timeout_ptr, menu_title_color);
    }
    if (current_choice == CHOICE_CREDITS) {
        *state_ptr = DRAW_CREDITS;
        *input_timeout_ptr = (20 * 1) /* 1/3 second */;
    }
}

static void
menu_choice_prev(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    current_choice--;
    /* Wrap around if we go below start */
    if (current_choice < CHOICE_START) {
        current_choice = CHOICE_END;
    }
    /* Keep skipping until we land on a valid option */
    int attempts = 0;
    while (attempts < CHOICE_END - CHOICE_START + 1) {
        int skip = 0;
        /* Skip SCROLL_ART option in non-Scroll modes */
        if (current_choice == CHOICE_SCROLL_ART && sf_ui[0] != UI_SCROLL) {
            skip = 1;
        }
        /* Skip SCROLL_INDEX option in non-Scroll modes */
        if (current_choice == CHOICE_SCROLL_INDEX && sf_ui[0] != UI_SCROLL) {
            skip = 1;
        }
        /* Skip DISC_DETAILS option in non-Scroll modes */
        if (current_choice == CHOICE_DISC_DETAILS && sf_ui[0] != UI_SCROLL) {
            skip = 1;
        }
        /* Skip MULTIDISC_GROUPING option in non-Folders modes or when Multi-Disc is "Show All" */
        if (current_choice == CHOICE_MULTIDISC_GROUPING && (sf_ui[0] != UI_FOLDERS || choices[CHOICE_MULTIDISC] == MULTIDISC_SHOW)) {
            skip = 1;
        }
        /* Skip FOLDERS_ART option in non-Folders modes */
        if (current_choice == CHOICE_FOLDERS_ART && sf_ui[0] != UI_FOLDERS) {
            skip = 1;
        }
        /* Skip FOLDERS_ITEM_DETAILS option in non-Folders modes */
        if (current_choice == CHOICE_FOLDERS_ITEM_DETAILS && sf_ui[0] != UI_FOLDERS) {
            skip = 1;
        }
        /* Skip MARQUEE_SPEED option in non-Scroll/Folders modes */
        if (current_choice == CHOICE_MARQUEE_SPEED && sf_ui[0] != UI_SCROLL && sf_ui[0] != UI_FOLDERS) {
            skip = 1;
        }
        /* Skip CLOCK option in non-Folders modes */
        if (current_choice == CHOICE_CLOCK && sf_ui[0] != UI_FOLDERS) {
            skip = 1;
        }
        /* Skip VM2_SEND_ALL option when no VM2 devices detected */
        if (current_choice == CHOICE_VM2_SEND_ALL && vm2_device_count == 0) {
            skip = 1;
        }
        /* Skip Aspect in Scroll mode (not used) */
        if (current_choice == CHOICE_ASPECT && sf_ui[0] == UI_SCROLL) {
            skip = 1;
        }
        /* Skip Aspect/Filter in Folders mode */
        if (sf_ui[0] == UI_FOLDERS && (current_choice == CHOICE_ASPECT || current_choice == CHOICE_FILTER)) {
            skip = 1;
        }
        /* Skip BEEP option (disabled/commented out) */
        if (current_choice == CHOICE_BEEP) {
            skip = 1;
        }
        /* Skip DCNOW in up/down navigation (reached via left/right from Save/Apply) */
        if (current_choice == CHOICE_DCNOW) {
            skip = 1;
        }
        /* Skip CREDITS in up/down navigation (reached via left/right from Save/Apply) */
        if (current_choice == CHOICE_CREDITS) {
            skip = 1;
        }
        if (!skip) {
            break;  /* Found a valid option */
        }
        current_choice--;
        if (current_choice < CHOICE_START) {
            current_choice = CHOICE_END;
        }
        attempts++;
    }
    *input_timeout_ptr = INPUT_TIMEOUT;
}

static void
menu_choice_next(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    current_choice++;
    /* Wrap around if we go past end */
    if (current_choice > CHOICE_END) {
        current_choice = CHOICE_START;
    }
    /* Keep skipping until we land on a valid option */
    int attempts = 0;
    while (attempts < CHOICE_END - CHOICE_START + 1) {
        int skip = 0;
        /* Skip SCROLL_ART option in non-Scroll modes */
        if (current_choice == CHOICE_SCROLL_ART && sf_ui[0] != UI_SCROLL) {
            skip = 1;
        }
        /* Skip SCROLL_INDEX option in non-Scroll modes */
        if (current_choice == CHOICE_SCROLL_INDEX && sf_ui[0] != UI_SCROLL) {
            skip = 1;
        }
        /* Skip DISC_DETAILS option in non-Scroll modes */
        if (current_choice == CHOICE_DISC_DETAILS && sf_ui[0] != UI_SCROLL) {
            skip = 1;
        }
        /* Skip MULTIDISC_GROUPING option in non-Folders modes or when Multi-Disc is "Show All" */
        if (current_choice == CHOICE_MULTIDISC_GROUPING && (sf_ui[0] != UI_FOLDERS || choices[CHOICE_MULTIDISC] == MULTIDISC_SHOW)) {
            skip = 1;
        }
        /* Skip FOLDERS_ART option in non-Folders modes */
        if (current_choice == CHOICE_FOLDERS_ART && sf_ui[0] != UI_FOLDERS) {
            skip = 1;
        }
        /* Skip FOLDERS_ITEM_DETAILS option in non-Folders modes */
        if (current_choice == CHOICE_FOLDERS_ITEM_DETAILS && sf_ui[0] != UI_FOLDERS) {
            skip = 1;
        }
        /* Skip MARQUEE_SPEED option in non-Scroll/Folders modes */
        if (current_choice == CHOICE_MARQUEE_SPEED && sf_ui[0] != UI_SCROLL && sf_ui[0] != UI_FOLDERS) {
            skip = 1;
        }
        /* Skip CLOCK option in non-Folders modes */
        if (current_choice == CHOICE_CLOCK && sf_ui[0] != UI_FOLDERS) {
            skip = 1;
        }
        /* Skip VM2_SEND_ALL option when no VM2 devices detected */
        if (current_choice == CHOICE_VM2_SEND_ALL && vm2_device_count == 0) {
            skip = 1;
        }
        /* Skip Aspect in Scroll mode (not used) */
        if (current_choice == CHOICE_ASPECT && sf_ui[0] == UI_SCROLL) {
            skip = 1;
        }
        /* Skip Aspect/Filter in Folders mode */
        if (sf_ui[0] == UI_FOLDERS && (current_choice == CHOICE_ASPECT || current_choice == CHOICE_FILTER)) {
            skip = 1;
        }
        /* Skip BEEP option (disabled/commented out) */
        if (current_choice == CHOICE_BEEP) {
            skip = 1;
        }
        /* Skip DCNOW in up/down navigation (reached via left/right from Save/Apply) */
        if (current_choice == CHOICE_DCNOW) {
            skip = 1;
        }
        /* Skip CREDITS in up/down navigation (reached via left/right from Save/Apply) */
        if (current_choice == CHOICE_CREDITS) {
            skip = 1;
        }
        if (!skip) {
            break;  /* Found a valid option */
        }
        current_choice++;
        if (current_choice > CHOICE_END) {
            current_choice = CHOICE_START;
        }
        attempts++;
    }
    *input_timeout_ptr = INPUT_TIMEOUT;
}

static void
menu_region_adj(void) {
    if (choices[CHOICE_THEME] != UI_SCROLL && choices[CHOICE_THEME] != UI_FOLDERS) {
        menu_choice_array[CHOICE_REGION] = region_choice_text;
        REGION_CHOICES = (sizeof(region_choice_text) / sizeof(region_choice_text)[0]);
        choices_max[CHOICE_REGION] = REGION_CHOICES;
        /* Grab custom themes if we have them */
        custom_themes = theme_get_custom(&num_custom_themes);
        if (num_custom_themes > 0) {
            for (int i = 0; i < num_custom_themes; i++) {
                choices_max[CHOICE_REGION]++;
                custom_theme_text[i] = custom_themes[i].name;
            }
        }
    } else {
        /* Assign appropriate default theme name based on current Style selection */
        if (choices[CHOICE_THEME] == UI_FOLDERS) {
            menu_choice_array[CHOICE_REGION] = region_choice_text_folders;
            REGION_CHOICES = (sizeof(region_choice_text_folders) / sizeof(region_choice_text_folders)[0]);
        } else {
            menu_choice_array[CHOICE_REGION] = region_choice_text_scroll;
            REGION_CHOICES = (sizeof(region_choice_text_scroll) / sizeof(region_choice_text_scroll)[0]);
        }
        choices_max[CHOICE_REGION] = REGION_CHOICES;
        /* Load appropriate themes based on UI mode */
        if (choices[CHOICE_THEME] == UI_FOLDERS) {
            custom_scroll = theme_get_folder(&num_custom_themes);
        } else {
            custom_scroll = theme_get_scroll(&num_custom_themes);
        }
        if (num_custom_themes > 0) {
            for (int i = 0; i < num_custom_themes; i++) {
                choices_max[CHOICE_REGION]++;
                custom_theme_text[i] = custom_scroll[i].name;
            }
        }
    }

    if (choices[CHOICE_REGION] >= choices_max[CHOICE_REGION]) {
        choices[CHOICE_REGION] = choices_max[CHOICE_REGION] - 1;
    }
}

static void
menu_choice_left(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    /* Handle Save/Apply/DC Now/Credits row navigation */
    if (current_choice == CHOICE_CREDITS) {
        /* Move left from Credits to DC Now */
        current_choice = CHOICE_DCNOW;
        *input_timeout_ptr = INPUT_TIMEOUT;
        return;
    }
    if (current_choice == CHOICE_DCNOW) {
        /* Move left from DC Now to Apply */
        current_choice = CHOICE_SAVE;
        choices[CHOICE_SAVE] = 1;  /* Select Apply */
        *input_timeout_ptr = INPUT_TIMEOUT;
        return;
    }
    if (current_choice == CHOICE_SAVE && choices[CHOICE_SAVE] == 0) {
        /* Already on Save (leftmost), do nothing */
        return;
    }
    choices[current_choice]--;
    if (choices[current_choice] < 0) {
        choices[current_choice] = 0;
    }
    if (current_choice == CHOICE_THEME) {
        menu_region_adj();
    }
    *input_timeout_ptr = INPUT_TIMEOUT;
}

static void
menu_choice_right(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    /* Handle Save/Apply/DC Now/Credits row navigation */
    if (current_choice == CHOICE_CREDITS) {
        /* Already on Credits (rightmost), do nothing */
        return;
    }
    if (current_choice == CHOICE_DCNOW) {
        /* Move right from DC Now to Credits */
        current_choice = CHOICE_CREDITS;
        *input_timeout_ptr = INPUT_TIMEOUT;
        return;
    }
    if (current_choice == CHOICE_SAVE && choices[CHOICE_SAVE] == 1) {
        /* On Apply, move right to DC Now */
        current_choice = CHOICE_DCNOW;
        *input_timeout_ptr = INPUT_TIMEOUT;
        return;
    }
    choices[current_choice]++;
    /* In Folders mode, limit Sort to 2 options */
    int max_choice = choices_max[current_choice];
    if (current_choice == CHOICE_SORT && sf_ui[0] == UI_FOLDERS) {
        max_choice = SORT_CHOICES_FOLDERS;
    }
    if (choices[current_choice] >= max_choice) {
        choices[current_choice]--;
    }
    if (current_choice == CHOICE_THEME) {
        menu_region_adj();
    }
    *input_timeout_ptr = INPUT_TIMEOUT;
}

static void
menu_multidisc_prev(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    current_choice--;
    if (current_choice < 0) {
        current_choice = 0;
    }
    *input_timeout_ptr = INPUT_TIMEOUT;
}

static void
menu_multidisc_next(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    current_choice++;
    int multidisc_len = list_multidisc_length();
    /* Allow one extra option for Close */
    if (current_choice > multidisc_len) {
        current_choice = multidisc_len;
    }
    *input_timeout_ptr = INPUT_TIMEOUT;
}

static void
menu_accept_multidisc(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    const gd_item** list_multidisc = list_get_multidisc();
    int multidisc_len = list_multidisc_length();

    /* Close option is at index multidisc_len */
    if (current_choice == multidisc_len) {
        menu_leave();
        return;
    }

    if (!cb_multidisc) {
        dreamcast_launch_disc(list_multidisc[current_choice]);
    } else {
        dreamcast_launch_cb(list_multidisc[current_choice]);
    }
}

static void
menu_exit_prev(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    exit_menu_choice--;
    if (exit_menu_choice < 0) {
        exit_menu_choice = 0;
    }
    *input_timeout_ptr = INPUT_TIMEOUT;
}

static void
menu_exit_next(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    exit_menu_choice++;
    if (exit_menu_choice >= exit_menu_num_options) {
        exit_menu_choice = exit_menu_num_options - 1;
    }
    *input_timeout_ptr = INPUT_TIMEOUT;
}

static void
menu_exit_accept(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }

    EXIT_OPTION selected = exit_options[exit_menu_choice];

    switch (selected) {
        case EXIT_OPT_CLOSE:
            /* Just close the popup */
            menu_leave();
            break;

        case EXIT_OPT_EXIT_ONLY:
            /* Exit to BIOS without mounting disc or sending ID */
            exit_to_bios_ex(0, 0);
            break;

        case EXIT_OPT_MOUNT_ONLY:
            /* Mount disc and exit to BIOS (no ID sending) */
            exit_to_bios_ex(1, 0);
            break;

        case EXIT_OPT_SENDID_ONLY:
            /* Send game ID and exit to BIOS (no disc mounting) */
            exit_to_bios_ex(0, 1);
            break;

        case EXIT_OPT_SENDID_MOUNT:
            /* Send game ID + mount disc + exit to BIOS */
            exit_to_bios_ex(1, 1);
            break;

        default:
            break;
    }
}

void
cb_menu_setup(enum draw_state* state, theme_color* _colors, int* timeout_ptr, uint32_t title_color) {
    common_setup(state, _colors, timeout_ptr);
    menu_title_color = title_color;

    /* Reset selection to first option (Launch) */
    cb_menu_choice = 0;
}

static void
menu_cb_prev(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    cb_menu_choice--;
    if (cb_menu_choice < 0) {
        cb_menu_choice = 0;
    }
    *input_timeout_ptr = INPUT_TIMEOUT;
}

static void
menu_cb_next(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    cb_menu_choice++;
    if (cb_menu_choice >= CB_MENU_NUM_OPTIONS) {
        cb_menu_choice = CB_MENU_NUM_OPTIONS - 1;
    }
    *input_timeout_ptr = INPUT_TIMEOUT;
}

static void
menu_cb_accept(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }

    CB_OPTION selected = (CB_OPTION)cb_menu_choice;

    switch (selected) {
        case CB_OPT_LAUNCH:
            start_cb = 1;
            break;

        case CB_OPT_CLOSE:
            menu_leave();
            break;

        default:
            break;
    }
}

void
handle_input_menu(enum control input) {
    switch (input) {
        case LEFT: menu_choice_left(); break;
        case RIGHT: menu_choice_right(); break;
        case UP: menu_choice_prev(); break;
        case DOWN: menu_choice_next(); break;
        case START:
        case B: menu_leave(); break;
        case A: menu_accept(); break;
        default: break;
    }
}

void
handle_input_credits(enum control input) {
    switch (input) {
        case A:
        case B:
        case START: credits_leave(); break;
        default: break;
    }
}

void
handle_input_multidisc(enum control input) {
    switch (input) {
        case UP: menu_multidisc_prev(); break;
        case DOWN: menu_multidisc_next(); break;
        case B: menu_leave(); break;
        case A: menu_accept_multidisc(); break;
        default: break;
    }
}

void
handle_input_exit(enum control input) {
    /* All modes use navigable menu */
    switch (input) {
        case UP: menu_exit_prev(); break;
        case DOWN: menu_exit_next(); break;
        case B: menu_leave(); break;
        case A: menu_exit_accept(); break;
        default: break;
    }
}

void
handle_input_codebreaker(enum control input) {
    switch (input) {
        case UP: menu_cb_prev(); break;
        case DOWN: menu_cb_next(); break;
        case B: menu_leave(); break;
        case A: menu_cb_accept(); break;
        default: break;
    }
}

void
draw_menu_op(void) { /* might be useless */ }

static void
string_outer_concat(char* out, const char* left, const char* right, int len) {
    const int input_len = strlen(left) + strlen(right);
    strcpy(out, left);
    for (int i = 0; i < len - input_len; i++) {
        strcat(out, " ");
    }
    strcat(out, right);
}

static void
draw_popup_menu(int x, int y, int width, int height) {
    const int border_width = 2;
    draw_draw_quad(x - border_width, y - border_width, width + (2 * border_width), height + (2 * border_width),
                   menu_bkg_border_color);
    draw_draw_quad(x, y, width, height, menu_bkg_color);

    if (sf_ui[0] == UI_SCROLL || sf_ui[0] == UI_FOLDERS) {
        /* Top header */
        draw_draw_quad(x, y, width, 20, menu_bkg_border_color);
    }
}

void
draw_menu_tr(void) {
    z_set_cond(205.0f);
    if (sf_ui[0] == UI_SCROLL || sf_ui[0] == UI_FOLDERS) {
        /* Menu size and placement */
        const int line_height = 24;
        const int width = 320;
        /* Calculate visible options for height.
         * Extra rows after options: Save/Apply/Credits, spacing, GDEMU version, Build version = 4 rows
         * The +4 in the height formula accounts for these rows */
        int visible_options = MENU_OPTIONS - 1;  /* Hide BEEP */
        if (sf_ui[0] == UI_SCROLL) {
            visible_options -= 4;  /* Hide Aspect, FOLDERS_ART, FOLDERS_ITEM_DETAILS, CLOCK, MULTIDISC_GROUPING (5 items, -1 for padding) */
        } else if (sf_ui[0] == UI_FOLDERS) {
            visible_options -= 4;  /* Hide Aspect, Filter, SCROLL_ART, SCROLL_INDEX, DISC_DETAILS (5 items, -1 for padding) */
            /* Dynamically hide MULTIDISC_GROUPING when Multi-Disc is "Show All" */
            if (choices[CHOICE_MULTIDISC] == MULTIDISC_SHOW) {
                visible_options -= 1;
            }
        }
        /* Dynamically hide VM2_SEND_ALL when no VM2 devices detected */
        if (vm2_device_count == 0) {
            visible_options -= 1;
        }
        const int height = (visible_options + 4) * line_height + (line_height * 11 / 12);
        const int x = (640 / 2) - (width / 2);
        const int y = (480 / 2) - (height / 2);
        const int x_item = x + 8;  /* 8px left margin */

        char line_buf[70];

        /* Draw a popup in the middle of the screen */
        draw_popup_menu(x, y, width, height);

        /* overlay our text on top with options */
        int cur_y = y + 2;
        font_bmp_begin_draw();
        font_bmp_set_color(menu_title_color);

        font_bmp_draw_main(width - (8 * 8 / 2), cur_y, "Settings");

        cur_y += line_height / 2;
        for (int i = 0; i < MENU_CHOICES; i++) {
            /* Skip SCROLL_ART option in non-Scroll modes */
            if (i == CHOICE_SCROLL_ART && sf_ui[0] != UI_SCROLL) {
                continue;
            }
            /* Skip SCROLL_INDEX option in non-Scroll modes */
            if (i == CHOICE_SCROLL_INDEX && sf_ui[0] != UI_SCROLL) {
                continue;
            }
            /* Skip DISC_DETAILS option in non-Scroll modes */
            if (i == CHOICE_DISC_DETAILS && sf_ui[0] != UI_SCROLL) {
                continue;
            }
            /* Skip MULTIDISC_GROUPING option in non-Folders modes or when Multi-Disc is "Show All" */
            if (i == CHOICE_MULTIDISC_GROUPING && (sf_ui[0] != UI_FOLDERS || choices[CHOICE_MULTIDISC] == MULTIDISC_SHOW)) {
                continue;
            }
            /* Skip FOLDERS_ART option in non-Folders modes */
            if (i == CHOICE_FOLDERS_ART && sf_ui[0] != UI_FOLDERS) {
                continue;
            }
            /* Skip FOLDERS_ITEM_DETAILS option in non-Folders modes */
            if (i == CHOICE_FOLDERS_ITEM_DETAILS && sf_ui[0] != UI_FOLDERS) {
                continue;
            }
            /* Skip MARQUEE_SPEED option in non-Scroll/Folders modes */
            if (i == CHOICE_MARQUEE_SPEED && sf_ui[0] != UI_SCROLL && sf_ui[0] != UI_FOLDERS) {
                continue;
            }
            /* Skip CLOCK option in non-Folders modes */
            if (i == CHOICE_CLOCK && sf_ui[0] != UI_FOLDERS) {
                continue;
            }
            /* Skip VM2_SEND_ALL option when no VM2 devices detected */
            if (i == CHOICE_VM2_SEND_ALL && vm2_device_count == 0) {
                continue;
            }
            /* Skip Aspect in Scroll mode (not used) */
            if (i == CHOICE_ASPECT && sf_ui[0] == UI_SCROLL) {
                continue;
            }
            /* Skip Aspect/Filter in Folders mode */
            if (sf_ui[0] == UI_FOLDERS && (i == CHOICE_ASPECT || i == CHOICE_FILTER)) {
                continue;
            }
            /* Skip BEEP option (disabled/commented out) */
            if (i == CHOICE_BEEP) {
                continue;
            }
            cur_y += line_height;
            if (i == current_choice) {
                font_bmp_set_color(highlight_color);
            } else {
                font_bmp_set_color(text_color);
            }
            if (i == CHOICE_REGION && (choices[i] >= REGION_CHOICES)) {
                string_outer_concat(line_buf, menu_choice_text[i], custom_theme_text[(int)choices[i] - REGION_CHOICES],
                                    38);
            } else if (i == CHOICE_SORT && sf_ui[0] == UI_FOLDERS) {
                /* In Folders mode, use Folders-specific sort text and clamp value */
                int sort_idx = choices[i] < SORT_CHOICES_FOLDERS ? choices[i] : 0;
                string_outer_concat(line_buf, menu_choice_text[i], sort_choice_text_folders[sort_idx], 38);
            } else {
                string_outer_concat(line_buf, menu_choice_text[i], menu_choice_array[i][(int)choices[i]], 38);
            }
            font_bmp_draw_main(x_item, cur_y, line_buf);
        }

        /* Draw Save/Apply/DC Now/Credits on one line */
        uint32_t save_color =
            ((current_choice == CHOICE_SAVE) && (choices[CHOICE_SAVE] == 0) ? highlight_color : text_color);
        uint32_t apply_color =
            ((current_choice == CHOICE_SAVE) && (choices[CHOICE_SAVE] == 1) ? highlight_color : text_color);
        uint32_t dcnow_color = (current_choice == CHOICE_DCNOW ? highlight_color : text_color);
        uint32_t credits_color = (current_choice == CHOICE_CREDITS ? highlight_color : text_color);
        cur_y += line_height;
        /* Save, Apply, DC Now, Credits across the bottom */
        font_bmp_set_color(save_color);
        font_bmp_draw_main(640 / 2 - (8 * 18), cur_y, save_choice_text[0]);
        font_bmp_set_color(apply_color);
        font_bmp_draw_main(640 / 2 - (8 * 7), cur_y, save_choice_text[1]);
        font_bmp_set_color(dcnow_color);
        font_bmp_draw_main(640 / 2 + (8 * 1), cur_y, "DC Now");
        font_bmp_set_color(credits_color);
        font_bmp_draw_main(640 / 2 + (8 * 11), cur_y, credits_text[0]);

        /* Add empty line for spacing */
        cur_y += line_height;

        /* Draw GDEMU firmware version (non-selectable) */
        uint8_t version_buffer[8] = {0};
        uint32_t version_size = 8;
        char version_str[40];
        if (gdemu_get_version(version_buffer, &version_size) == 0) {
            snprintf(version_str, sizeof(version_str), "GDEMU Firmware: %d.%02x.%d",
                     version_buffer[7], version_buffer[6], version_buffer[5]);
        } else {
            snprintf(version_str, sizeof(version_str), "GDEMU Firmware: N/A");
        }
        font_bmp_set_color(text_color);
        cur_y += line_height;
        /* Center based on actual string length (8 pixels per character) */
        int str_pixel_width = strlen(version_str) * 8;
        font_bmp_draw_main(640 / 2 - (str_pixel_width / 2), cur_y, version_str);

        /* Draw openMenu build version (non-selectable) */
        char build_str[80];
        snprintf(build_str, sizeof(build_str), "openMenu Build: %s", OPENMENU_BUILD_VERSION);
        cur_y += line_height;
        str_pixel_width = strlen(build_str) * 8;
        font_bmp_draw_main(640 / 2 - (str_pixel_width / 2), cur_y, build_str);

    } else {
        /* Menu size and placement (many options not shown in LineDesc/Grid3) */
        const int line_height = 32;
        const int width = 400;
        /* Exclude: BEEP, SCROLL_ART, SCROLL_INDEX, DISC_DETAILS, FOLDERS_ART, FOLDERS_ITEM_DETAILS, MARQUEE_SPEED, CLOCK, MULTIDISC_GROUPING (9 items, -1 for padding) */
        int visible_options = MENU_OPTIONS - 8;
        /* Dynamically hide VM2_SEND_ALL when no VM2 devices detected */
        if (vm2_device_count == 0) {
            visible_options -= 1;
        }
        const int height = (visible_options + 3) * line_height - line_height / 4 + line_height; /* Add space for version strings */
        const int x = (640 / 2) - (width / 2);
        const int y = (480 / 2) - (height / 2); /* Vertically centered */
        const int x_item = x + 4;
        const int x_choice = 344 + 24 + 20 + 25; /* magic :( */

        /* Draw a popup in the middle of the screen */
        draw_popup_menu(x, y, width, height);

        /* overlay our text on top with options */
        int cur_y = y + 2;
        font_bmf_begin_draw();
        font_bmf_set_height_default();

        font_bmf_draw(x_item, cur_y, text_color, "Settings");

        cur_y += line_height / 4;
        for (int i = 0; i < MENU_CHOICES; i++) {
            /* Skip SCROLL_ART option in non-Scroll modes */
            if (i == CHOICE_SCROLL_ART && sf_ui[0] != UI_SCROLL) {
                continue;
            }
            /* Skip SCROLL_INDEX option in non-Scroll modes */
            if (i == CHOICE_SCROLL_INDEX && sf_ui[0] != UI_SCROLL) {
                continue;
            }
            /* Skip DISC_DETAILS option in non-Scroll modes */
            if (i == CHOICE_DISC_DETAILS && sf_ui[0] != UI_SCROLL) {
                continue;
            }
            /* Skip FOLDERS_ART option in non-Folders modes */
            if (i == CHOICE_FOLDERS_ART && sf_ui[0] != UI_FOLDERS) {
                continue;
            }
            /* Skip FOLDERS_ITEM_DETAILS option in non-Folders modes */
            if (i == CHOICE_FOLDERS_ITEM_DETAILS && sf_ui[0] != UI_FOLDERS) {
                continue;
            }
            /* Skip MARQUEE_SPEED option in non-Scroll/Folders modes */
            if (i == CHOICE_MARQUEE_SPEED && sf_ui[0] != UI_SCROLL && sf_ui[0] != UI_FOLDERS) {
                continue;
            }
            /* Skip CLOCK option in non-Folders modes */
            if (i == CHOICE_CLOCK && sf_ui[0] != UI_FOLDERS) {
                continue;
            }
            /* Skip VM2_SEND_ALL option when no VM2 devices detected */
            if (i == CHOICE_VM2_SEND_ALL && vm2_device_count == 0) {
                continue;
            }
            /* Skip MULTIDISC_GROUPING option in non-Folders modes or when Multi-Disc is "Show All" */
            if (i == CHOICE_MULTIDISC_GROUPING && (sf_ui[0] != UI_FOLDERS || choices[CHOICE_MULTIDISC] == MULTIDISC_SHOW)) {
                continue;
            }
            /* Skip Aspect/Sort/Filter in Folders mode */
            if (sf_ui[0] == UI_FOLDERS && (i == CHOICE_ASPECT || i == CHOICE_SORT || i == CHOICE_FILTER)) {
                continue;
            }
            /* Skip BEEP option (disabled/commented out) */
            if (i == CHOICE_BEEP) {
                continue;
            }
            cur_y += line_height;
            uint32_t temp_color = text_color;
            if (i == current_choice) {
                temp_color = highlight_color;
            }
            font_bmf_draw(x_item, cur_y, temp_color, menu_choice_text[i]);

            if (i == CHOICE_REGION && (choices[i] >= REGION_CHOICES)) {
                font_bmf_draw_centered(x_choice, cur_y, temp_color,
                                       custom_theme_text[(int)choices[i] - REGION_CHOICES]);
            } else {
                font_bmf_draw_centered(x_choice, cur_y, temp_color, menu_choice_array[i][(int)choices[i]]);
            }
        }

        /* Draw Save/Apply/DC Now/Credits on one line */
        uint32_t save_color =
            ((current_choice == CHOICE_SAVE) && (choices[CHOICE_SAVE] == 0) ? highlight_color : text_color);
        uint32_t apply_color =
            ((current_choice == CHOICE_SAVE) && (choices[CHOICE_SAVE] == 1) ? highlight_color : text_color);
        uint32_t dcnow_color = ((current_choice == CHOICE_DCNOW) ? highlight_color : text_color);
        uint32_t credits_color = ((current_choice == CHOICE_CREDITS) ? highlight_color : text_color);
        cur_y += line_height;
        font_bmf_draw_centered(640 / 2 - (width / 2) + 50, cur_y, save_color, save_choice_text[0]);
        font_bmf_draw_centered(640 / 2 - (width / 6), cur_y, apply_color, save_choice_text[1]);
        font_bmf_draw_centered(640 / 2 + (width / 6), cur_y, dcnow_color, "DC Now");
        font_bmf_draw_centered(640 / 2 + (width / 2) - 50, cur_y, credits_color, credits_text[0]);

        /* Add empty line for spacing */
        cur_y += line_height;

        /* Draw GDEMU firmware version (non-selectable, smaller font) */
        uint8_t version_buffer[8] = {0};
        uint32_t version_size = 8;
        char version_str[40];
        if (gdemu_get_version(version_buffer, &version_size) == 0) {
            snprintf(version_str, sizeof(version_str), "GDEMU  Firmware:  %d.%02x.%d",
                     version_buffer[7], version_buffer[6], version_buffer[5]);
        } else {
            snprintf(version_str, sizeof(version_str), "GDEMU  Firmware:  N/A");
        }
        cur_y += line_height / 2;
        font_bmf_set_height(20.0f);
        font_bmf_draw_centered(640 / 2, cur_y, text_color, version_str);

        /* Draw openMenu build version (non-selectable, smaller font) */
        char build_str[80];
        snprintf(build_str, sizeof(build_str), "openMenu  Build :  %s", OPENMENU_BUILD_VERSION);
        cur_y += line_height * 3 / 4;
        font_bmf_draw_centered(640 / 2, cur_y, text_color, build_str);

        font_bmf_set_height_default();
    }
}

void
draw_credits_op(void) { /* Again nothing... */ }

void
draw_credits_tr(void) {
    z_set_cond(205.0f);

    if (sf_ui[0] == UI_SCROLL || sf_ui[0] == UI_FOLDERS) {
        /* Menu size and placement */
        const int line_height = 24;
        const int width = 320;
        const int height = (num_credits + 1) * line_height + (line_height * 13 / 12);
        const int x = (640 / 2) - (width / 2);
        const int y = (480 / 2) - (height / 2);
        const int x_item = x + 8;  /* 8px left margin */

        char line_buf[65];

        /* Draw a popup in the middle of the screen */
        draw_popup_menu(x, y, width, height);

        /* overlay our text on top with options */
        int cur_y = y + 4;
        font_bmp_begin_draw();
        font_bmp_set_color(sf_ui[0] == UI_FOLDERS ? menu_title_color : text_color);

        font_bmp_draw_main(width - (8 * 8 / 2), cur_y, "Credits");
        font_bmp_set_color(sf_ui[0] == UI_FOLDERS ? text_color : highlight_color);

        cur_y += line_height / 2;
        for (int i = 0; i < num_credits; i++) {
            cur_y += line_height;
            string_outer_concat(line_buf, credits[i].contributor, credits[i].role, 38);
            font_bmp_draw_main(x_item, cur_y, line_buf);
        }

    } else {
        /* Menu size and placement */
        const int line_height = 26;
        const int width = 560;
        const int height = (num_credits + 2) * line_height;
        const int x = (640 / 2) - (width / 2);
        const int y = (480 / 2) - (height / 2);
        const int x_choice = 344 + 24 + 60; /* magic :( */
        const int x_item = x + 4;

        /* Draw a popup in the middle of the screen */
        draw_popup_menu(x, y, width, height);

        /* overlay our text on top with options */
        int cur_y = y + 2;
        font_bmf_begin_draw();
        font_bmf_set_height(24.0f);

        font_bmf_draw(x_item, cur_y, text_color, "Credits");

        cur_y += line_height / 4;
        for (int i = 0; i < num_credits; i++) {
            cur_y += line_height;
            font_bmf_draw(x_item, cur_y, highlight_color, credits[i].contributor);
            font_bmf_draw_centered(x_choice, cur_y, highlight_color, credits[i].role);
        }
    }
}

void
draw_multidisc_op(void) { /* Again nothing...Still... */ }

void
draw_multidisc_tr(void) {
    const gd_item** list_multidisc = list_get_multidisc();
    int multidisc_len = list_multidisc_length();

    z_set_cond(205.0f);
    if (sf_ui[0] == UI_SCROLL || sf_ui[0] == UI_FOLDERS) {
        /* Menu size and placement - width auto-sized based on disc labels */
        const int line_height = 24;
        const int title_gap = line_height / 2;
        const int title_width = 10 * 8;  /* "Multi-Disc" = 10 chars */
        const int padding = 16;  /* 8px margin on each side */
        const int max_name_chars = 35;  /* Maximum characters for game name */
        char line_buf[48];
        char temp_game_name[36];

        /* Find the longest disc label to determine popup width */
        int max_label_len = 0;
        for (int i = 0; i < multidisc_len; i++) {
            int name_len = strlen(list_multidisc[i]->name);
            if (name_len > max_name_chars) {
                name_len = max_name_chars;
            }
            /* Account for " #N" or " #NN" suffix (space + # + 1-2 digits) */
            int disc_num = gd_item_disc_num(list_multidisc[i]->disc);
            int suffix_len = (disc_num >= 10) ? 4 : 3;
            int label_len = name_len + suffix_len;
            if (label_len > max_label_len) {
                max_label_len = label_len;
            }
        }

        /* Width is the larger of title or max label, plus padding */
        const int content_width = max_label_len * 8;
        const int width = (content_width > title_width ? content_width : title_width) + padding;
        const int height = (multidisc_len + 2) * line_height + (line_height / 2) + title_gap;
        const int x = (640 / 2) - (width / 2);
        const int y = (480 / 2) - (height / 2);
        const int x_item = x + (padding / 2);

        /* Draw a popup in the middle of the screen */
        draw_popup_menu(x, y, width, height);

        /* overlay our text on top with options */
        int cur_y = y + 2;
        font_bmp_begin_draw();
        font_bmp_set_color(sf_ui[0] == UI_FOLDERS ? menu_title_color : text_color);

        font_bmp_draw_main(x + width / 2 - (10 * 8 / 2), cur_y, "Multi-Disc");

        cur_y += title_gap;  /* Add spacing after title */
        for (int i = 0; i < multidisc_len; i++) {
            cur_y += line_height;
            if (i == current_choice) {
                font_bmp_set_color(highlight_color);
            } else {
                font_bmp_set_color(text_color);
            }
            const int disc_num = gd_item_disc_num(list_multidisc[i]->disc);
            strncpy(temp_game_name, list_multidisc[i]->name, sizeof(temp_game_name) - 1);
            temp_game_name[sizeof(temp_game_name) - 1] = '\0';
            /* Add ellipsis if name was truncated */
            if (strlen(list_multidisc[i]->name) >= sizeof(temp_game_name)) {
                strcpy(&temp_game_name[sizeof(temp_game_name) - 4], "...");
            }
            /* Format as "GameName #N" without fixed-width padding */
            snprintf(line_buf, sizeof(line_buf), "%s #%d", temp_game_name, disc_num);
            font_bmp_draw_main(x_item, cur_y, line_buf);
        }

        /* Close option */
        cur_y += line_height;
        font_bmp_set_color(current_choice == multidisc_len ? highlight_color : text_color);
        font_bmp_draw_main(x_item, cur_y, "Close");
    } else {
        /* Menu size and placement */
        const int line_height = 32;
        const int width = 300;
        const int height = (multidisc_len + 2) * line_height + (line_height / 2);
        const int x = (640 / 2) - (width / 2);
        const int y = (480 / 2) - (height / 2);
        const int x_item = x + 4;
        char line_buf[65];
        char temp_game_name[62];

        /* Draw a popup in the middle of the screen */
        draw_popup_menu(x, y, width, height);

        /* overlay our text on top with options */
        int cur_y = y + 2;
        font_bmf_begin_draw();
        font_bmf_set_height_default();

        font_bmf_draw_centered(x + width / 2, cur_y, text_color, "Multi-Disc");

        cur_y += line_height / 4;

        for (int i = 0; i < multidisc_len; i++) {
            cur_y += line_height;
            uint32_t temp_color = text_color;
            if (i == current_choice) {
                temp_color = highlight_color;
            }
            const int disc_num = gd_item_disc_num(list_multidisc[i]->disc);
            strncpy(temp_game_name, list_multidisc[i]->name, sizeof(temp_game_name) - 1);
            temp_game_name[sizeof(temp_game_name) - 1] = '\0';
            /* Add ellipsis if name was truncated */
            if (strlen(list_multidisc[i]->name) >= sizeof(temp_game_name)) {
                strcpy(&temp_game_name[sizeof(temp_game_name) - 4], "...");
            }
            snprintf(line_buf, 69, "%s #%d", temp_game_name, disc_num);
            font_bmf_draw_auto_size(x_item, cur_y, temp_color, line_buf, width - 4);
        }

        /* Close option */
        cur_y += line_height;
        font_bmf_draw(x_item, cur_y, current_choice == multidisc_len ? highlight_color : text_color, "Close");
    }
}

void
draw_exit_op(void) { /* Again nothing...Still...Ugh... */ }

void
draw_exit_tr(void) {
    z_set_cond(205.0f);

    if (sf_ui[0] == UI_SCROLL || sf_ui[0] == UI_FOLDERS) {
        /* Menu size and placement - width calculated based on actual options */
        const int line_height = 24;
        const int title_gap = line_height / 2;
        const int padding = 16;  /* 8px margin on each side */
        const int title_width = 12 * 8;  /* "Exit to BIOS" = 12 chars */

        /* Find the longest option text in the current menu */
        int max_option_len = 0;
        for (int i = 0; i < exit_menu_num_options; i++) {
            int len = strlen(exit_option_text[exit_options[i]]);
            if (len > max_option_len) {
                max_option_len = len;
            }
        }

        /* Width is the larger of title or max option, plus padding */
        const int content_width = max_option_len * 8;
        const int width = (content_width > title_width ? content_width : title_width) + padding;
        const int height = (exit_menu_num_options + 1) * line_height + (line_height / 2) + title_gap;
        const int x = (640 / 2) - (width / 2);
        const int y = (480 / 2) - (height / 2);
        const int x_item = x + (padding / 2);

        draw_popup_menu(x, y, width, height);

        int cur_y = y + 2;
        font_bmp_begin_draw();
        font_bmp_set_color(menu_title_color);

        font_bmp_draw_main(x + width / 2 - (12 * 8 / 2), cur_y, "Exit to BIOS");

        cur_y += title_gap;
        for (int i = 0; i < exit_menu_num_options; i++) {
            cur_y += line_height;
            if (i == exit_menu_choice) {
                font_bmp_set_color(highlight_color);
            } else {
                font_bmp_set_color(text_color);
            }
            font_bmp_draw_main(x_item, cur_y, exit_option_text[exit_options[i]]);
        }
    } else {
        /* LineDesc/Grid modes - dynamic menu with larger font */
        const int line_height = 32;
        const int title_gap = line_height / 4;
        const int padding = 20;

        /* Find the longest option text in the current menu */
        int max_option_len = 0;
        for (int i = 0; i < exit_menu_num_options; i++) {
            int len = strlen(exit_option_text[exit_options[i]]);
            if (len > max_option_len) {
                max_option_len = len;
            }
        }

        /* Estimate width based on font (roughly 10-12px per char for bmf font) */
        const int content_width = max_option_len * 10;
        const int title_width = 12 * 10;  /* "Exit to BIOS" */
        const int width = (content_width > title_width ? content_width : title_width) + padding;
        const int height = (exit_menu_num_options + 1) * line_height + (line_height / 2);
        const int x = (640 / 2) - (width / 2);
        const int y = (480 / 2) - (height / 2);
        const int x_item = x + 10;

        draw_popup_menu(x, y, width, height);

        int cur_y = y + 2;
        font_bmf_begin_draw();
        font_bmf_set_height_default();

        font_bmf_draw_centered(x + width / 2, cur_y, text_color, "Exit to BIOS");

        cur_y += title_gap;
        for (int i = 0; i < exit_menu_num_options; i++) {
            cur_y += line_height;
            uint32_t temp_color = text_color;
            if (i == exit_menu_choice) {
                temp_color = highlight_color;
            }
            font_bmf_draw_auto_size(x_item, cur_y, temp_color, exit_option_text[exit_options[i]], width - 20);
        }
    }
}

void
draw_codebreaker_op(void) { /* Again nothing...Still...Ugh... */ }

void
draw_codebreaker_tr(void) {
    z_set_cond(205.0f);

    if (sf_ui[0] == UI_SCROLL || sf_ui[0] == UI_FOLDERS) {
        /* Menu size and placement - width calculated based on actual options */
        const int line_height = 24;
        const int title_gap = line_height / 2;
        const int padding = 16;  /* 8px margin on each side */
        const int title_width = 10 * 8;  /* "Use Cheats" = 10 chars */

        /* Find the longest option text */
        int max_option_len = 0;
        for (int i = 0; i < CB_MENU_NUM_OPTIONS; i++) {
            int len = strlen(cb_option_text[i]);
            if (len > max_option_len) {
                max_option_len = len;
            }
        }

        /* Width is the larger of title or max option, plus padding */
        const int content_width = max_option_len * 8;
        const int width = (content_width > title_width ? content_width : title_width) + padding;
        const int height = (CB_MENU_NUM_OPTIONS + 1) * line_height + (line_height / 2) + title_gap;
        const int x = (640 / 2) - (width / 2);
        const int y = (480 / 2) - (height / 2);
        const int x_item = x + (padding / 2);

        draw_popup_menu(x, y, width, height);

        int cur_y = y + 2;
        font_bmp_begin_draw();
        font_bmp_set_color(menu_title_color);

        font_bmp_draw_main(x + width / 2 - (10 * 8 / 2), cur_y, "Use Cheats");

        cur_y += title_gap;
        for (int i = 0; i < CB_MENU_NUM_OPTIONS; i++) {
            cur_y += line_height;
            if (i == cb_menu_choice) {
                font_bmp_set_color(highlight_color);
            } else {
                font_bmp_set_color(text_color);
            }
            font_bmp_draw_main(x_item, cur_y, cb_option_text[i]);
        }
    } else {
        /* LineDesc/Grid modes - dynamic menu with larger font */
        const int line_height = 32;
        const int title_gap = line_height / 4;
        const int padding = 20;

        /* Find the longest option text */
        int max_option_len = 0;
        for (int i = 0; i < CB_MENU_NUM_OPTIONS; i++) {
            int len = strlen(cb_option_text[i]);
            if (len > max_option_len) {
                max_option_len = len;
            }
        }

        /* Estimate width based on font (roughly 10-12px per char for bmf font) */
        const int content_width = max_option_len * 10;
        const int title_width = 10 * 10;  /* "Use Cheats" */
        const int width = (content_width > title_width ? content_width : title_width) + padding;
        const int height = (CB_MENU_NUM_OPTIONS + 1) * line_height + (line_height / 2);
        const int x = (640 / 2) - (width / 2);
        const int y = (480 / 2) - (height / 2);
        const int x_item = x + 10;

        draw_popup_menu(x, y, width, height);

        int cur_y = y + 2;
        font_bmf_begin_draw();
        font_bmf_set_height_default();

        font_bmf_draw_centered(x + width / 2, cur_y, text_color, "Use Cheats");

        cur_y += title_gap;
        for (int i = 0; i < CB_MENU_NUM_OPTIONS; i++) {
            cur_y += line_height;
            uint32_t temp_color = text_color;
            if (i == cb_menu_choice) {
                temp_color = highlight_color;
            }
            font_bmf_draw_auto_size(x_item, cur_y, temp_color, cb_option_text[i], width - 20);
        }
    }
}

/* PSX Launcher popup functions */
static void
menu_psx_launcher_prev(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    psx_launcher_choice--;
    if (psx_launcher_choice < 0) {
        psx_launcher_choice = 0;
    }
    *input_timeout_ptr = INPUT_TIMEOUT;
}

static void
menu_psx_launcher_next(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    psx_launcher_choice++;
    if (psx_launcher_choice > 2) {
        psx_launcher_choice = 2;
    }
    *input_timeout_ptr = INPUT_TIMEOUT;
}

static void
menu_accept_psx_launcher(void) {
    if (*input_timeout_ptr > 0) {
        return;
    }
    if (psx_launcher_choice == 0) {
        bleem_launch(cur_game_item);
    } else if (psx_launcher_choice == 1) {
        bloom_launch(cur_game_item);
    } else {
        /* Close */
        menu_leave();
    }
}

void
handle_input_psx_launcher(enum control input) {
    switch (input) {
        case UP: menu_psx_launcher_prev(); break;
        case DOWN: menu_psx_launcher_next(); break;
        case B: menu_leave(); break;
        case A: menu_accept_psx_launcher(); break;
        default: break;
    }
}

void
draw_psx_launcher_op(void) { /* Nothing needed */ }

void
draw_psx_launcher_tr(void) {
    z_set_cond(205.0f);

    if (sf_ui[0] == UI_SCROLL || sf_ui[0] == UI_FOLDERS) {
        /* Menu size and placement - width based on title "PlayStation Launcher" (20 chars) */
        const int line_height = 24;
        const int title_gap = line_height / 2;
        const int padding = 16;  /* 8px margin on each side */
        const int width = 20 * 8 + padding;  /* 176 */
        const int height = 4 * line_height + (line_height / 2) + title_gap;
        const int x = (640 / 2) - (width / 2);
        const int y = (480 / 2) - (height / 2);
        const int x_item = x + (padding / 2);

        draw_popup_menu(x, y, width, height);

        int cur_y = y + 2;
        font_bmp_begin_draw();
        font_bmp_set_color(sf_ui[0] == UI_FOLDERS ? menu_title_color : text_color);

        font_bmp_draw_main(x + width / 2 - (20 * 8 / 2), cur_y, "PlayStation Launcher");

        cur_y += title_gap;
        cur_y += line_height;
        font_bmp_set_color(psx_launcher_choice == 0 ? highlight_color : text_color);
        font_bmp_draw_main(x_item, cur_y, "Bleemcast!");

        cur_y += line_height;
        font_bmp_set_color(psx_launcher_choice == 1 ? highlight_color : text_color);
        font_bmp_draw_main(x_item, cur_y, "Bloom");

        cur_y += line_height;
        font_bmp_set_color(psx_launcher_choice == 2 ? highlight_color : text_color);
        font_bmp_draw_main(x_item, cur_y, "Close");
    } else {
        /* LineDesc/Grid modes - keep original sizing */
        const int line_height = 32;
        const int width = 200;
        const int height = 4 * line_height + (line_height / 2);
        const int x = (640 / 2) - (width / 2);
        const int y = (480 / 2) - (height / 2);
        const int x_item = x + 10;

        draw_popup_menu(x, y, width, height);

        int cur_y = y + 2;
        font_bmf_begin_draw();
        font_bmf_set_height_default();

        font_bmf_draw_centered(x + width / 2, cur_y, text_color, "PlayStation Launcher");

        cur_y += line_height;
        font_bmf_draw(x_item, cur_y, psx_launcher_choice == 0 ? highlight_color : text_color, "Bleemcast!");

        cur_y += line_height;
        font_bmf_draw(x_item, cur_y, psx_launcher_choice == 1 ? highlight_color : text_color, "Bloom");

        cur_y += line_height;
        font_bmf_draw(x_item, cur_y, psx_launcher_choice == 2 ? highlight_color : text_color, "Close");
    }
}

/*
 * DC Now (dreamcast.online/now) Player Status Popup
 */
#include "../dcnow/dcnow_api.h"
#include "../dcnow/dcnow_net_init.h"

static dcnow_data_t dcnow_data;
static int dcnow_choice = 0;
static bool dcnow_data_fetched = false;
static bool dcnow_is_loading = false;
static bool dcnow_net_initialized = false;
static char connection_status[128] = "";

/* Visual callback for network connection status - called FROM update_status() in dcnow_net_init.c */
/* NOTE: This is called INSIDE a pvr_scene_begin/finish from update_status(), so DON'T call them again */
static void dcnow_connection_status_callback(const char* message) {
    /* Draw transparent background overlay */
    draw_set_list(PVR_LIST_TR_POLY);
    pvr_list_begin(PVR_LIST_TR_POLY);

    /* Semi-transparent black background */
    const int border_width = 2;
    draw_draw_quad(160 - border_width, 200 - border_width,
                   320 + (2 * border_width), 80 + (2 * border_width),
                   0xFFFFFFFF);  /* White border */

    draw_draw_quad(160, 200, 320, 80, 0xFF000000);  /* Black background */

    /* Draw text */
    font_bmf_begin_draw();
    font_bmf_set_height_default();
    font_bmf_draw_centered(320, 215, 0xFFFFFFFF, "DC Now - Connecting");
    font_bmf_draw_centered(320, 240, 0xFFFFFF00, message);  /* Yellow status */

    pvr_list_finish();
}

void
dcnow_setup(enum draw_state* state, struct theme_color* _colors, int* timeout_ptr, uint32_t title_color) {
    popup_setup(state, _colors, timeout_ptr, title_color);
    dcnow_choice = 0;

    /* Set the draw state to DRAW_DCNOW_PLAYERS to actually show the DC Now menu */
    *state = DRAW_DCNOW_PLAYERS;

    /* Network initialization is now done via menu option, not automatically */
    /* User can select "Connect to DreamPi" from the DC Now menu */

    /* If network is already initialized and we haven't fetched data yet, try to fetch */
    if (dcnow_net_initialized && !dcnow_data_fetched && !dcnow_is_loading) {
        dcnow_is_loading = true;

        /* Attempt to fetch fresh data from dreamcast.online/now */
        int result = dcnow_fetch_data(&dcnow_data, 5000);  /* 5 second timeout */

        if (result == 0) {
            dcnow_data_fetched = true;
        } else {
            /* Failed to fetch - try to use cached data */
            if (!dcnow_get_cached_data(&dcnow_data)) {
                /* No cached data available, show error */
                memset(&dcnow_data, 0, sizeof(dcnow_data));
                strcpy(dcnow_data.error_message, "Not connected - select Connect to begin");
                dcnow_data.data_valid = false;
            }
        }

        dcnow_is_loading = false;
    } else if (!dcnow_net_initialized) {
        /* Show message prompting user to connect */
        memset(&dcnow_data, 0, sizeof(dcnow_data));
        strcpy(dcnow_data.error_message, "Not connected");
        dcnow_data.data_valid = false;
    }
}

/* Helper to render a connection status frame before blocking operation */
static void render_connection_frame(const char* message) {
    pvr_wait_ready();
    pvr_scene_begin();

    draw_set_list(PVR_LIST_OP_POLY);
    pvr_list_begin(PVR_LIST_OP_POLY);
    pvr_list_finish();

    draw_set_list(PVR_LIST_TR_POLY);
    pvr_list_begin(PVR_LIST_TR_POLY);

    /* White border */
    const int border_width = 2;
    draw_draw_quad(160 - border_width, 200 - border_width,
                   320 + (2 * border_width), 80 + (2 * border_width),
                   0xFFFFFFFF);

    /* Black background */
    draw_draw_quad(160, 200, 320, 80, 0xFF000000);

    /* Message */
    font_bmf_begin_draw();
    font_bmf_draw_centered(320, 230, 0xFFFFFFFF, message);

    pvr_list_finish();
    pvr_scene_finish();
}

void
handle_input_dcnow(enum control input) {
    switch (input) {
        case A: {
            /* A button: Connect or Refresh */
            if (!dcnow_net_initialized) {
                /* Connect to DreamPi - SET CALLBACK FOR VISUAL FEEDBACK */
                printf("DC Now: Starting connection...\n");
                dcnow_set_status_callback(dcnow_connection_status_callback);

                /* Initialize network/modem - this will show status messages via callback */
                int net_result = dcnow_net_early_init();

                /* Clear callback after connection attempt */
                dcnow_set_status_callback(NULL);

                if (net_result < 0) {
                    printf("DC Now: Connection failed: %d\n", net_result);
                    memset(&dcnow_data, 0, sizeof(dcnow_data));
                    snprintf(dcnow_data.error_message, sizeof(dcnow_data.error_message),
                            "Connection failed (error %d)", net_result);
                    dcnow_data.data_valid = false;
                    dcnow_net_initialized = true;  /* Mark as attempted */
                } else {
                    printf("DC Now: Connection successful\n");

                    /* Initialize DC Now API layer */
                    int api_result = dcnow_init();
                    if (api_result < 0) {
                        printf("DC Now: API init failed: %d\n", api_result);
                        memset(&dcnow_data, 0, sizeof(dcnow_data));
                        snprintf(dcnow_data.error_message, sizeof(dcnow_data.error_message),
                                "Init failed (error %d)", api_result);
                        dcnow_data.data_valid = false;
                    } else {
                        printf("DC Now: API ready, fetching data...\n");

                        /* Now fetch data */
                        dcnow_is_loading = true;
                        int result = dcnow_fetch_data(&dcnow_data, 5000);
                        if (result == 0) {
                            dcnow_data_fetched = true;
                            printf("DC Now: Data fetched successfully\n");
                        } else {
                            printf("DC Now: Data fetch failed: %d\n", result);
                            /* error_message already set by dcnow_fetch_data */
                        }
                        dcnow_is_loading = false;
                    }

                    dcnow_net_initialized = true;
                }
            } else {
                /* Already connected - refresh data */
                printf("DC Now: Refreshing data...\n");
                dcnow_data_fetched = false;
                dcnow_is_loading = true;

                int result = dcnow_fetch_data(&dcnow_data, 5000);
                if (result == 0) {
                    dcnow_data_fetched = true;
                    printf("DC Now: Data refreshed successfully\n");
                } else {
                    printf("DC Now: Data refresh failed: %d\n", result);
                }

                dcnow_is_loading = false;
            }
        } break;
        case B: {
            /* B button: Close DC Now menu */
            *state_ptr = DRAW_UI;
        } break;
        case START: {
            /* START button: Do nothing (let it close the menu naturally) */
        } break;
        case UP: {
            /* Scroll up through game list */
            if (dcnow_data.data_valid && dcnow_choice > 0) {
                dcnow_choice--;
            }
        } break;
        case DOWN: {
            /* Scroll down through game list */
            if (dcnow_data.data_valid && dcnow_choice < dcnow_data.game_count) {
                dcnow_choice++;
            }
        } break;
        default:
            break;
    }
}

void
draw_dcnow_op(void) { /* Opaque pass - nothing to draw */ }

void
draw_dcnow_tr(void) {
    z_set_cond(205.0f);

    if (sf_ui[0] == UI_SCROLL || sf_ui[0] == UI_FOLDERS) {
        /* Scroll/Folders mode - use bitmap font */
        const int line_height = 20;
        const int title_gap = line_height;
        const int padding = 16;
        const int max_visible_games = 10;  /* Show at most 10 games at once */

        /* Calculate width based on content */
        int max_line_len = 30;  /* "Dreamcast Live - Online Now" */
        if (dcnow_data.data_valid) {
            for (int i = 0; i < dcnow_data.game_count; i++) {
                int len = strlen(dcnow_data.games[i].game_name) + 10;  /* name + " - 999 players" */
                if (len > max_line_len) {
                    max_line_len = len;
                }
            }
        } else {
            int err_len = strlen(dcnow_data.error_message);
            if (err_len > max_line_len) {
                max_line_len = err_len;
            }
        }

        const int width = max_line_len * 8 + padding;

        int num_lines = 2;  /* Title + total players line */
        if (dcnow_data.data_valid) {
            num_lines += (dcnow_data.game_count < max_visible_games ? dcnow_data.game_count : max_visible_games);
            num_lines++;  /* Close button */
        } else {
            num_lines += 2;  /* Error message + close button */
        }

        const int height = num_lines * line_height + title_gap;
        const int x = (640 / 2) - (width / 2);
        const int y = (480 / 2) - (height / 2);
        const int x_item = x + (padding / 2);

        draw_popup_menu(x, y, width, height);

        int cur_y = y + 2;
        font_bmp_begin_draw();
        font_bmp_set_color(menu_title_color);

        /* Title */
        const char* title = "Dreamcast Live - Online Now";
        int title_x = x + (width / 2) - ((strlen(title) * 8) / 2);
        font_bmp_draw_main(title_x, cur_y, title);

        cur_y += title_gap;

        if (dcnow_is_loading) {
            /* Show loading message */
            font_bmp_set_color(text_color);
            font_bmp_draw_main(x_item, cur_y, "Loading...");
            cur_y += line_height;
        } else if (dcnow_data.data_valid) {
            /* Show total players */
            char total_buf[64];
            snprintf(total_buf, sizeof(total_buf), "Total Active Players: %d", dcnow_data.total_players);
            font_bmp_set_color(text_color);
            font_bmp_draw_main(x_item, cur_y, total_buf);
            cur_y += line_height;

            /* Show game list */
            if (dcnow_data.game_count == 0) {
                font_bmp_set_color(text_color);
                font_bmp_draw_main(x_item, cur_y, "No active games");
                cur_y += line_height;
            } else {
                for (int i = 0; i < dcnow_data.game_count && i < max_visible_games; i++) {
                    char game_buf[128];
                    const char* status = dcnow_data.games[i].is_active ? "" : " (offline)";

                    if (dcnow_data.games[i].player_count == 1) {
                        snprintf(game_buf, sizeof(game_buf), "%s - %d player%s",
                                 dcnow_data.games[i].game_name,
                                 dcnow_data.games[i].player_count,
                                 status);
                    } else {
                        snprintf(game_buf, sizeof(game_buf), "%s - %d players%s",
                                 dcnow_data.games[i].game_name,
                                 dcnow_data.games[i].player_count,
                                 status);
                    }

                    font_bmp_set_color(i == dcnow_choice ? highlight_color : text_color);
                    font_bmp_draw_main(x_item, cur_y, game_buf);
                    cur_y += line_height;
                }
            }
        } else {
            /* Show error message or connection prompt */
            font_bmp_set_color(text_color);
            font_bmp_draw_main(x_item, cur_y, dcnow_data.error_message);
            cur_y += line_height;
            if (!dcnow_net_initialized) {
                font_bmp_draw_main(x_item, cur_y, "Press A to connect");
            } else {
                font_bmp_draw_main(x_item, cur_y, "Press A to retry");
            }
            cur_y += line_height;
            font_bmp_draw_main(x_item, cur_y, "Press B to close");
            cur_y += line_height;
        }

        /* Close button */
        font_bmp_set_color(dcnow_choice == dcnow_data.game_count ? highlight_color : text_color);
        font_bmp_draw_main(x_item, cur_y, "Close");

    } else {
        /* LineDesc/Grid modes - use vector font */
        const int line_height = 28;
        const int title_gap = line_height / 2;
        const int padding = 20;
        const int max_visible_games = 8;

        /* Calculate width based on content */
        int max_line_len = 300;  /* Title width estimate */

        int num_lines = 2;  /* Title + total */
        if (dcnow_data.data_valid) {
            num_lines += (dcnow_data.game_count < max_visible_games ? dcnow_data.game_count : max_visible_games);
            num_lines++;  /* Close */
        } else {
            num_lines += 2;  /* Error + retry hint */
        }

        const int width = max_line_len + padding;
        const int height = num_lines * line_height + title_gap;
        const int x = (640 / 2) - (width / 2);
        const int y = (480 / 2) - (height / 2);
        const int x_item = x + 10;

        draw_popup_menu(x, y, width, height);

        int cur_y = y + 2;
        font_bmf_begin_draw();
        font_bmf_set_height_default();

        /* Title */
        font_bmf_draw_centered(x + width / 2, cur_y, text_color, "Dreamcast Live - Online Now");
        cur_y += title_gap;

        if (dcnow_is_loading) {
            cur_y += line_height;
            font_bmf_draw(x_item, cur_y, text_color, "Loading...");
        } else if (dcnow_data.data_valid) {
            /* Total players */
            cur_y += line_height;
            char total_buf[64];
            snprintf(total_buf, sizeof(total_buf), "Total Active Players: %d", dcnow_data.total_players);
            font_bmf_draw(x_item, cur_y, text_color, total_buf);

            /* Game list */
            if (dcnow_data.game_count == 0) {
                cur_y += line_height;
                font_bmf_draw(x_item, cur_y, text_color, "No active games");
            } else {
                for (int i = 0; i < dcnow_data.game_count && i < max_visible_games; i++) {
                    cur_y += line_height;
                    char game_buf[128];
                    const char* status = dcnow_data.games[i].is_active ? "" : " (offline)";

                    if (dcnow_data.games[i].player_count == 1) {
                        snprintf(game_buf, sizeof(game_buf), "%s - %d player%s",
                                 dcnow_data.games[i].game_name,
                                 dcnow_data.games[i].player_count,
                                 status);
                    } else {
                        snprintf(game_buf, sizeof(game_buf), "%s - %d players%s",
                                 dcnow_data.games[i].game_name,
                                 dcnow_data.games[i].player_count,
                                 status);
                    }

                    uint32_t color = (i == dcnow_choice) ? highlight_color : text_color;
                    font_bmf_draw_auto_size(x_item, cur_y, color, game_buf, width - 20);
                }
            }
        } else {
            /* Error or connection prompt */
            cur_y += line_height;
            font_bmf_draw(x_item, cur_y, text_color, dcnow_data.error_message);
            cur_y += line_height;
            if (!dcnow_net_initialized) {
                font_bmf_draw(x_item, cur_y, text_color, "Press A to connect");
            } else {
                font_bmf_draw(x_item, cur_y, text_color, "Press A to retry");
            }
            cur_y += line_height;
            font_bmf_draw(x_item, cur_y, text_color, "Press B to close");
        }

        /* Close */
        cur_y += line_height;
        uint32_t close_color = (dcnow_choice == dcnow_data.game_count) ? highlight_color : text_color;
        font_bmf_draw(x_item, cur_y, close_color, "Close");
    }
}
