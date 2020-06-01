/*
 * This software is published under GNU General Public License v3.0.
 * For more information see LICENSE
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include "util2d.h"
#include "input.h"

#define MAX_ROWS 128
#define MAX_COLS 128

#define DEFAULT_ROWS 20
#define DEFAULT_COLS 20

typedef struct {
	enum {covered='-', uncovered='\0', flagged='X'} state;
	enum {empty='\0', bomb='O'} guts;
} Cell;

int rows, cols, bombs;
int correct_flags;
int origin_x = 1, origin_y = 1;
int cursor_x, cursor_y;
Cell field[MAX_ROWS][MAX_COLS];
enum {playing, lost, won} gamestate;

void
gameexit()
{
	clear();
	text_color(lgray);
}

void 
gameinit()
{
	gamestate = playing;
	cursor_x = 0;
	cursor_y = 0;
	bombs = 0;
	correct_flags = 0;
	for(int i=0;i<rows;i++){
		for(int j=0;j<cols;j++){
			field[i][j].state = covered; 
			if(rand() % 10){
				field[i][j].guts = empty;
			}else{
				field[i][j].guts = bomb;
				bombs++;
			}
		}
	}
}	

void 
cursor_move(int dir)
{
	switch(dir % 4){
	case 0:
		cursor_x = cursor_x - 1 >= 0 ? cursor_x - 1 : 0; break;
	case 1:
		cursor_y = cursor_y - 1 >= 0 ? cursor_y - 1 : 0; break;
	case 2:
		cursor_x = cursor_x + 1 < cols ? cursor_x + 1 : cols-1; break;
	case 3:
		cursor_y = cursor_y + 1 < rows ? cursor_y + 1 : rows-1; break;
	}
}

void 
flag(int x, int y)
{
	switch(field[y][x].state){
		case uncovered: break;
		case covered:
			field[y][x].state = flagged;
			if(field[y][x].guts == bomb) correct_flags++;
			break;
		case flagged:
			field[y][x].state = covered;
			if(field[y][x].guts == bomb) correct_flags--;
			break;
	} 
}

int 
count_neigh(int x, int y)
{
	int ans = 0;
	for(int i=-1;i<=1;i++){
		for(int j=-1;j<=1;j++){
			if(y+i < 0 || y+i >= rows) continue;
			if(x+j < 0 || x+j >= cols) continue;
			if(i == 0 && j == 0) continue;
			ans += field[y+i][x+j].guts == bomb;
		}
	}
	return ans;
}

int
soft_uncover(int x, int y)
{
	if(x < 0 || x >= cols || y < 0 || y >= rows) return 1;
	if(field[y][x].state == flagged) return 1;
	field[y][x].state = uncovered;
	if(field[y][x].guts == bomb){
		gamestate = lost;
		return 1;
	}
	return 0;
}

void 
recursive_uncover(int x, int y)
{
	Cell original = field[y][x];
	if(soft_uncover(x, y)) return;
	if(original.state == covered && count_neigh(x, y) == 0){
		recursive_uncover(x-1, y+1);
		recursive_uncover(x-1, y-1);
		recursive_uncover(x-1, y  );
		recursive_uncover(x+1, y+1);
		recursive_uncover(x+1, y-1);
		recursive_uncover(x+1, y  );
		recursive_uncover(x  , y+1);
		recursive_uncover(x  , y-1);
	}
}

void
bulk_uncover(int x, int y)
{
	recursive_uncover(x-1, y+1);
	recursive_uncover(x-1, y-1);
	recursive_uncover(x-1, y  );
	recursive_uncover(x+1, y+1);
	recursive_uncover(x+1, y-1);
	recursive_uncover(x+1, y  );
	recursive_uncover(x  , y+1);
	recursive_uncover(x  , y-1);
	recursive_uncover(x  , y  );
}

void 
uncover(int x, int y)
{
	if(x < 0 || x >= cols || y < 0 || y >= rows) return;
	switch(field[y][x].state){
	case uncovered: bulk_uncover(x, y); break;
	case covered: recursive_uncover(x, y); break;
	}
}

Color 
calc_color(Cell c, int neigh)
{
	switch(c.state){
	case covered: return lgray;
	case flagged: return red;
	case uncovered:
		switch(c.guts){
		case bomb: return gray;
		case empty:
			switch(neigh){
			case 0: return lgray;
			case 1: return blue;
			case 2: return green;
			case 3: return red;
			case 4: return blue;
			case 5: return red;
			case 6: return cyan;
			case 7: return gray;
			case 8: return lgray;
			}
		}
	}
}	

char
calc_char(Cell c, int neigh){
	switch(c.state){
	case covered: return '-';
	case flagged: return 'X';
	case uncovered:
		switch(c.guts){
		case bomb: return 'O';
		case empty: return neigh == 0 ? ' ' : '0' + neigh;
		}
	}
}

void 
print_cell(Cell c, int neigh)
{
	text_color(calc_color(c, neigh));
	printf("%c", calc_char(c, neigh));
}

void
print_controls(int x, int y)
{
	text_color(lgray);
	move(x, y);	
	printf("hjkl - move cursor");
	move(x, y+1);
	printf("u - uncover a cell");
	move(x, y+2);
	printf("f - flag a cell");
	move(x, y+3);
	printf("q - quit minecweeper");
	move(x, y+4);
	printf("n - begin a new game of minecweeper");
	move(x, y+5);
	printf("$ minecweeper <rows> <columns> - specify the dimensions of the field");
}

void 
gameloop()
{
	unsigned char in=' ';
	do{
		clear();
		if(iscntrl(in)){
			continue;
		}
		switch(gamestate){
			case playing:
				switch(in){
					case 'h': cursor_move(0); break; /* Left */
					case 'j': cursor_move(3); break; /* Down */
					case 'k': cursor_move(1); break; /* Up */
					case 'l': cursor_move(2); break; /* Right */
					case 'f': flag(cursor_x, cursor_y); break; 
					case 'u': uncover(cursor_x, cursor_y); break; 
				}
				/* FALLTROUGH */
			default:
				switch(in){
					case 'q': exit(0);
					case 'n': return;
				}
		}
		if(correct_flags == bombs) gamestate = won;
		clear();
		for(int i=0;i<rows;i++){
			for(int j=0;j<cols;j++){
				move(origin_x+j, origin_y+i);
				int neigh = count_neigh(j, i);
				print_cell(field[i][j], neigh);
			}
		}
		print_controls(origin_x + cols + 1, origin_y);
		switch(gamestate){
			case playing:
				text_color(lgray);
				move(origin_x, origin_y + rows + 1);
				printf("There are %d bombs in this game", bombs);
				break;
			case lost:
				text_color(red);
				move(origin_x, origin_y + rows + 1);
				printf("You lost. :^C");
				break;
			case won:
				text_color(green);
				move(origin_x, origin_y + rows + 1);
				printf("YOU WON! :^D");
				break;
		}
		move(origin_x + cursor_x, origin_y + cursor_y);
		fflush(stdout);
	}while(read(STDIN_FILENO, &in, 1) == 1);
}

void 
print_usage()
{
	printf("The program can be ran with zero or two arguments\n");
	printf("First argument - the number of columns\n");
	printf("Second argument - the number of rows\n");
	exit(0);
}

	int 
is_pos_dec(char * s)
{
	for(int i=0;s[i]!='\0';i++){
		if(! (s[i] >= '0' && s[i] <= '9'))
			return 0;
	}
	return 1;
}

	int 
main(int argc, char* argv[])
{
	switch(argc){
		case 1:
			rows = DEFAULT_ROWS;
			cols = DEFAULT_COLS;
			break;
		case 3:
			if(!is_pos_dec(argv[1])) print_usage();
			if(!is_pos_dec(argv[2])) print_usage();
			rows = strtol(argv[1], NULL, 10);
			rows = rows > MAX_ROWS ? DEFAULT_ROWS : rows;
			cols = strtol(argv[2], NULL, 10);
			cols = cols > MAX_COLS ? DEFAULT_COLS : cols;
			break;
		default:
			print_usage();
	}
	srand(time(0));
	atexit(gameexit);
	enable_raw_mode();
	while(1){
		gameinit();
		gameloop();
	}
}
