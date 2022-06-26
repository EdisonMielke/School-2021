#include "command.h"
#include "string_parser.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
FILE *fp;

int valid_commands(char* line, int count)
{
	int retval = 0;
	if(strstr(line,"ls")
	|| strstr(line,"pwd")
	|| strstr(line,"mkdir")
	|| strstr(line,"cd")
	|| strstr(line,"mv")
	|| strstr(line,"rm")
	|| strstr(line,"cat"))
		retval = 1;
	
	if((strstr(line,"ls") && count == 1) 
	|| (strstr(line,"pwd") && count == 1)
	|| (strstr(line,"mkdir") && count == 2)
	|| (strstr(line,"cd") && count == 2)
	|| (strstr(line,"cp") && count == 3)
	|| (strstr(line,"mv") && count == 3)
	|| (strstr(line,"rm") && count == 2)
	|| (strstr(line,"cat") && count == 2))
		retval = 2;

	return retval;
}

void file_mode(FILE* fp)
{
	size_t len = 128;
	char* line_buf = malloc (len);
	command_line large_token_buffer;
	command_line small_token_buffer;
	
	int line_num = 0;
	
	//loop until the file is over
	while (getline (&line_buf, &len, fp) != -1)
	{
		//tokenize line buffer
		//large token is seperated by ";"
		large_token_buffer = str_filler (line_buf, ";");
		//iterate through each large token
		for (int i = 0; large_token_buffer.command_list[i] != NULL; i++)
		{
			//tokenize large buffer
			//smaller token is seperated by " "(space bar)
			small_token_buffer = str_filler (large_token_buffer.command_list[i], " ");
			int j = 0;
			//iterate through each smaller token to print
			for (j = 0; small_token_buffer.command_list[j] != NULL; j++);
			char* command = small_token_buffer.command_list[0];
			if (valid_commands(command,j) == 2)
			{
				if (strstr(command,"ls"))
				{
					listDir();
				}
				if (strstr(command,"pwd"))
				{
					showCurrentDir();
				}
				if (strstr(command,"mkdir"))
				{
					makeDir(small_token_buffer.command_list[1]);
				}
				if (strstr(command,"cd"))
				{
					DIR* dir = opendir(small_token_buffer.command_list[1]);
					if(dir)
					{
						changeDir(small_token_buffer.command_list[1]);
						closedir(dir);
					}
					else
					{	
						printf("Error! Invalid Directory: %s\n", small_token_buffer.command_list[1]);
					}						
				}
				if (strstr(command,"cp"))
				{
					FILE* fp = fopen(small_token_buffer.command_list[1],"r");
					//DIR* dir = opendir(small_token_buffer.command_list[2]);
					//if ((fp != NULL) && (dir != NULL))
					if (fp != NULL)	
					{
						copyFile(small_token_buffer.command_list[1], small_token_buffer.command_list[2]);
						fclose(fp);
						//closedir(dir);
					}
					else if (fp == NULL)
					{
						printf("Error! Missing or invalid file: %s\n", small_token_buffer.command_list[1]);
						//if(dir != NULL)
						//	closedir(dir);
					}
					//else if (dir == NULL)
					//{
					//	printf("Error! Missing or invalid directory: %s\n", small_token_buffer.command_list[2]);
					//	if(fp != NULL)
					//		fclose(fp);
					//}
				}
				if (strstr(command,"mv"))
				{
					FILE* fp = fopen(small_token_buffer.command_list[1],"r");
					if(fp != NULL)
					{
						moveFile(small_token_buffer.command_list[1], small_token_buffer.command_list[2]);
						fclose(fp);
					}
					else if (fp == NULL)
					{
						printf("Error! Missing or invalid file: %s\n", small_token_buffer.command_list[1]);
					}
				}
				if (strstr(command,"rm"))
				{
					FILE* fp = fopen(small_token_buffer.command_list[1],"r");
					if (fp != NULL)
					{
						deleteFile(small_token_buffer.command_list[1]);
						fclose(fp);
					}
					else
					{
						printf("Error! Missing or invalid file: %s\n", small_token_buffer.command_list[1]);
					}
				
				}
				if (strstr(command,"cat"))
				{
					FILE* fp = fopen(small_token_buffer.command_list[1],"r");
					if (fp != NULL)
					{
						displayFile(small_token_buffer.command_list[1]);
						fclose(fp);
					}
					else
						fprintf(stderr,"Error! Missing or invalid file: %s\n", small_token_buffer.command_list[1]);
				}
			}
			else if (valid_commands(small_token_buffer.command_list[0],j) == 0)
			{
				printf("Error! Unrecognized command: %s\n", small_token_buffer.command_list[0]);
			}
			else
			{
			printf("Error! Unsupported parameters for command: %s\n", small_token_buffer.command_list[0]);
			}
			//free smaller tokens and reset variable
			free_command_line(&small_token_buffer);
			memset (&small_token_buffer, 0, 0);
		}

		//free smaller tokens and reset variable
		free_command_line (&large_token_buffer);
		memset (&large_token_buffer, 0, 0);
	}
	//fclose(inFPtr);
	//free line buffer
	free (line_buf);
}



