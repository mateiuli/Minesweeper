#ifndef __MENUSTATE_H__
#define __MENUSTATE_H__

#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>
#include "util.h"
#include "settings_state.h"
#include "game_state.h"

enum MENU_ACTION {
	NEW_GAME,
	SETTINGS,
	QUIT,
};

void play_menu_state();
MENU *create_menu(char**, int, ITEM**);
ITEM **create_menu_items(char**, int);
void destroy_menu(MENU*, ITEM**, char**, int);

#endif