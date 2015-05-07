#include "game_state.h"

void play_game_state(int start_time)
{
	// main window
	WINDOW *game_win = newwin(0, 0, 0, 0), *board;

	// some game settings - dont change 'cuz I now where you live
	int c, sel_row, sel_col; 
	
	// timer - since first hit
	struct timeval start_timer, end_timer;

	// timer - since current round
	struct timeval r_start_timer, r_end_timer;	

	// signals
	enum STATUS status;

	// some random game details - defaults
	MS_GAME game = {
		.rows 	= ms_game_settings.rows,
		.cols 	= ms_game_settings.cols,
		.mines 	= ms_game_settings.mines,
		.flags 	= 0,
		.timer 	= 0,
		.cell_size = ms_game_settings.cell_size,
	};

	// init windows
	init_game_state(&game_win, &board, game.cell_size);

	// generate board
	game.matrix = create_board(board, game.rows, game.cols, game.cell_size);	

	// place random mines
	generate_mines(&game);

	// alias :)
	MS_CELL **matrix = game.matrix;

	// update screen
	wrefresh(game_win);	

	// default selected cell
	sel_row = game.rows / 2;
	sel_col = game.cols / 2;
	select_cell(&matrix[sel_row][sel_col]);	
	
	// trick to update time without waiting for user input
	timeout(1000); 

	// here starts the round
	gettimeofday(&r_start_timer, NULL);

	// add seconds 
	r_start_timer.tv_sec -= start_time;
	
	// HUD
	display_HUD(game_win, &game);

	// magic. do not touch.
	while(status != GAME_OVER  && status != GAME_WON && (c = getch()) != 27) {
		// previous cell 
		deselect_cell(&matrix[sel_row][sel_col]);		
		
		switch(c) {
			case KEY_LEFT:
				sel_col > 0 ? sel_col-- : sel_col;
				break;

			case KEY_RIGHT:
				sel_col < ms_game_settings.cols - 1 ? sel_col++ : sel_col;
				break;

			case KEY_UP:
				sel_row > 0 ? sel_row-- : sel_row;
				break;

			case KEY_DOWN:
				sel_row < ms_game_settings.rows - 1 ? sel_row++ : sel_row;
				break;

			// M key: toggle flag
			case 109:
				if(!(matrix[sel_row][sel_col].flags & HIT))
					game.flags += toggle_flag(&matrix[sel_row][sel_col], FLAG);
				else
					// possible multiple mark
					game.flags += multiple_mark(&game, sel_row, sel_col);
				break;
			
			// X key: hit
			case 120:
				// start timer
				if(game.hits == 0)
					gettimeofday(&start_timer, NULL);
				
				if(!(matrix[sel_row][sel_col].flags & FLAG)) {
					// hit the cell
					switch(hit_cell(&game, sel_row, sel_col)) {
						case GAME_OVER: 
							// send exit signal
							status = GAME_OVER;
							continue; /* go to the next iteration to break */							
							break;

						// simple hit
						case GAME_OK:
							hit_adjacent(&game, sel_row, sel_col);
							break;

						// multiple hit
						case GAME_MULTIPLE_HIT:
							status = multiple_hit(&game, sel_row, sel_col);

							if(status == GAME_OVER)
								continue;

							break;

						default:
							break;
					}
				}

				break;

			case 's':
				//mvprintw(LINES-2, COLS-15, "Saving game...");
				//save_game(&game);
				break;

		}

		// check if the game should end
		status = game_won(&game);
		if(status == GAME_WON) 
			continue;

		// DEBUG ONLY
		//mvprintw(LINES-1, 0, "Flags: %d", matrix[sel_row][sel_col].flags);
		//mvprintw(LINES-1, 15, "Adj_mines: %3d", matrix[sel_row][sel_col].adj_mines);
		//mvprintw(LINES-1, 35, "Hits: %3d", game.hits);

		// update timer
		if(game.hits > 0) {
			gettimeofday(&end_timer, NULL);
			game.timer = end_timer.tv_sec - start_timer.tv_sec;
		}

		// update round timer
		gettimeofday(&r_end_timer, NULL);
		game.start_time = r_end_timer.tv_sec - r_start_timer.tv_sec; 

		// mark cell (if needed - bit flag) with flag, bomb, etc
		mark_cell(&matrix[sel_row][sel_col], game.cell_size);

		// next cell
		select_cell(&matrix[sel_row][sel_col]);

		// update HUD
		display_HUD(game_win, &game);
	}

	// take actions based on the exit signal
	switch(status) {
		case GAME_OVER:
			show_mines(&game);
			beep();
			print_message(game_win, "Game Over. Press ESC to return to main menu.", 8);
			
			while((c = getch()) != 27) {
				// update round timer
				gettimeofday(&r_end_timer, NULL);
				game.start_time = r_end_timer.tv_sec - r_start_timer.tv_sec; 

				// update time
				display_HUD(game_win, &game);
			}

			break;

		case GAME_WON:
			beep();
			print_message(game_win, "Press ENTER to continue or ESC to exit.", 9);

			do {
				c = getch();

				// update round timer
				gettimeofday(&r_end_timer, NULL);
				game.start_time = r_end_timer.tv_sec - r_start_timer.tv_sec; 

				// update time
				display_HUD(game_win, &game);
			} while(c != 10 && c != 27);

			// press enter to play again
			if(c == 10)  {
				// play again
				play_game_state(game.start_time);	
			}

			break;

		default:

			break;
	}

	// free memory
	endwin();
	delwin(board);
	delwin(game_win);
	destroy_board(matrix, ms_game_settings.rows, ms_game_settings.cols);
	
	// ESC was pressed - go to main menu
	if(start_time == 0) {
		play_menu_state();
	}

}

