#include "../lib/my.h"
#include <stdio.h>
#include <mynotsominishell.h>
#include <unistd.h>
//#define BUF_SZ 256

int main()
{


	int n;
	prompt = "$ : "
	int i;
    char* buffer;
    buffer = malloc(sizeof(char) * (READMIN+1));
    
    if(argc < 2){
        my_str("not enough args");
        return 0;
    }
    gl_env.elements = malloc( (argc-1)*sizeof(t_elem));
    gl_env.nbelems = argc - 1;
    for(i=1; i<argc; i++){
        //i is starting at 1 because 0 of argv is program name
        t_elem *current_elem = gl_env.elements+(i-1);
        current_elem->elem = my_strdup(argv[i]);
        current_elem->size = my_strlen(argv[i]);
        current_elem->mode = 0;


    }
    init_terminal();
    signal(SIGINT, quit_program);
    signal(SIGWINCH, show_elems);
    init_caps();
    term_hide_cursor();
    show_elems();
    
    int j = 8;
    while(1){
        int n = read(0, buffer, READMIN);
        if(!n){
            continue;
        }
        buffer[n]='\0';
//        if(!gl_env.flag){
            //if screen showed successfully
        if(!my_strcmp(buffer, KU)){
            previous_command();
        }
        else if(!my_strcmp(buffer,KD)){
            next_command();
        }
        else if(!my_strcmp(buffer,KL)){
            moveleft();
        }
        else if(!my_strcmp(buffer,KR)){
            moveright();
        }
     /*   else if(!my_strcmp(buffer," ")){
            doselect();
        }*/
//      }
        else if(n==1 && buffer[0]==ESC){
            quit_program();
        }
        else if(n==1 && buffer[0]=='\n'){
            my_exec(gl_env.current_cmd);
        }
        //my_str(buffer);
    }
    restore_terminal();
    //Initialize terminal
}


int my_exec(char *cmd){
	char ** vect;
	pid_t pid;
	my_char('\n');
	if ( my_strcmp(cmd, "exit") == 0 )	
	{
		my_str("\nOh, so you prefer the other shell? \n");
		my_str("Goodbye. \n");
		exit(0);
	}
	
	vect = my_str2vect(cmd);
	
	if ( my_strcmp(vect[0], "cd" ) == 0 )
	{
		chdir(vect[1]);
	}
	else
	{
	    if( ( pid=fork () ) < 0 )
		{
    		my_str ("PID less than zero \n");
			exit(0);
		}
		else if( pid == 0 )
		{
			if ( execvp(vect[0], vect) < 0 )
			{
				my_str("Execution problem \n");
				exit(0);
			}
		}
	    else
		{
			wait();
		}
	}
}

//this accepts nothing and initializes the terminal in a custom io mode
void init_terminal(){
    char* term;
    char* name;
    int fd;
    struct termio line;
    term = getenv("TERM");
    char bp[2048];
    tgetent(bp, term);
    ioctl(0, TCGETA, &line);
    if(!term){
        my_str("BAD TERM! PANIC!");
        exit(0);
    }
    gl_env.line_backup = line;
    line.c_cc[VMIN] = READMIN;
    line.c_cc[VTIME] = READTIME;
    line.c_lflag &= (~(ICANON|ECHO|ISIG));
    ioctl(0, TCSETA, &line);
    ioctl(0, TIOCGWINSZ, &(gl_env.win));
    name = ttyname(0);
    fd = open(name, O_WRONLY);
    gl_env.stdio_backup = dup(1);
    dup2(fd, 1);
}
//this function gets strings for termcaps I need to store in gl_env
void init_caps(){

    gl_env.clear = term_get_cap(CLEAR);
    gl_env.move = term_get_cap(MOVE);
    gl_env.left = term_get_cap(LEFT);
    //gl_env.right = term_get_cap(RIGHT);
    //gl_env.up = term_get_cap(UP);
    //gl_env.down = term_get_cap(DOWN);
    //gl_env.esc = term_get_cap(ESC);
    gl_env.underline = term_get_cap(UNDERLINE);
    gl_env.under_end = term_get_cap(UNDER_END);
    gl_env.standout = term_get_cap(STANDOUT);
    gl_env.stand_end = term_get_cap(STAND_END);
    gl_env.cursoroff = VECAP;
    gl_env.cursoron = VICAP;
}
//this function restores the terminal to the way I elft it before init terminal was called
void restore_terminal(){
    term_clear();
    ioctl(0, TCSETA, &gl_env.line_backup);
    //my_str("RESTORED");
    //how to restore window thing?
    tputs(VECAP, 1, my_termprint);
    dup2(gl_env.stdio_backup,1);
}
//this function accepts a string and returns the string for the termcap used
char *term_get_cap(char* cap){

    char area[2048];
    char *str;
    str = tgetstr(cap ,(char**)(&area));
    if(!str){
        my_str("bad cal");

    }
    return str;

}
//this accepts a string, stops the program, and returns
void panic(char* str){
    restore_terminal();
    my_str(str);
    exit(0);
}
//this quits the program. gracefully
void quit_program(){
    restore_terminal();
    exit(0);
}

int my_termprint(int c){
    return write(1, &c, 1);
}

void term_move_cursor(int column, int row){
    tputs(tgoto(gl_env.move, column, row), 1, my_termprint);
}
//this shows cursor
void term_hide_cursor(){
    tputs(VICAP, 1, my_termprint);
}
//this starts a highlight
void term_stand_out(){
    tputs(gl_env.standout, 1, my_termprint);
}
//this ends highlighting
void term_stand_end(){
    tputs(gl_env.stand_end, 1, my_termprint);
}
//this clears the terminal
void term_clear(){
    tputs(gl_env.clear, 1, my_termprint);
}