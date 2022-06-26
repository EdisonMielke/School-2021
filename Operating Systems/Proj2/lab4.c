#include<stdio.h>
#include <sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

void script_print (pid_t* pid_ary, int size);

int main(int argc,char*argv[])
{
	if (argc == 1)
	{
		printf ("Wrong number of argument\n");
		exit (0);
	}
	int n = atoi(argv[1]);
	pid_t* pid_array = malloc(sizeof(pid_t) * n);
	pid_t pid;
	char* arg[] = {"./iobound", "-seconds", "5", NULL};
	for (int i = 0; i<n; i++)
	{
		pid = fork();
		pid_array[i] = pid;
		if (pid == 0)
		{
			script_print(pid_array, n);
			execvp(arg[0],arg);
			//script_print(pid_array,n);
		}
	}
	free(pid_array);
	return 0;
}


void script_print (pid_t* pid_ary, int size)
{
	FILE* fout;
	fout = fopen ("top_script.sh", "w");
	fprintf(fout, "#!/bin/bash\ntop");
	for (int i = 0; i < size; i++)
	{
		fprintf(fout, " -p %d", (int)(pid_ary[i]));
	}
	fprintf(fout, "\n");
	fclose (fout);

	char* top_arg[] = {"gnome-terminal", "--", "bash", "top_script.sh", NULL};
	pid_t top_pid;

	top_pid = fork();
	{
		if (top_pid == 0)
		{
			if(execvp(top_arg[0], top_arg) == -1)
			{
				perror ("top command: ");
			}
			exit(0);
		}
	}
}


