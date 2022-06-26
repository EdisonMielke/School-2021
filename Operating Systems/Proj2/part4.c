#include<stdio.h>
#include <sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include"string_parser.h"

void file_commands(FILE *fp, int count);
void important_stats();
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
		//let's forking go crazy
		pid_array[i] = fork();
		//measured crazy by adding pid_array to the global pid_array
		handler_args.pid_array[i] = pid_array[i];
		if (pid_array[i] < 0)
		{
			perror("Error!");
		} 
		else if (pid_array[i] == 0) 
		{
			//wait until SIGUSR1 is signalled
			sigwait(&sigset, &signal_val);
			if (execvp(command_list_array[i][0],command_list_array[i]) == -1)
			{
				perror ("Error!");
			}
			//cleanup before exit
			cleanup(token_buffer_array, line_buf_array, count);
			sigwait(&sigset, &signal_val);
			exit(-1);
		}
	}
	
	//signal the signal handler
	signal(SIGALRM, signal_handler);
	alarm(1);
	//start the processes normally while signal handler does its magic
        for (int i = 0;i < count; i++)
        {
		printf("Process %d Started:\n",pid_array[i]);
                kill(pid_array[i],SIGUSR1);
		important_stats(pid_array[i]);
		printf("\n\n");
        }
	
	//finish processes
	if (waitpid(0, &status, 0));
		for(int i = 0; i < count; i++)
		{
			kill(pid_array[i],SIGUSR1);
		}
	//probably redundant, but to be safe
	while(WIFEXITED(status) < 1);

	//cleanup resources
	cleanup(token_buffer_array, line_buf_array, count);
	exit(0);

}

void important_stats()
{
//this is sloppy, but it isn't leaking
size_t len = 128;
char* line_buf_array[len];
char* line_buf;
line_buf_array[0] = malloc(len);
int i = 0;
command_line com;
command_line com_array[len];


	FILE* fp = fopen("/proc/stat","r");
        for (i = 0; getline(&line_buf_array[i], &len, fp) != -1; i++)
        {
         	line_buf_array[i+1] = malloc(len);
		com = str_filler(line_buf_array[i], " ");
                com_array[i] = com;
		if (strstr(com_array[i].command_list[0], "procs_running"))
			printf("Processes Running: %s | ", com_array[i].command_list[1]);
		if (strstr("cpu", com_array[i].command_list[0]))
                        printf("User Mode Processes: %s | ", com_array[i].command_list[1]);
		if (strstr("cpu", com_array[i].command_list[0]))
                        printf("Kernel Mode Processes: %s | ", com_array[i].command_list[3]);
		
        }
	for (int j = 0; j<i; j++)
	{
		free(line_buf_array[j]);
		free_command_line(&com_array[j]);
		memset(&com_array[j],0,0);
	}
	free(line_buf_array[i]);
        fclose(fp);
	

}
