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
        else if(!my_strcmp(buffer," ")){
            doselect();
        }
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