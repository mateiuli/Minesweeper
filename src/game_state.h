#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

#include <ncurses.h>
#include <sys/time.h>
#include <time.h>

#include "util.h"
#include "settings_state.h"
#include "menu_state.h"

// powers of 2
enum FLAGS {
	FLAG = 1,
	MINE = 2,
	HIT  = 4,
};

enum STATUS {
	GAME_OVER,
	GAME_WON,
	GAME_STOP,
	GAME_OK,
	GAME_MULTIPLE_HIT,
	GAME_CONTINUE,
};

typedef struct {
	WINDOW *wcell;
	unsigned int flags; /* bit flags */
	int adj_mines;		/* adjacent mines */
} MS_CELL;

typedef struct {
	// board
	int rows, cols, mines, hits;
	int cell_size, flags; /* flags counter */
	
	// time
	int timer;		/* since first hit */
	int start_time;	/* new round */
	
	MS_CELL **matrix;
} MS_GAME;

MS_CELL **create_board(WINDOW*, int, int, int);
void play_game_state(int);
void init_game_state(WINDOW**, WINDOW**, int);
void destroy_board(MS_CELL**, int, int);
void display_HUD(WINDOW*, MS_GAME*);
void generate_mines(MS_GAME*);
void hit_adjacent(MS_GAME*, int, int);
void show_mines(MS_GAME*);
void print_message(WINDOW*, char*, char);
void load_game(MS_GAME*, char*);
void save_game(MS_GAME*);
int multiple_hit(MS_GAME*, int, int);
int multiple_mark(MS_GAME*, int, int);
int hit_cell(MS_GAME*, int, int);
int game_won(MS_GAME*);

// struct methods
void select_cell(MS_CELL*);
void deselect_cell(MS_CELL*);
void mark_cell(MS_CELL*, int);
void flag_cell(MS_CELL*, int);
void add_flag(MS_CELL*, enum FLAGS);
void del_flag(MS_CELL*, enum FLAGS);
int toggle_flag(MS_CELL*, enum FLAGS);

#endif