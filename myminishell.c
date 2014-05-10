#include "../lib/my.h"
#include <stdio.h>
#include "./mynotsominishell.h"
#include <unistd.h>
//#define BUF_SZ 256

int main()
{


	int n;
	int i;
    char* buffer;
    gl_env.prompt = "$ : ";
    gl_env.current_cmd = malloc(1);
    gl_env.current_cmd[0] = '\0';
    gl_env.cmd_i = 0;
    buffer = malloc(sizeof(char) * (READMIN+1));
    
    /*if(argc < 2){
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


    }*/
//    return 1;
    init_terminal();
    signal(SIGINT, quit_program);
    //signal(SIGWINCH, show_elems);
    //INIT CAPS MAKES ME SEGFAULT AND I DONT KNOW WHY
    init_caps();
    /*restore_terminal();
    return 0;*/
    //term_hide_cursor();
    //show_elems();
    
    int j = 8;
    my_str("\n");
    my_str(gl_env.prompt);
    while(1){
        int n = read(0, buffer, READMIN);
        if(!n){
            continue;
        }
        buffer[n]='\0';
//        if(!gl_env.flag){
            //if screen showed successfully
        if (buffer[0] >= 'A' && buffer[0] <= 'Z' || buffer[0] >= 'a' && buffer[0] <= 'z' || buffer[0]>='0' && buffer[0]<='9'){

//            my_char(buffer[0]);
            update_current_command(buffer[0],gl_env.cmd_i);
            gl_env.cmd_i++;
        }
        else if(!my_strcmp(buffer, KU)){
            //previous_command();
        }
        else if(!my_strcmp(buffer,KD)){
            //next_command();
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
            my_str("quitting program");
            quit_program();
        }
        else if(n==1 && buffer[0]=='\n'){
            my_exec(gl_env.current_cmd);
            gl_env.current_cmd = NULL;
            gl_env.cmd_i = 0;
            my_str("\n");
            my_str(gl_env.prompt);
        }
        //my_str(buffer);
    }
    restore_terminal();
    //Initialize terminal
}
void update_current_command(char c, int i){
    char * new_cmd;
    //my_str("\nAdding ");
    my_char(c);
    //my_str(" At: ");
    //my_int(i);
    //my_str(" to: ");
    if(!gl_env.current_cmd){
        gl_env.current_cmd = xmalloc(1);
        gl_env.current_cmd[0] = '\0';
    }
    //my_str(gl_env.current_cmd);
    //my_str("This should be 2:");

    if(i <= my_strlen(gl_env.current_cmd)){
        if(i==my_strlen(gl_env.current_cmd)){
    //        my_int(my_strlen(gl_env.current_cmd) + 2);
            new_cmd = xmalloc(my_strlen(gl_env.current_cmd)+2);
            my_strcpy(new_cmd,gl_env.current_cmd);
            new_cmd[my_strlen(gl_env.current_cmd) + 1]= '\0';
            new_cmd[my_strlen(gl_env.current_cmd)]= c;
            //my_str("\nNew size = ");
     //       my_int(my_strlen(gl_env.current_cmd)+2);
            free(gl_env.current_cmd);
            gl_env.current_cmd = new_cmd;
//            my_str(new_cmd);
      //      my_char('\0');
            return;
        }
        gl_env.current_cmd[i] = c;

    }else{
        my_str("something went wrong");
    }
}


int my_exec(char *cmd){
	char ** vect;
	pid_t pid;
	my_char('\n');
    if(!cmd){
        my_str("Try typing a command before pressing enter.");
        return;
    }
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
                my_str(cmd);
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
    //term_clear();
    ioctl(0, TCSETA, &gl_env.line_backup);

    my_str("RESTORED");
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
void moveright(){
    //int cmd_len = my_strlen(gl_env.current_cmd);
    //length of line TO cursor
    
   /* int max_x = gl_env.win.ws_col;
    int line_len = gl_env.cmd_i + my_strlen(gl_env.prompt);
    if(gl_env.cmd_i < my_strlen(gl_env.current_cmd)){
        //if can move left
        if(line_len/gl_env. 
    }*/
}
void moveleft(){}

//this accepts a string, stops the program, and returns
void panic(char* str){
    restore_terminal();
    my_str(str);
    exit(0);
}
//this quits the program. gracefully
void quit_program(){
    //restore_terminal();
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
