#include "intro_state.h"
#include "util.h"

void play_intro_state() 
{
	WINDOW *intro = newwin(0, 0, 0, 0);
	
	// window's background color
	wbkgd(intro, COLOR_PAIR(7));

	// fake loading :)
	print_in_middle(intro, "loading...");
	wrefresh(intro);
	
	// play some useless animation
	loading_animation(intro, 1);

	// splash screen (animated)
	wattron(intro, COLOR_PAIR(15));
	print_splash_screen(intro, 1);
	wattroff(intro, COLOR_PAIR(15));

	// loading complete
	print_in_middle(intro, "Press any key to continue...");
	
	// play some random terminal sound when ready
	beep();

	// waiting for the 'any' key to be pressed :))
	getch();

	// destroy state
	destroy_intro(intro);
}

void destroy_intro(WINDOW *intro)
{
	wrefresh(intro);
	delwin(intro);
	endwin();
}