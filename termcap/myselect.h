#ifndef _MYSELECT_H_
#define _MYSELECT_H_

#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/termios.h>
#include "../lib/my.h"
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <termcap.h>

#define READTIME 1
/* used to return from read after 1/10th of  second passed from the first byte read */

#define READMIN 3
/* used to return from min if 3 bytes have been read. Needed for things like characters which are \E[A B C or D */

#ifndef NULL
#define NULL (0)
#endif

#define KU "\E[A"
#define KD "\E[B"
#define KR "\E[C"
#define KL "\E[D"
#define UP "ku"
#define DOWN "kd"
#define LEFT "kl"
#define RIGHT "kr"
#define CLEAR "cl"
#define MOVE "cm"
#define STANDOUT "so"
#define STAND_END "se"
#define UNDERLINE "us"
#define UNDER_END "ue"
#define CURSOROFF "vi"
#define CURSORON "ve"
#define VECAP "\E[?25h" // actual string for VE in case tgetstr doesn't find it
#define VICAP "\E[?25l" // same as above but for VI
#define ESC 27

typedef struct	s_elem
{
    char	*elem;
    int		size;
    int		x;
    int		y;
    int		mode;
}		t_elem;

typedef struct	s_env
{
    char	*left;
    char	*right;
    char	*up;
    char	*down;
    char	*esc;
    char	*underline;
    char	*under_end;
    char	*standout;
    char	*stand_end;
    char	*move;
    char	*clear;
    char	*cursoroff;
    char	*cursoron;
    struct winsize win;
    struct termio line_backup;
    int	    flag;
    int		stdio_backup;
    int		nbelems;
    int		pos;
    t_elem	*elements;
}		t_env;

t_env	gl_env;
// created as a structure to limit number of globals to 1
void	init_terminal();
void	restore_terminal();
void    return_program();
char	*term_get_cap(char*);
void    panic(char*);
int     my_termprint(int);
void	init_caps();
void    term_move_cursor(int row, int column);
void    term_hide_cursor();
void	term_clear();
void	term_vi();
void	term_ve();
char	check_character(char *c);
char	get_win_size();
void	show_elems();
void	refreshin();
void	refreshout(int);
void	moveup();
void	movedown();
void	moveleft();
void	moveright();
void	doselect();
/*
void	getout(char);
void	setup_elems(int, char**);
char	check_char(char*);*/
void	term_move_to_item(int);
void    move_cursor_to_item(int);
void    quit_program();
//void	term_underline();
void	term_standout();
void	term_standend();/*
void	term_underend();*/
#endif
