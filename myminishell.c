#include "../lib/my.h"
#include <stdio.h>
#include <unistd.h>
#define BUF_SZ 256

int main()
{
	int n;
	char buffer[BUF_SZ];
	char *cwd;
	char **vect;
	pid_t pid;

	while (1)
	{
		cwd = getcwd(0, 0);
		my_str("\n");
		my_str(cwd);
		my_str(" > ");
		n = read(0, buffer, BUF_SZ - 1);
		buffer[n-1] = '\0';
		//tests
//		my_str(buffer);
//		my_int(my_strcmp(buffer, "exit"));
		if ( my_strcmp(buffer, "exit") == 0 )
		{
			my_str("\nOh, so you prefer the other shell? \n");
			my_str("Goodbye. \n");
			exit(0);
		}
		vect = my_str2vect(buffer);
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
	return 0;
}
