#ifndef __SETTINGSSTATE_H__
#define __SETTINGSSTATE_H__

#include <ncurses.h>
#include <form.h>
#include "menu_state.h"
#include "util.h"

#define SETTINGS_FILE "game.settings"

typedef struct {
	int rows, cols, mines, cell_size;
} CONFIG_FILE;

// one definition rule
// avoiding link-time errors
extern CONFIG_FILE ms_game_settings;

// game state
void play_settings_state();
void print_settings_values(WINDOW*);

// config file
void load_saved_settings();
void save_settings();
void incr_value();
void decr_value();
void incr_rows();
void decr_rows();
void incr_cols();
void decr_cols();
void incr_mines();
void decr_mines();
void check_mines();
void incr_cell();
void decr_cell();
void check_cell();

#endif