void command_line_mode()
{
	//I wanted to just call file_mode for stdin, but that ended up being more trouble than it's worth...
	size_t len = 128;
	char* line_buf = malloc (len);

	command_line large_token_buffer;
	command_line small_token_buffer;

	int line_num = 0;
	int quitFlag = 0;

	while (quitFlag == 0) 
	{
		printf(">>>");
		getline( &line_buf, &len, stdin);
			//printf ("Line %d:\n", ++line_num);

			//tokenize line buffer
			//large token is seperated by ";"
			large_token_buffer = str_filler (line_buf, ";");
			//iterate through each large token
			for (int i = 0; large_token_buffer.command_list[i] != NULL; i++)
			{

				//tokenize large buffer
				//smaller token is seperated by " "(space bar)
				small_token_buffer = str_filler (large_token_buffer.command_list[i], " ");
				int j = 0;
				//iterate through each smaller token to do commands
				for (j = 0; small_token_buffer.command_list[j] != NULL; j++);
					//There's almost certainly a far neater way to do this, however, 
					//this makes sense for me currently and doesn't break the assignment
					//it occurs to me now that this is pretty good example on why someone should use a switch
					//welp, in too deep now
					char* command = small_token_buffer.command_list[0];
					
					if (valid_commands(command,j) == 2)
					{
						if (strstr(command,"ls"))
						{
							listDir();
						}
						if (strstr(command,"pwd"))
						{
							showCurrentDir();
						}
						if (strstr(command,"mkdir"))
						{
							makeDir(small_token_buffer.command_list[1]);
						}
						if (strstr(command,"cd"))
						{
							DIR* dir = opendir(small_token_buffer.command_list[1]);
							if(dir)
							{
								changeDir(small_token_buffer.command_list[1]);
								closedir(dir);
							}
							else
							{
								printf("Error! Invalid Directory: %s\n", small_token_buffer.command_list[1]);
							}					
						}
						if (strstr(command,"cp"))
						{
							FILE* fp = fopen(small_token_buffer.command_list[1],"r");
							if(fp!=NULL)
							{
								copyFile(small_token_buffer.command_list[1], small_token_buffer.command_list[2]);
								fclose(fp);
							}
							else if (fp == NULL)
							{
								printf("Error! Missing or invalid file: %s\n", small_token_buffer.command_list[1]);
							}
						}
						if (strstr(command,"mv"))
						{
							FILE* fp = fopen(small_token_buffer.command_list[1],"r");

							if (fp!=NULL)
							{
								moveFile(small_token_buffer.command_list[1], small_token_buffer.command_list[2]);
								fclose(fp);
							}
							else if (fp == NULL)
							{
								printf("Error! Missing or invalid file: %s\n", small_token_buffer.command_list[1]);
							}
						}
						if (strstr(command,"rm"))
						{
							FILE* fp = fopen(small_token_buffer.command_list[1],"r");
							if (fp != NULL)
							{
								deleteFile(small_token_buffer.command_list[1]);
							fclose(fp);
							}
							else
							{
								printf("Error! Missing or invalid file: %s\n", small_token_buffer.command_list[1]);
							}
						}
						if (strstr(command,"cat"))
						{
							FILE* fp = fopen(small_token_buffer.command_list[1],"r");
							if (fp != NULL)
							{
							displayFile(small_token_buffer.command_list[1]);
							fclose(fp);
							}
							else
							{
								printf("Error! Missing or invalid file: %s\n", small_token_buffer.command_list[1]);
							}
						}
					}
					else if (strstr(command,"quit"))
					{	
						quitFlag++;
					}
					else if (valid_commands(command,j) == 0)
					{
						printf("Error! Unrecognized command: %s\n",command);
					}
					else
					{
						printf("Error! Unsupported parameters for command: %s\n",command);
					}
				//free smaller tokens and reset variable
				free_command_line(&small_token_buffer);
				memset (&small_token_buffer, 0, 0);
			}

			//free smaller tokens and reset variable
			free_command_line (&large_token_buffer);
			memset (&large_token_buffer, 0, 0);
	}
	//free line buffer
	free (line_buf);
		
}

int main(int argc, char* argv[])
{	
	if(argc == 1)
	{
		//printf("GOING TO COMMAND LINE\n");  //REMOVE WHEN DONE
		command_line_mode();
	}
	
	else if(strstr(argv[1],"-f"))
		{
			fp = fopen(argv[2],"r");
			if(fp == NULL)
			{
				printf("ERROR: missing or invalid file\n");
			}
			else
			{
				//printf("GOING TO FILE MODE\n"); //REMOVE WHEN DONE
				freopen("output.txt","w",stdout);
				file_mode(fp);
				//file_mode(fp);
				fclose(fp);
			}
		}
	else
	{
		printf("INVALID USAGE:\n For interactive command line mode enter: ./pseudo-shell\n For file mode enter: ./pseudo-shell -f filename\n");
	}
}
