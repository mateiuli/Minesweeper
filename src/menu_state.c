#include "menu_state.h"

void play_menu_state()
{
	// menu options
	int c, selected_item = -1, n_choices = 3;
	
	// menu option's name
	char *choices[] = {
		"New game",
		"Settings",
		"Quit",
	};

	// full size window
	WINDOW *menu_win = newwin(0, 0, 0, 0);
	WINDOW *menu_box = subwin(menu_win, 6, 12, (LINES - 6)/2 + 3, (COLS - 12)/2);
	
	// same colors as intro state
	wbkgd(menu_win, COLOR_PAIR(7));
	
	// border - without animation
	loading_animation(menu_win, 0);

	// logo
	wattron(menu_win, COLOR_PAIR(15));
	print_splash_screen(menu_win, 0);
	wattroff(menu_win, COLOR_PAIR(15));

	// menu and items
	ITEM **menu_items = create_menu_items(choices, n_choices);
	MENU *menu = create_menu(choices, n_choices, menu_items);

	// menu's parent window
	set_menu_win(menu, menu_box);

	// show menu
	post_menu(menu);

	// update screen
	wrefresh(menu_win);
	wrefresh(menu_box);
	
	// if you dont understand this code, you should be flipping burgers instead
	while(selected_item == -1 && (c = getch())) {
		switch(c) {
			case KEY_DOWN:
		        menu_driver(menu, REQ_DOWN_ITEM);
		        beep();
				break;

			case KEY_UP:
				menu_driver(menu, REQ_UP_ITEM);
				beep();
				break;

			// enter
			case 10:
				selected_item = item_index(current_item(menu));	
				break;
		}

		wrefresh(menu_box);
	}

	// free memory
	destroy_menu(menu, menu_items, choices, n_choices);
	delwin(menu_box);
	delwin(menu_win);

	// taking some action based on the selected itme
	switch(selected_item) {
		case QUIT:
			// do nothing - just quit
		break;

		case SETTINGS:
			// enter settings state
			play_settings_state();
		break;

		case NEW_GAME:
			// enter new game state
			play_game_state(0);
		break;
	}
}

MENU *create_menu(char **choices, int n_choices, ITEM **my_items) 
{
	MENU *my_menu = new_menu((ITEM**)my_items);

	// eye-candy menu
	set_menu_fore(my_menu, COLOR_PAIR(14));
	set_menu_back(my_menu, COLOR_PAIR(7));
	set_menu_mark(my_menu, "-> ");
	//set_menu_spacing(my_menu, 4, 2, 0);

	return my_menu;
}

ITEM **create_menu_items(char **choices, int n_choices) 
{
	ITEM **my_items;
	int i;

	// create menu items
	my_items = (ITEM**)calloc(n_choices + 1, sizeof(ITEM*));

	// map each item with name and description
	for(i = 0; i < n_choices; i++)
		my_items[i] = new_item(choices[i], NULL);
		
	my_items[n_choices] = (ITEM*)NULL;

	return my_items;
}

void destroy_menu(MENU *menu, ITEM **my_items, char **choices, int n_choices) 
{
	int i;
	for(i = 0; i < n_choices; i++) {
		free_item(my_items[i]);
		free(my_items[i]);		
	}
	
	unpost_menu(menu);
	free(my_items);
	free_menu(menu);
	endwin();
}
