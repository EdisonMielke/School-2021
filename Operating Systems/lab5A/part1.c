#include<stdio.h>
#include <sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include"string_parser.h"

void script_print (pid_t* pid_ary, int size);
void file_mode(FILE *fp);
void file_commands(FILE *fp, int count);

int main(int argc,char*argv[])
{
FILE* fp;
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
	free (line_buf);
	fclose(fp);
	fp = fopen(argv[1], "r");
	file_commands(fp, linecount);	
	return 0;
}

void file_commands(FILE *fp, int count)
{
size_t len = 128;
char* line_buf =  malloc(len);
char* buf_dup[count];
pid_t pid, ppid;
int status;
pid_t pid_array[count];
command_line token_buffer;
command_line token_buffer_array[count];
char** command_list_array[count];
	
	//copy the command line to an array
	for (int i = 0; i < count; i++)
	{
		getline(&line_buf, &len, fp);
		buf_dup[i] = strdup(line_buf);
		token_buffer = str_filler(buf_dup[i], " ");
		command_list_array[i] = token_buffer.command_list;
		token_buffer_array[i] = token_buffer;
	}
	fclose(fp);


	for (int i = 0; i < count; i++)
	{
		pid_array[i] = fork();
		if (pid_array[i] < 0)
		{
			perror("Error!");
		} else if (pid_array[i] == 0) 
		{
			free(line_buf);
			if (execvp(command_list_array[i][0],command_list_array[i]) == -1)
			{
				perror ("Error!");
			}
			exit(-1);
		}
		free(buf_dup[i]);
		free_command_line(&token_buffer_array[i]);
		memset(&token_buffer_array[i],0,0);
	}

	while(ppid = wait(&status) > 0);

	free(line_buf);
	exit(0);

}
