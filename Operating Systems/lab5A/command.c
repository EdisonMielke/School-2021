#include "command.h"
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/sendfile.h>
//since string.h is permitted
#include <string.h>
//for BUFSIZ
#include <stdio.h>
void listDir() /*for the ls command*/
{
	DIR* dir;
	struct dirent *dp;
	char buf[BUFSIZ];
	getcwd(buf, sizeof(buf));
	//according to lab 2, opendir + readdir are syscalls
	dir = opendir(buf);
	
	while (dp = readdir(dir))
	{
		write(1, dp->d_name,strlen(dp->d_name));
		write(1, " ", 1);
	}
	write(1, "\n", 1);
	closedir(dir);
}
void showCurrentDir() /*for the pwd command*/
{
	char buf[BUFSIZ];
	size_t bufsize = sizeof(buf);
	if (getcwd(buf, bufsize) != NULL)
	{	
		strcat(buf,"\n");
		write(1,buf,strlen(buf));
	}
}
void makeDir(char *dirName) /*for the mkdir command*/
{
	mkdir(dirName, 0777);
}
void changeDir(char *dirName) /*for the cd command*/
{
	chdir(dirName);
}
void copyFile(char *sourcePath, char *destinationPath) /*for the cp command*/
{
	int input, output;    
    	input = open(sourcePath, O_RDONLY);
   	output = creat(destinationPath, 0660);
    	off_t bytesCopied = 0;
    	struct stat fileinfo = {0};
    	fstat(input, &fileinfo);
    	int result = sendfile(output, input, &bytesCopied, fileinfo.st_size);

    	close(input);
    	close(output);
}
void moveFile(char *sourcePath, char *destinationPath) /*for the mv command*/
{	
	copyFile(sourcePath, destinationPath);
	deleteFile(sourcePath);
}
void deleteFile(char *filename) /*for the rm command*/
{
	unlink(filename);
}
void displayFile(char *filename) /*for the cat command*/
{
	int open_file = open(filename, O_RDONLY);
	char buf[BUFSIZ];
	size_t bufsize = sizeof(buf);
	if (read(open_file, buf, bufsize))
		write(1,buf,strlen(buf));
	close(open_file);

}
