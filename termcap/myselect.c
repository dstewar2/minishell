#include "./myselect.h"
//main function displays argv and lets you select them with arrow keys and space then return selected by typing return
int main(int argc, char** argv){
    //initialize selections
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
        if(!gl_env.flag){
            //if screen showed successfully
            if(!my_strcmp(buffer, KU)){
                moveup();
            }
            else if(!my_strcmp(buffer,KD)){
                movedown();
            }
            else if(!my_strcmp(buffer,KL)){
                moveleft();
            }
            else if(!my_strcmp(buffer,KR)){
                moveright();
            }
            else if(!my_strcmp(buffer," ")){
                doselect();
            }
        }
            if(n==1 && buffer[0]==ESC){
                quit_program();
            }
            else if(n==1 && buffer[0]=='\n'){
                return_program();
            }
        //my_str(buffer);
    }
    restore_terminal();
    //Initialize terminal

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

//this quits the program gracefully and returns  selected strings from argv
void return_program(){
    int i;
    restore_terminal();
    int space = 0;
    for(i=0; i<gl_env.nbelems; i++){
        if(gl_env.elements[i].mode){
            if(space)
                my_char(' ');
            my_str(gl_env.elements[i].elem);
            space = 1;
        }
    }
    exit(0);
}

//this accepts a char (despite it being an int) and writes it to standard output.
int my_termprint(int c){
    return write(1, &c, 1);
}

//displays elements
void show_elems(){
    //get window size
    int curr_x = 0;
    int curr_y = 0;
    int i, max_x, max_y;
    ioctl(0, TIOCGWINSZ, &(gl_env.win));
    max_x = gl_env.win.ws_col;
    max_y = gl_env.win.ws_row;
    //init terminal
    term_clear();
    //my_str("SHOULD HAVE CLEARED");
    if(max_y <=0){
        term_clear();
        my_str("Not enough space. Please enlarge window");
        gl_env.flag = 1;
    }
    int column_length = 0;
    //print each element
    for(i=0; i<gl_env.nbelems; i++){
        //my_str("it ");
        //my_int(i);
        t_elem* curr_elem = &gl_env.elements[i];
        if(curr_y >= max_y){
            //if y hits bottom go to top of screen
            curr_y = 0;
            //reset column length
            curr_x += column_length + 1;
            column_length = 0;
        }
        if((curr_x + curr_elem->size) < max_x){
            if(curr_elem->size > column_length){
                column_length = curr_elem->size;
            }
            curr_elem->x = curr_x;
            curr_elem->y = curr_y;
            refreshout(i);
            //my_str(curr_elem->elem);
        }else{
            term_clear();
            my_str("Not enough space. Please enlarge window");
            gl_env.flag = 1;
            return;
        }
        //
        //move cursor down
        curr_y++;

    }
    gl_env.flag = 0;
    refreshin();
}

//HELPER FUNCTIONS.
//moves cursor to column and row
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

//accepts index of element and moves cursor to it
void move_cursor_to_item(int i){
    term_move_cursor(gl_env.elements[i].x, gl_env.elements[i].y);

}

//underlines currently selected item
void refreshin(){
    if(gl_env.pos<0)
        gl_env.pos =0;
    move_cursor_to_item(gl_env.pos);
    tputs(gl_env.underline, 1, my_termprint);
    if(gl_env.elements[gl_env.pos].mode){
        term_stand_out();
    }
    my_str(gl_env.elements[gl_env.pos].elem);
    term_stand_end();
    tputs(gl_env.under_end, 1, my_termprint);
}

void refreshout(int i){
    if(i>=gl_env.nbelems){
        my_str("Bad index");
        return;
    }
    move_cursor_to_item(i);
    if(gl_env.elements[i].mode){
        term_stand_out();
    }
    //term
    my_str(gl_env.elements[i].elem);
    term_stand_end();
}

//MOVEMENT FUNCTIONS
//moves to an element
void term_move_to_item(int i){
    if(i<0){
        i=0; 
    }
    if(i>=gl_env.nbelems){
        panic("Bad index!");
    }
    refreshout(gl_env.pos);
    //refreshout(i+1);
    gl_env.pos = i;
    refreshin(i);
}
//moves down. Loops aroujnd
void movedown(){
    term_move_to_item((gl_env.pos+1)%gl_env.nbelems);
}
//moves up. loops around
void moveup(){
    if((gl_env.pos-1)<0){
        //go to bottom if press up too many times
        term_move_to_item(gl_env.nbelems-1);
    }else{
        term_move_to_item(gl_env.pos-1);
    }
}
//moves right. Doesn't loop around
void moveright(){
    if( (gl_env.pos + gl_env.win.ws_row) >= gl_env.nbelems ){
        //if can't move right a row
        term_move_to_item(gl_env.nbelems-1);
    }else{
        term_move_to_item(gl_env.pos +gl_env.win.ws_row);
    }
}
//moves left. doesn't loop around
void moveleft(){
    if( (gl_env.pos - gl_env.win.ws_row) < 0 ){
        //if can't move right a row
        term_move_to_item(0);
    }else{
        term_move_to_item(gl_env.pos - gl_env.win.ws_row);
    }
}

//select or deselect current item
void doselect(){
    gl_env.elements[gl_env.pos].mode = !gl_env.elements[gl_env.pos].mode;
    refreshout(gl_env.pos);
}

