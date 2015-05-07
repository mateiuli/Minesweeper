#ifndef __INTROSTATE_H__
#define __INTROSTATE_H___

#include <ncurses.h>
#include <sys/ioctl.h>
#include <linux/kd.h>

void play_intro_state();
void destroy_intro(WINDOW*);

#endif