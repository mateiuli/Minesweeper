#include <unistd.h>

#include "util.h"
#include "minesweeper.h"
#include "intro_state.h"
#include "menu_state.h"
#include "settings_state.h"

int main()
{
	// init
	ms_init_game();
	ms_init_colors();

	// load configuration file
	load_saved_settings();

	// if this comment is removed the program will blow up 
	refresh();
	play_intro_state();
	
	refresh();
	//play_menu_state();
	play_game_state(0);

	// end	
	ms_end_game();

	return 0;
}

void ms_init_game()
{
	// start curses mode
	initscr();
	
	// line buffering disabled
	raw();
	
	// get all the keys
	keypad(stdscr, TRUE);
	
	// dont echo while we do getch
	noecho();
	
	// hide the cursor
	curs_set(0);
	
	// resize terminal (xterm)
	//resizeterm(40, 100);
	// fix for ESC key delay
	if (getenv("ESCDELAY") == NULL)
 		ESCDELAY = 25;

}

void ms_end_game()
{
	// end curses mode
	endwin();
}

void ms_init_colors() 
{
	if(has_colors() == FALSE)
	{	
		endwin();
		printf("Your terminal does not support color\n");
		return;
	}

	start_color();
	
	init_pair(1,  COLOR_RED,     COLOR_BLACK);
	init_pair(2,  COLOR_GREEN,   COLOR_BLACK);
	init_pair(3,  COLOR_YELLOW,  COLOR_BLACK);
	init_pair(4,  COLOR_BLUE,    COLOR_BLACK);
	init_pair(5,  COLOR_WHITE,   COLOR_MAGENTA);
	init_pair(6,  COLOR_CYAN,    COLOR_BLACK);
	init_pair(7,  COLOR_BLUE,    COLOR_WHITE);
	init_pair(8,  COLOR_WHITE,   COLOR_RED);
	init_pair(9,  COLOR_BLACK,   COLOR_GREEN);
	init_pair(10, COLOR_BLUE,    COLOR_YELLOW);
	init_pair(11, COLOR_WHITE,   COLOR_BLUE);
	init_pair(12, COLOR_WHITE,   COLOR_MAGENTA);
	init_pair(13, COLOR_BLACK,   COLOR_CYAN);
	// menu selected itme
	init_pair(14, COLOR_RED,     COLOR_WHITE);
	// used by splash-screen
	init_pair(15, COLOR_CYAN,    COLOR_WHITE);
	init_pair(16, COLOR_WHITE,	 COLOR_CYAN);
	init_pair(17, COLOR_BLACK,	 COLOR_RED);

}