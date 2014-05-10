#include "../lib/my.h"
#include "mynotsominishell.h"
#include <stdio.h>
#include <unistd.h>

void print_history_at(unsigned int n)
{
	if(n >= HISTMAX)
	{
		my_panic("Out of range");
	}
	else
	{
		delete_from(0);
		insert_at(gl_env.history_array[n], 0);
	}
}

void older_history() //to be called when pressing up on the keyboard
{
	if(gl_env.history_current != gl_env.history_size - 1) //If the user is not at the earliest item in history
	{
		gl_env.history_current++; //go one item further back into history
		print_history_at(gl_env.history_current); //print that item from history into the prompt
	}
	exit(0);
}

void newer_history()
{
	if(gl_env.history_current != -1) //nothing should happen if you are typing a new command
	{
		if(gl_env.history_current !=0)
		{
			gl_env.history_current--;
			print_history_at(gl_env.history_current);
		}
	}
	exit(0);
}

void addtohist(char *command)
{
	if(gl_env.history_size != 0)
	{
		int i;

		if(gl_env.history_size == HISTMAX) //if the array is full
			free(gl_env.history_array[HISTMAX-1]);

		for(i = gl_env.history_size-1; i > 0; i--) //start at 1 less than the size of history (the last populated item in the array); do for every item except the first
		{
			gl_env.history_array[i] = gl_env.history_array[i-1]; //move it back
		}
	}
	if(gl_env.history_size < HISTMAX) //if the array is not currently filled
		gl_env.history_size++; //increase history_size

	gl_env.history_array[0] = command; //make history_array[0] the command that was just entered
	gl_env.history_current = -1; //set history_current to -1
}

void savehist()
{
    int fd;
    int i;
    char *del[] = {"rm", ".history", NULL};

    execvp(del[0], del);
    if(gl_env.history_size > 0)
    {
        fd = open(".history", O_WRONLY);
        if(!fd)
            my_panic("Error opening .history! \n");

        for(i = 0; i < gl_env.history_size; i++)
        {
            write(fd, gl_env.history_array[i], (sizeof(char) * my_strlen(gl_env.history_array[i])));
        }
        close(fd);
    }
	exit(0);
}