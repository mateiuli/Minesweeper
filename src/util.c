#include "util.h"

void print_in_middle(WINDOW *win, char *str)
{
	if(win == NULL)
		win = stdscr;

	int maxx, maxy, begx, begy, length;

	// window's limits and size
	getmaxyx(win, maxy, maxx);
	getbegyx(win, begy, begx);

	// a little bit of magic
	length = strlen(str);
	mvwprintw(win, (maxy - begy)/2, (maxx - begx - length)/2, str);
	wrefresh(win);
}

// simluare animatie de incarcare :)
void loading_animation(WINDOW *intro, int animated) 
{
	int i;

	// background color on
	wattron(intro, COLOR_PAIR(11));

	for(i = 0; i < LINES; i++) {
		mvwprintw(intro, i, 0, " ");
		mvwprintw(intro, LINES - 1 - i, COLS - 1, " ");	
		wrefresh(intro);
		if(animated)
			usleep(5000);
	}

	for(i = 0; i < COLS; i++) {
		mvwprintw(intro, LINES - 1, i, " ");
		mvwprintw(intro, 0, COLS - 1 - i, " ");	
		wrefresh(intro);
		if(animated)
			usleep(5000);
	}

	// background color off
	wattroff(intro, COLOR_PAIR(13));
}

void print_splash_screen(WINDOW *intro, int animated) 
{
	// doing some magic trick to print it in the middle of the screen (max line length = 60)
	int x_pos = (COLS - 60)/2;

	mvwprintw(intro, 5, x_pos, "  /\\/\\ (_)_ __   ___  _____      _____  ___ _ __   ___ _ __ ");
	if(animated) {
		wrefresh(intro); usleep(100000);
	}
	
	mvwprintw(intro, 6, x_pos, " /    \\| | '_ \\ / _ \\/ __\\ \\ /\\ / / _ \\/ _ \\ '_ \\ / _ \\ '__|");
	if(animated) {
		wrefresh(intro); usleep(100000);
	}
	
	mvwprintw(intro, 7, x_pos, "/ /\\/\\ \\ | | | |  __/\\__ \\\\ V  V /  __/  __/ |_) |  __/ |   ");
	if(animated) {
		wrefresh(intro); usleep(100000);
	}
	
	mvwprintw(intro, 8, x_pos, "\\/    \\/_|_| |_|\\___||___/ \\_/\\_/ \\___|\\___| .__/ \\___|_|   ");
	if(animated) {
		wrefresh(intro); usleep(100000);
	}

	mvwprintw(intro, 9, x_pos, "                                           |_|              ");
	wrefresh(intro); usleep(100000);
}

int in_interval(int val, int min, int max)
{
	return val >= min && val <= max;
}

void shuffle_array(int *array, int n) 
{
	static int counter = 0;
	if (n > 1) 
    {
    	srand(time(NULL) + 3600 * counter++);
        int i, t;
        for (i = 0; i < n - 1; i++) 
        {
          int j = i + rand() / (RAND_MAX / (n - i) + 1);
          t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

Point **random_coordinates(int rows, int cols, int mines) 
{
	/**
	* 	Implementing 'Reservoir sampling' algorithm for randomly  
	*	choosing a sample of k items from a list S containing n items.
	*	
	*	Each item in S has equal probability of being chosen for the reservoir
	*/
	int i, j, n = 0;
	Point **S = (Point**)malloc(rows * cols * sizeof(Point*));
	
	// gnerating the list of elements
	for(i = 0; i < rows; i++)
		for(j = 0; j < cols; j++) {
			// the cell from center cannot be a mine
			if(i == rows/2 && j == cols/2)
				continue;

			S[n] = (Point*)malloc(sizeof(Point));
			S[n]->row = i;
			S[n++]->col = j;
		}

	// creating the reservoir
	Point **R = (Point**)malloc(mines * sizeof(Point*));

	// fill the reservoir array
	for(i = 0; i < mines; i++) {
		// copy - free the memory later
		R[i] = (Point*)malloc(sizeof(Point));
		R[i]->row = S[i]->row;
		R[i]->col = S[i]->col;

	}

	// replace elements - decreasing probability
	for(i = mines; i < n; i++) {
		j = get_random(0, i);
		if(j < mines) {
			R[j]->row = S[i]->row;
			R[j]->col = S[i]->col;	
		}
	}

	// it's nice to clean after ourselves (:
	for(i = 0; i < n; i++)
		free(S[i]);

	free(S);

	return R;
}

int get_random(int start, int end)
{
	static int counter = 0;
	
	// should be enough
	srand(time(NULL) + counter++ * 3600);

	return (start + rand() % (end - start));
}

void sprintf_current_datetime(char *str)
{
	// date and time
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(str, 80, "%F %T", timeinfo);
}

/* 	
*	Those 3 functions convert seconds 
*	into hours:minutes:seconds
*/
int st_hours(int seconds) 
{
	return seconds / 3600;
}

int st_minutes(int seconds)
{
	return (seconds / 60) % 60;
}

int st_seconds(int seconds)
{
	return seconds % 60;
}