void print_message(WINDOW *game_win, char *str, char color)
{
	wattron(game_win, COLOR_PAIR(color));
	mvwprintw(game_win, 0, (COLS - strlen(str))/2, str);
	wattroff(game_win, COLOR_PAIR(color));
	wrefresh(game_win);
}

void init_game_state(WINDOW **game_win, WINDOW **board, int cell_size) 
{
	// window's background color
	wbkgd(*game_win, COLOR_PAIR(7));

	// play some useless animation
	loading_animation(*game_win, 0);
	
	// board window size
	int w_sizer = cell_size * ms_game_settings.rows + ms_game_settings.rows - 1;	
	int w_sizec = 2 * cell_size * ms_game_settings.cols + ms_game_settings.cols - 1;
	
	// create window
	*board = newwin(w_sizer, w_sizec, (LINES - w_sizer)/2, (COLS - w_sizec)/2);
	wbkgd(*board, COLOR_PAIR(11));
}

void display_HUD(WINDOW *game_win, MS_GAME* game)
{	
	// white on blue
	wattron(game_win, COLOR_PAIR(11));
	
	mvwprintw(game_win, 0, COLS - 10, "Flags:%3d", game->flags);
	mvwprintw(game_win, 0, COLS - 29, "Mines left:%3d", game->mines - game->flags);
	mvwprintw(game_win, 0, 18, "Seconds elapsed:%3ds", game->timer);
	mvwprintw(game_win, 0, 1, "Time: %02d:%02d:%02d", st_hours(game->start_time), st_minutes(game->start_time), st_seconds(game->start_time));
	
	// how to play
	mvwprintw(game_win, LINES - 1, 1, "Use arrows to move    X - hit cell    M - place flag    ESC - main menu");

	// curent date and time
	char buffer[25];
	sprintf_current_datetime(buffer);

	// right down corner
	mvwprintw(game_win, LINES - 1, COLS - 20, "%s", buffer);

	wattroff(game_win, COLOR_PAIR(11));
	wrefresh(game_win);
}

MS_CELL **create_board(WINDOW *wparent, int rows, int cols, int cell_s) 
{
	int i, j;
	MS_CELL **matrix = (MS_CELL**)malloc(rows * sizeof(MS_CELL*));
	
	for(i = 0; i < rows; i++) {
		// create cell
		matrix[i] = (MS_CELL*)malloc(cols * sizeof(MS_CELL));
		
		// create each cell's window
		for(j = 0; j < cols; j++) {
			// relative position to the origin of the parent window
			matrix[i][j].wcell = derwin(wparent, cell_s, 2 * cell_s, i * (cell_s+1), j * 2 * (cell_s)+j);

			// unselected cell
			wbkgd(matrix[i][j].wcell, COLOR_PAIR(11));
			//box(matrix[i][j].wcell, 0, 0);

			wrefresh(matrix[i][j].wcell);

			// no flags
			matrix[i][j].flags = 0;

			// no adjacent mines
			matrix[i][j].adj_mines = 0;
		}
	}

	return matrix;
}

void destroy_board(MS_CELL **matrix, int rows, int cols)
{
	int i, j;
	for(i = 0; i < rows; i++) {
		for(j = 0; j < cols; j++) {
			delwin(matrix[i][j].wcell);
		}

		free(matrix[i]);
	}

	free(matrix);
}

