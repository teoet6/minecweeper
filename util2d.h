/*
 * This software is published under GNU General Public License v3.0.
 * For more information see LICENSE
*/
#ifndef UTIL2D_H
#define UTIL2D_H

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

void 
move(int x, int y)
{
	printf("\033[%d;%dH", y, x);
}

void 
clear()
{
	move(1, 1);
	printf("\033[2J");
}

int 
term_y()
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_row;
}

int 
term_x()
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
}

void 
cursor_hide()
{
	printf("\033[?25l");
}

void 
cursor_show()
{
	printf("\033[?25h");
}

typedef enum {black, lgray, gray, red, green, yellow, blue, purple, cyan, white} Color;
void 
text_color(Color c)
{
	switch(c){
	case black:	printf("\033[30m"); break;
	case lgray:	printf("\033[37m"); break; 
	case gray:	printf("\033[90m"); break;
	case red:	printf("\033[91m"); break;
	case green:	printf("\033[92m"); break;
	case yellow:	printf("\033[93m"); break;
	case blue:	printf("\033[94m"); break;
	case purple:	printf("\033[95m"); break;
	case cyan:	printf("\033[96m"); break;
	case white:	printf("\033[97m"); break;
	}
}

typedef enum {normal, bold, italic, underline, striketrough} Style;
void
text_style(Style s){
	return;
}

#endif /* UTIL2D_H */
