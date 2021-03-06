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
    loadhist();
    gl_env.history_current = -1;
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
    // signal(SIGINT, quit_program);
    signal(SIGWINCH, redraw_page);
    //INIT CAPS MAKES ME SEGFAULT AND I DONT KNOW WHY
    init_caps();
//    restore_terminal();
//    my_str("about to return");
//    return 0;
    /*restore_terminal();
    return 0;*/
    //term_hide_cursor();
    //show_elems();
    
    int j = 8;
    term_clear();
    //my_int(gl_env.win.ws_col);
    //my_str("\n");
    my_str(gl_env.prompt);
    while(1){
        int n = read(0, buffer, READMIN);
        if(!n){
            continue;
        }
        buffer[n]='\0';
//        if(!gl_env.flag){
            //if screen showed successfully
//        my_int(buffer[0]);
        if (buffer[0] == ' ' || buffer[0] >= 'A' && buffer[0] <= 'Z' || buffer[0] >= 'a' && buffer[0] <= 'z' || buffer[0]>='0' && buffer[0]<='9'){

//            my_char(buffer[0]);
            update_current_command(buffer[0],gl_env.cmd_i);
            gl_env.cmd_i++;
        }
        else if(buffer[0] == CTRLK){
            copy_from(gl_env.cmd_i);
            delete_from(gl_env.cmd_i);
        }
        else if(buffer[0] == CTRLY){
            //my_str(gl_env.clipboard);
            if(gl_env.clipboard){
                insert_at(gl_env.clipboard, gl_env.cmd_i);
            }
        }
        else if(buffer[0] == CTRLA){
            int k = gl_env.cmd_i;
            while(k--){
                moveleft();
            }
        } 
        else if(buffer[0] == '\t'){
            clear_line();
        }
        else if(buffer[0] == CTRLE){
            //move to end of line
            int k = my_strlen(gl_env.current_cmd) - gl_env.cmd_i;
            while(k--){
                moveright();
            }
            
        }
        else if(buffer[0] == CTRLL){
            term_clear();
            rewrite_line(-1);
        }
        else if(buffer[0] == 127 || buffer[0] == '\b' || buffer[0]==0x08){
 //           my_str("DEEEELEEEEETE");
            delete_char(gl_env.cmd_i-1);
        }
        else if(!my_strcmp(buffer, KU)){
            older_history();
        }
        else if(!my_strcmp(buffer,KD)){
            newer_history();
        }
        else if(!my_strcmp(buffer,KL)){
//            my_str("strlen legt!");
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
			addtohist(gl_env.current_cmd);
            my_exec(gl_env.current_cmd);
           // gl_env.current_cmd = 
            gl_env.current_cmd = malloc(1);
            gl_env.current_cmd[0] = '\0';

            gl_env.cmd_i = 0;
            my_str("\n");
            my_str(gl_env.prompt);
        }else{

            update_current_command(buffer[0],gl_env.cmd_i);
            gl_env.cmd_i++;
        }
        //my_str(buffer);
    }
    restore_terminal();
    //Initialize terminal
}
//backspace
//deletes character at index i
void delete_char(int i){
    //my_int(i);
    //delete char and does stuff
    int temp = i;
    //if(i>my_strlen(gl_env.current_cmd)){
    //my_str("DEEEEEELEEEEEEEEEETING");
    if(i<0){
        return;
    }
    clear_line();
    while(i<my_strlen(gl_env.current_cmd)){
        gl_env.current_cmd[i] = gl_env.current_cmd[i+1];
        i++;
    }
    my_str(gl_env.current_cmd);
    gl_env.cmd_i = my_strlen(gl_env.current_cmd);
    while(temp++<my_strlen(gl_env.current_cmd)){
        moveleft();
    }
}