// Placing some mines totally random
void generate_mines(MS_GAME *game)
{
	int i, x = 0, y = 0, mr = 0, mc = 0;

	/*
	*	Getting some unique random numbers using only rand
	*	is slow (loop) and dangerous (trial-and-error iterations)
	*	A better way is to use Reservoir sampling algorithm
	*/
	Point **R = random_coordinates(game->rows, game->cols, game->mines);

	for(i = 0; i < game->mines; i++) {
		// fetch coordinates
		mr = R[i]->row;
		mc = R[i]->col;
	
		// 8 direction 
		for(x = -1; x <= 1; x++)
			for(y = -1; y <= 1; y++) 
				if(x == 0 && y == 0) {
					// make it negative enough for a mine
					game->matrix[mr][mc].adj_mines = -3 * game->mines;
				
					// TODO: delete me
					wbkgd(game->matrix[mr + x][mc + y].wcell, COLOR_PAIR(3));
				
					// add the MINE flag
					add_flag(&game->matrix[mr][mc], MINE);
				}
				else if(in_interval(mr + x, 0, game->rows - 1))
						if(in_interval(mc + y, 0, game->cols - 1)) {
							// count the mines in adjacent positions  
							game->matrix[mr + x][mc + y].adj_mines++;
						}				
	}

	// free memory
	for(i = 0; i < game->mines; i++)
		free(R[i]);

	free(R);	
}

int hit_cell(MS_GAME *game, int c_row, int c_col)
{
	int adj_mines;
	WINDOW *tmp = game->matrix[c_row][c_col].wcell;

	if(game->matrix[c_row][c_col].flags & HIT)
		return GAME_MULTIPLE_HIT;

	if(game->matrix[c_row][c_col].flags & (HIT | FLAG))
		return GAME_STOP;

	// first hit the selected cell and mark it
	add_flag(&game->matrix[c_row][c_col], HIT);
	mark_cell(&game->matrix[c_row][c_col], game->cell_size);
	game->hits++;

	// clean code
	adj_mines = game->matrix[c_row][c_col].adj_mines;

	// bomb? game over
	if(game->matrix[c_row][c_col].flags & MINE)
		return GAME_OVER;

	if(adj_mines == 0) {
		return GAME_OK;		/* hit adjacent cells */
	} 
	else if(adj_mines > 0) {
		mvwprintw(tmp, game->cell_size - 1, 0, "%d", adj_mines);
		wrefresh(tmp);

		return GAME_STOP;	/* don't hit adjacent cells */
	}

	return GAME_STOP;
}

void hit_adjacent(MS_GAME *game, int c_row, int c_col)
{
	int x, y, adj_mines;
	WINDOW *tmp = game->matrix[c_row][c_col].wcell;

	for(x = -1; x <= 1; x++)
		for(y = -1; y <= 1; y++) {
			if(!(x != 0 || y != 0))
				continue;

			if(!in_interval(c_row + x, 0, game->rows - 1)) 
				continue;

			if(!in_interval(c_col + y, 0, game->cols - 1))
				continue;

			// short lines
			adj_mines = game->matrix[c_row + x][c_col + y].adj_mines;

			if(adj_mines < 0) 
				continue;

			if(!(game->matrix[c_row + x][c_col + y].flags & (HIT | FLAG))) {
				// mark as hit
				add_flag(&game->matrix[c_row + x][c_col + y], HIT);

				// make the hit visible
				mark_cell(&game->matrix[c_row + x][c_col + y], game->cell_size);

				game->hits++;
				
				if(adj_mines > 0) {			
					// print the number of adjacent mines
					tmp = game->matrix[c_row + x][c_col + y].wcell;

					// no mine, no flag
					if(!(game->matrix[c_row + x][c_col + y].flags & MINE & FLAG)) {
						mvwprintw(tmp, game->cell_size - 1, 0, "%d", adj_mines);
						wrefresh(tmp);
					
						// a little animation
						usleep(15000);
					}
				}				
				
				if(adj_mines == 0) {
					// recursive hit
					hit_adjacent(game, c_row + x, c_col + y);
				}
			}
		}
}

