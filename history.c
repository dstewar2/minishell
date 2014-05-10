#include "../lib/my.h"
#include "mynotsominishell.h"
#include <stdio.h>
#include <unistd.h>

void print_history_at(unsigned int n)
{
	if(n >= HISTMAX)
	{
		my_str("Out of range");
	}
	else
	{
		delete_from(0);
		insert_at(gl_env.history_array[n], 0);
	}
	exit(0);
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

	gl_env.history_array[0] = my_strdup(command); //make history_array[0] the command that was just entered
	gl_env.history_current = -1; //set history_current to -1
}

void savehist()
{
	int fd;
	int i;
	char *del[] = {"rm", ".history", NULL};

	execvp(del[0], del);  //deletes .history if it already exists
	if(gl_env.history_size > 0)  //if there are commands to save
	{
		fd = open(".history", O_WRONLY); //opens .history in write-only mode
		if(!fd)  //if .history can't be opened
		{
			my_str("Error opening .history! \n"); //panic
			exit(2);
		}

		for(i = 0; i < gl_env.history_size; i++) //for every item in history
			write(fd, gl_env.history_array[i], (sizeof(char) * my_strlen(gl_env.history_array[i]))); //writes write it out in .history

		close(fd); //close/save .history
	}
	exit(0); //end function
}

void loadhist() //this totally won't work.  Needs a lot of editing.
{
	int fd;
	int i;
	char bufchar;

	if(gl_env.history_size > 0)
	{
		fd = open(".history", O_RDONLY);
		if(!fd)
		{
			my_str("Error opening .history! \n");
			exit(2);
		}

		for(i = 0; i < gl_env.history_size; i++)
			while (bufchar != '\n')
				bufchar = read(fd, gl_env.history_array[i], 1);

		close(fd);
	}
	exit(0);
}
