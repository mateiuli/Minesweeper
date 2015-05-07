#include "settings_state.h"

CONFIG_FILE ms_game_settings = {9, 9, 10, 3};

void play_settings_state()
{
	int c, selected_item = -1, n_choices = 5;
	
	// menu options
	char *choices[] = {
		"Rows number",
		"Cols number",
		"Mines number",
		"Cell size", 
		"Go back",
	};

	// full size window
	WINDOW *settings_win = newwin(0, 0, 0, 0);
	WINDOW *settings_box = subwin(settings_win, 6, 21, (LINES - 6)/2 + 3, (COLS - 20)/2);
	
	// same colors as intro state
	wbkgd(settings_win, COLOR_PAIR(7));
	
	// border - without animation
	loading_animation(settings_win, 0);

	// logo
	wattron(settings_win, COLOR_PAIR(15));
	print_splash_screen(settings_win, 0);
	wattroff(settings_win, COLOR_PAIR(15));

	// menu and items
	ITEM **settings_items = create_menu_items(choices, n_choices);
	MENU *settings = create_menu(choices, n_choices, settings_items);

	// menu's parent window
	set_menu_win(settings, settings_box);

	// show menu
	post_menu(settings);

	// info message
	wattron(settings_win, COLOR_PAIR(14));
	mvwprintw(settings_win, LINES - 6, (COLS - 52)/2, "Use LEFT/RIGHT arrows to decrease/increase the value");
	wattroff(settings_win, COLOR_PAIR(14));

	// update screen
	wrefresh(settings_win);
	wrefresh(settings_box);

	// print settings values
	print_settings_values(settings_box);	

	// if you dont understand this code, you should be flipping burgers instead
	while(selected_item != n_choices - 1 && (c = getch())) {
		switch(c) {
			case KEY_DOWN:
		        menu_driver(settings, REQ_DOWN_ITEM);
		        beep();
				break;

			case KEY_UP:
				menu_driver(settings, REQ_UP_ITEM);
				beep();
				break;

			// enter - exit only
			case 10:
				selected_item = item_index(current_item(settings));
				break;

			case KEY_RIGHT:
				incr_value(item_index(current_item(settings)));
				break;

			case KEY_LEFT:
				decr_value(item_index(current_item(settings)));
				break;

			default:
				// exit with ESC
				selected_item = c == 27 ? n_choices - 1 : selected_item;
				break;
		}

		// print the new values
		print_settings_values(settings_box);
		wrefresh(settings_box);
	}

	// free memory
	destroy_menu(settings, settings_items, choices, n_choices);
	delwin(settings_box);
	delwin(settings_win);

	// write changes to the file
	save_settings();
	
	// escape was pressed, return to menu state
	play_menu_state();
}

void print_settings_values(WINDOW *settings_box)
{
	wattron(settings_box, COLOR_PAIR(7));
	mvwprintw(settings_box, 0, 18, "%3d", ms_game_settings.rows);
	mvwprintw(settings_box, 1, 18, "%3d", ms_game_settings.cols);
	mvwprintw(settings_box, 2, 18, "%3d", ms_game_settings.mines);
	mvwprintw(settings_box, 3, 18, "%3d", ms_game_settings.cell_size);
	wattroff(settings_box, COLOR_PAIR(7));
	wrefresh(settings_box);
}

void incr_value(int index)
{
	switch(index) {
		// rows
		case 0:
			incr_rows();
			break;
		// cols
		case 1:
			incr_cols();
			break;
		// mines
		case 2:
			incr_mines();
			break;
		// cell height = cell_size, width = 2 * cell_size
		case 3: 
			incr_cell();
			break;
	}

	// auto cell size
	check_cell();
}

void decr_value(int index)
{
	switch(index) {
		// rows
		case 0:
			decr_rows();
			break;
		// cols
		case 1:
			decr_cols();
			break;
		// mines
		case 2:
			decr_mines();
			break;
		// cell size
		case 3: 
			decr_cell();
			break;
	}

	// avoid a bug
	check_mines();
}

// load settings from file
void load_saved_settings() 
{
	FILE *f = fopen(SETTINGS_FILE, "rb");
	
	if(!f) {
		// create file
		save_settings();
		return;
	}

	// file exists
	fread(&ms_game_settings, sizeof(ms_game_settings), 1, f);

	fclose(f);
}

// save settings to file
void save_settings()
{
	FILE *f = fopen(SETTINGS_FILE, "wb");
	fwrite(&ms_game_settings, sizeof(ms_game_settings), 1, f);
	fclose(f);
}

void incr_rows()
{
	if(ms_game_settings.rows < 30)
		ms_game_settings.rows++;
}

void decr_rows()
{
	if(ms_game_settings.rows > 1)
		ms_game_settings.rows--;
}

void incr_cols()
{
	if(ms_game_settings.cols < 30)
		ms_game_settings.cols++;
}

void decr_cols()
{
	if(ms_game_settings.cols > 1)
		ms_game_settings.cols--;
}

void incr_mines()
{
	if(ms_game_settings.mines < ms_game_settings.rows * ms_game_settings.cols - 1)
		ms_game_settings.mines++;
}

void decr_mines()
{
	if(ms_game_settings.mines > 1)
		ms_game_settings.mines--;
}

void check_mines()
{
	int max = ms_game_settings.rows * ms_game_settings.cols - 1;
	if(ms_game_settings.mines > max)
		ms_game_settings.mines = max;

	if(!ms_game_settings.mines)
		ms_game_settings.mines = 1;
}

void incr_cell()
{
	if(ms_game_settings.cell_size < 3)
		ms_game_settings.cell_size++;
}

void decr_cell()
{
	if(ms_game_settings.cell_size > 1)
		ms_game_settings.cell_size--;
}

void check_cell()
{
	// 12 rows and 18 cols fits a 132x43 terminal with cell_size = 2
	int max;
	if(ms_game_settings.rows > ms_game_settings.cols)
		max = ms_game_settings.rows;
	else
		max = ms_game_settings.cols;

	if(max >= 1)
		ms_game_settings.cell_size = 3;
	
	if(max > 10)
		ms_game_settings.cell_size = 2;

	if(max > 12)	
		ms_game_settings.cell_size = 1;
}