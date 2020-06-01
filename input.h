/*
 * This software is published under GNU General Public License v3.0.
 * For more information see LICENSE
*/
#ifndef INPUT_H
#define INPUT_H

#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void 
disable_raw_mode()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void 
enable_raw_mode()
{
	tcgetattr(STDIN_FILENO, &orig_termios);
	atexit(disable_raw_mode);
	struct termios raw = orig_termios;
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

#endif /* INPUT_H */
