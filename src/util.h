#ifndef __UTIL_H__
#define __UTIL_H__

#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
	char row, col;
} Point;

Point **random_coordinates(int, int, int);
void print_in_middle(WINDOW*, char*);
void loading_animation(WINDOW*, int);
void print_splash_screen(WINDOW*, int);
void sprintf_current_datetime(char*);
void shuffle_array(int*, int);
int in_interval(int, int, int);
int get_random(int, int);
int st_hours(int);
int st_minutes(int);
int st_seconds(int);

#endif