int multiple_hit(MS_GAME *game, int c_row, int c_col)
{
	int x, y, flags = 0;
	for(x = -1; x <= 1; x++)
		for(y = -1; y <= 1; y++) {
			if(x == 0 && y == 0)
				continue;

			if(!in_interval(c_row + x, 0, game->rows - 1)) 
				continue;

			if(!in_interval(c_col + y, 0, game->cols - 1))
				continue;

			if(game->matrix[c_row + x][c_col + y].flags & FLAG)
				flags++;
		}
	
	if(flags != game->matrix[c_row][c_col].adj_mines || flags == 0)
		return GAME_CONTINUE;

	for(x = -1; x <= 1; x++)
		for(y = -1; y <= 1; y++) {
			if(x == 0 && y == 0)
				continue;
			
			if(!in_interval(c_row + x, 0, game->rows - 1)) 
				continue;

			if(!in_interval(c_col + y, 0, game->cols - 1))
				continue;

			if(hit_cell(game, c_row + x, c_col + y) == GAME_OVER)
				return GAME_OVER;
		}

	return GAME_CONTINUE;
}

int multiple_mark(MS_GAME *game, int c_row, int c_col)
{
	int x, y, cells = 0, flags = 0;
	for(x = -1; x <= 1; x++)
		for(y = -1; y <= 1; y++) {
			if(x == 0 && y == 0)
				continue;

			if(!in_interval(c_row + x, 0, game->rows - 1)) 
				continue;

			if(!in_interval(c_col + y, 0, game->cols - 1))
				continue;

			if(!(game->matrix[c_row + x][c_col + y].flags & HIT))
				cells++;
		}

	if(cells != game->matrix[c_row][c_col].adj_mines || cells == 0)
		return 0;	

	for(x = -1; x <= 1; x++)
		for(y = -1; y <= 1; y++) {
			if(x == 0 && y == 0)
				continue;

			if(!in_interval(c_row + x, 0, game->rows - 1)) 
				continue;

			if(!in_interval(c_col + y, 0, game->cols - 1))
				continue;

			if(!(game->matrix[c_row + x][c_col + y].flags & (HIT | FLAG))) {
				flags += toggle_flag(&game->matrix[c_row + x][c_col + y], FLAG);
				flag_cell(&game->matrix[c_row + x][c_col + y], game->cell_size);
			}
		}

	return flags;
}

int game_won(MS_GAME* game) 
{
	return game->hits + game->mines == game->rows * game->cols ? GAME_WON : GAME_CONTINUE;
}

void show_mines(MS_GAME* game)
{
	int i, j;
	for(i = 0; i < game->rows; i++)
		for(j = 0; j < game->cols; j++)
			if(game->matrix[i][j].flags & MINE) {
				wbkgd(game->matrix[i][j].wcell, COLOR_PAIR(3));
				wrefresh(game->matrix[i][j].wcell);
			}
}

void select_cell(MS_CELL *cell) 
{
	wbkgd(cell->wcell, COLOR_PAIR(8));
	wrefresh(cell->wcell);
}

void deselect_cell(MS_CELL *cell) 
{
	if(cell->flags & HIT) {
		if(cell->flags & MINE)
			wbkgd(cell->wcell, COLOR_PAIR(3));
		else
			wbkgd(cell->wcell, COLOR_PAIR(9));
	}
	else
		wbkgd(cell->wcell, COLOR_PAIR(11));
	
	wrefresh(cell->wcell);
}

void mark_cell(MS_CELL *cell, int cell_size) 
{
	// cell marked with a flag	
	flag_cell(cell, cell_size);

	if(cell->flags & HIT)
		wbkgd(cell->wcell, COLOR_PAIR(9));

	if(cell->flags & MINE & HIT) 
		wbkgd(cell->wcell, COLOR_PAIR(3));

	wrefresh(cell->wcell);
}

void flag_cell(MS_CELL *cell, int cell_size)
{
	wbkgd(cell->wcell, COLOR_PAIR(11));

	if(cell_size >= 2)
		mvwprintw(cell->wcell, 0, 2 * cell_size - 3, cell->flags & FLAG ? "[F]" : "   ");
	else
		mvwprintw(cell->wcell, 0, 2 * cell_size - 1, cell->flags & FLAG ? "F" : " ");

	wrefresh(cell->wcell);
}

// flags can be OR-ed
void add_flag(MS_CELL *cell, enum FLAGS flag)
{
	cell->flags |= flag; 
}

void del_flag(MS_CELL *cell, enum FLAGS flag) 
{
	cell->flags &= ~flag; 
}

int toggle_flag(MS_CELL *cell, enum FLAGS flag) 
{
	if(cell->flags & flag) {
		del_flag(cell, flag);
		return -1;
	}
	
	add_flag(cell, flag);
	return 1;
}
