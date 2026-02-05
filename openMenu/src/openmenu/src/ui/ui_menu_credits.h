/*
 * File: ui_menu_credits.h
 * Project: ui
 * File Created: Monday, 12th July 2021 11:40:41 pm
 * Author: Hayden Kowalchuk
 * -----
 * Copyright (c) 2021 Hayden Kowalchuk, Hayden Kowalchuk
 * License: BSD 3-clause "New" or "Revised" License, http://www.opensource.org/licenses/BSD-3-Clause
 */

#pragma once

#include <backend/gd_item.h>
#include <openmenu_settings.h>
#include "ui/common.h"

struct theme_color;

void menu_setup(enum draw_state* state, struct theme_color* _colors, int* timeout_ptr, uint32_t title_color);
void popup_setup(enum draw_state* state, struct theme_color* _colors, int* timeout_ptr, uint32_t title_color);
void exit_menu_setup(enum draw_state* state, struct theme_color* _colors, int* timeout_ptr, uint32_t title_color, int is_folder);
void cb_menu_setup(enum draw_state* state, struct theme_color* _colors, int* timeout_ptr, uint32_t title_color);
void saveload_setup(enum draw_state* state, struct theme_color* _colors, int* timeout_ptr, uint32_t title_color);

void handle_input_menu(enum control input);
void handle_input_credits(enum control input);
void handle_input_multidisc(enum control input);
void handle_input_exit(enum control input);
void handle_input_codebreaker(enum control input);
void handle_input_psx_launcher(enum control input);
void handle_input_saveload(enum control input);

void draw_menu_op(void);
void draw_menu_tr(void);

void draw_credits_op(void);
void draw_credits_tr(void);

void draw_multidisc_op(void);
void draw_multidisc_tr(void);

void draw_exit_op(void);
void draw_exit_tr(void);

void draw_codebreaker_op(void);
void draw_codebreaker_tr(void);

void draw_psx_launcher_op(void);
void draw_psx_launcher_tr(void);

void draw_saveload_op(void);
void draw_saveload_tr(void);

void dcnow_setup(enum draw_state* state, struct theme_color* _colors, int* timeout_ptr, uint32_t title_color);
void handle_input_dcnow(enum control input);
void draw_dcnow_op(void);
void draw_dcnow_tr(void);
void dcnow_background_tick(void);

void set_cur_game_item(const gd_item* id);
const gd_item* get_cur_game_item();