//deletes line, rewrites prompt
void clear_line(){
    //clear line
    /*if(!gl_env.current_cmd){
        return;
    }*/
//    my_int(gl_env.cmd_i);
   // return;
    int i=gl_env.cmd_i;
    while(i--){
        moveleft();
    }
    i = my_strlen(gl_env.prompt);
    while(i--){
        curs_left();
    }
    i = my_strlen(gl_env.prompt)+(gl_env.current_cmd?my_strlen(gl_env.current_cmd):0);
    while(i-->0){
        my_char(' ');
    }
    gl_env.cmd_i = (gl_env.current_cmd?(1+my_strlen(gl_env.current_cmd)):1);
    //move to beginning of line 
    //i = my_strlen(gl_env.current_cmd)+1;
    i = gl_env.cmd_i;
    while(i-- > 0){
        moveleft();
    }
    i = my_strlen(gl_env.prompt) + 1;
    while(i--){
        curs_left();
    }
    

    //delete_from(0);
    //write prompt
    my_str(gl_env.prompt);
    //my_str("AAAAAAAAAAAAAAAAAAAAAA");
    gl_env.cmd_i = 0;
}
//rewrites line and places cursor at index i. if -1 goes to end of line
//just rewrites entire line
void rewrite_line(int i){
//    ioctl(0, TIOCGWINSZ, &(gl_env.win));
    clear_line();
    my_str(gl_env.current_cmd);
    gl_env.cmd_i = my_strlen(gl_env.current_cmd);
    if(i<0){
        return;
    }
    while(i++<my_strlen(gl_env.current_cmd)){
        moveleft();
    }
}

//update current command by placing char c at index i. if i is too big, expand current command
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

//executes cmd as a shell command
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
        quit_program();
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
				my_str("Bad Command \n");
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
    my_int(gl_env.win.ws_col);
    name = ttyname(0);
    fd = open(name, O_WRONLY);
    gl_env.stdio_backup = dup(1);
    dup2(fd, 1);
    //margins
    //tputs(gl_env.auto_wrap, 1, my_termprint);
    tputs(gl_env.left_wrap, 1, my_termprint);
}
//this function gets strings for termcaps I need to store in gl_env
void init_caps(){

    gl_env.clear = term_get_cap(CLEAR);

    gl_env.move = term_get_cap(MOVE);
    gl_env.left = term_get_cap(LEFT);
    gl_env.curs_left = term_get_cap("le");
    gl_env.curs_right = term_get_cap("nd");
    gl_env.curs_up = term_get_cap("up");
    gl_env.curs_down = term_get_cap("do");
//    gl_env.auto_wrap = term_get_cap("am");
//    gl_env.left_wrap = term_get_cap("bw");
    //might ot be specifieid on linux machine
    gl_env.beg_line = term_get_cap("cr");

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

//    my_str("RESTORED");
    //how to restore window thing?
    tputs(VECAP, 1, my_termprint);
    dup2(gl_env.stdio_backup,1);
}
//this function accepts a string and returns the string for the termcap used
char *term_get_cap(char* cap){

    char* area = malloc(2048);
    char* str;

    //return 0;
    str = tgetstr(cap ,(char**)(&area));
    if(!str){
        my_str("bad call on");
        my_str(cap);

    }
    //return 0;
    return str;

}
//the next two functions don't work but im scared to delete it and it's 4 AM
int get_curs_x(){
    int max_x = gl_env.win.ws_col;
    int line_len = gl_env.cmd_i + my_strlen(gl_env.prompt);
    return line_len % max_x;
}
int get_curs_y(){
    int max_x = gl_env.win.ws_col;
    int line_len = gl_env.cmd_i + my_strlen(gl_env.prompt);
    return gl_env.line_number + line_len / max_x;
    
}
//move cursor right. If at end of command, do nothing. Wrap at end of line
void moveright(){
//    sleep(1);
    //move_end();
    //return;
    if(gl_env.cmd_i < my_strlen(gl_env.current_cmd)){
        gl_env.cmd_i++;
        int size_line = gl_env.cmd_i + my_strlen(gl_env.prompt);
        //term_move_cursor(get_curs_x(), get_curs_y());
        //if(!(gl_env.cmd_i % gl_env.win.ws_col)){
        if(!(size_line % gl_env.win.ws_col)){
            //if need to move down a line
            //tputs(gl_env.curs_down, 1, my_termprint);
            //my_str("IT HAPPENED");
            curs_down();
            move_beg();

        }else{
            curs_right();
            //tputs(gl_env.curs_right, 1, my_termprint);
        }
    }
    //int cmd_len = my_strlen(gl_env.current_cmd);
    //length of line TO cursor
    
   /* int max_x = gl_env.win.ws_col;
    int line_len = gl_env.cmd_i + my_strlen(gl_env.prompt);
    if(gl_env.cmd_i < my_strlen(gl_env.current_cmd)){
        //if can move left
        if(line_len/gl_env. 
    }*/
}
//
//move cursor left. if at beginning of command, do nothing. wrap at beginning of line
void moveleft(){
    //move_beg();
 //   sleep(1);
    if(gl_env.cmd_i > 0){
        int size_line = gl_env.cmd_i + my_strlen(gl_env.prompt);
        gl_env.cmd_i--;
        //if((gl_env.win.ws_col-1)==(gl_env.cmd_i % gl_env.win.ws_col)){
        if(!(size_line % gl_env.win.ws_col)){
            //if need to move up a line
//            my_str("THIS SHOULD HAPPEN");
            curs_up();
            move_end();
        }else{
            //tputs(gl_env.curs_left, 1, my_termprint);
            //curs_down();
            curs_left();
        }
        //term_move_cursor(get_curs_x(), get_curs_y());
    }
}

