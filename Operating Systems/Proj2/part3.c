#include<stdio.h>
#include <sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include"string_parser.h"

void file_commands(FILE *fp, int count);

struct
{
        int count;
        int index;
        pid_t* pid_array;
} handler_args;

void signal_handler(int sigio)
{
        //kill the pid array members
        if (handler_args.index != handler_args.count+1)
        {
        kill(handler_args.pid_array[handler_args.index], SIGSTOP);
        kill(handler_args.pid_array[handler_args.index-1], SIGCONT);
        handler_args.index++;
        alarm(1);
        }
        //kill the straggler
        kill(handler_args.pid_array[handler_args.count],SIGCONT);
}

void cleanup(command_line* c, char** ary, int count)
{
        free(handler_args.pid_array);
        for (int i = 0; i < count; i++)
        {
                free(ary[i]);
                free_command_line(&c[i]);
                memset(&c[i],0,0);
        }
}


int main(int argc,char*argv[])
{
FILE* fp;
handler_args.index = 1;
int linecount = 0;
int newline;
size_t len = 128;
char* line_buf = malloc(len);

	if (argc != 2)
	{
		printf ("Wrong number of arguments\n");
		exit (0);
	}
	fp = fopen(argv[1], "r");
	//get line count
	while (getline(&line_buf, &len, fp)!=-1)
	{
		linecount++;
	}
	handler_args.count = linecount;
	free (line_buf);
	fclose(fp);
	fp = fopen(argv[1], "r");
	file_commands(fp, linecount);	
	return 0;
}

void file_commands(FILE *fp, int count)
{
size_t len = 128;
char* line_buf_array[count];
pid_t pid, ppid;
sigset_t sigset;
int status, signal_val;
pid_t pid_array[count];
command_line token_buffer;
command_line token_buffer_array[count];
char** command_list_array[count];

sigemptyset(&sigset);
sigaddset(&sigset, SIGUSR1);
sigaddset(&sigset, SIGSTOP);
sigaddset(&sigset, SIGCONT);
sigprocmask(SIG_BLOCK, &sigset, NULL);

	//copy the command line to an array
	for (int i = 0; i < count; i++)
	{
		line_buf_array[i] = malloc(len);
		getline(&line_buf_array[i], &len, fp);
		token_buffer = str_filler(line_buf_array[i], " ");
		command_list_array[i] = token_buffer.command_list;
		token_buffer_array[i] = token_buffer;
	}
	fclose(fp);

	handler_args.pid_array = malloc(sizeof(pid_array));

	for (int i = 0; i < count; i++)
	{

		pid_array[i] = fork();
		handler_args.pid_array[i] = pid_array[i];

		if (pid_array[i] < 0)
		{
			perror("Error!");
		} 

		else if (pid_array[i] == 0) 
		{
			sigwait(&sigset, &signal_val);
			if (execvp(command_list_array[i][0],command_list_array[i]) == -1)
			{
				perror ("Error!");
			}
			cleanup(token_buffer_array, line_buf_array, count);
			status = sigwait(&sigset, &signal_val);
			exit(-1);

		}

	}

	signal(SIGALRM, signal_handler);
	alarm(1);
        for (int i = 0;i < count; i++)

	{
                kill(pid_array[i],SIGUSR1);
        }
	waitpid(0, &status,0);
	//finish processes
        if (WIFEXITED(status) < 1)
                for(int i = 0; i < count; i++)
                {
                        kill(pid_array[i],SIGUSR1);
                }

	while(WIFEXITED(status) < 1);

	cleanup(token_buffer_array, line_buf_array, count);
	exit(0);

}