//clear page and rewrite current line
void redraw_page(){
    ioctl(0, TIOCGWINSZ, &(gl_env.win));
    term_clear();
    rewrite_line(-1);
}
//not good at freeing memory;
//backspace, deletes character at i
void delete_from(i){
    int counter = 0;
    int temp = i;
    if(i<my_strlen(gl_env.current_cmd)){
         while (temp++ < my_strlen(gl_env.current_cmd)){
             my_char(' ');
             gl_env.cmd_i++;
             //moveright();
             counter++;
         }
         moveleft();
         //my_int(counter);
         while(counter--){
             //my_int(counter);
             moveleft();
         }
         gl_env.current_cmd[i] = 0;
    }

}
//copies from index i in current command to clipboard
void copy_from(i){
    if(i<my_strlen(gl_env.current_cmd)){
        gl_env.clipboard = my_strdup(gl_env.current_cmd + i);
    }
}

//inserts string at i in string
void insert_at(char* str, int i){
    char c;
    int temp = 0;
    if(!str){
        return;
    }
    while(c = str[temp++]){
//        my_str("UPDATING\n");
        update_current_command(c, i);
        gl_env.cmd_i++;
        i++;
    }
    /*int i = 0;
    int paste_len = 
    while(i<gl_env.clipboard*/
}


//this accepts a string, stops the program, and returns
void panic(char* str){
    restore_terminal();
    my_str(str);
    exit(0);
}
//this quits the program. gracefully
void quit_program(){
    term_clear();
    my_str("Goodbye!\n");
    restore_terminal();
    savehist();
    exit(0);
}
//send message to terminal
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
//move cursor left
void curs_left(){
    tputs(gl_env.curs_left, 1, my_termprint);
}
//move cursor right
void curs_right(){
    tputs(gl_env.curs_right, 1, my_termprint);
}
//move cursor up
void curs_up(){
    tputs(gl_env.curs_up, 1, my_termprint);
}
//move cursor down
void curs_down(){
    tputs(gl_env.curs_down, 1, my_termprint);
}
//moves to beginning of line
void move_beg(){
    tputs(gl_env.beg_line, 1, my_termprint);
}
//move to end of command
void move_end(){
    ioctl(0, TIOCGWINSZ, &(gl_env.win));
    int size_col = gl_env.win.ws_row;
//    my_int(size_col);
    move_beg();
    while(--size_col){
        curs_right();
    }
} 
