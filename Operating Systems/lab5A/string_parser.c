#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GNU_SOURCE

int count_token (char* buf, const char* delim)
{

	//Declare variables
	char* bufcopy = strdup(buf);
	char* bufcopy_ptr = bufcopy;
	char* token;
	int tokCount = 0;
	// A,B Checks the zero position delim, end delim is accounted for by strtok_r
	if (&buf[0] == delim) 
	{
		strtok_r(bufcopy, delim, &bufcopy);
	}
	// 1./2. Iterate through the string and keep a check for the NULL string
	while ((token = strtok_r(bufcopy, delim, &bufcopy)) != NULL)
	{
		//Counts the tokens to be returned later
		tokCount++;
	}
	//Free copy pointer
	free(bufcopy_ptr);
	//return count
	return tokCount;
		/*
		*	#1.X	Check for NULL string
		*	#2.X	iterate through string counting tokens
		*		Cases to watchout for
		*			a.	string start with delimeter
		*			b. 	string end with delimeter
		*			c.	account NULL for the last token
		*	#3.X return the number of token (note not number of delimeter)
		*/
}

command_line str_filler (char* buf, const char* delim)
{
	// 1 Declarations
	char* token = NULL;
	command_line return_var;

	// 2 grab number of tokens
	return_var.num_token = count_token(buf, delim);
	
	// 3 allocate the array list
	// I'm not 100% sure why +8 works and not +1, but +8 works without any errors, 
	// +1 *works* but with errors and warnings in valgrind
	// I don't know why but +8 is the sweet spot
	
	return_var.command_list = malloc(return_var.num_token * sizeof(char**)+8);	

	for (int i = 0; i < return_var.num_token; i++)
	{
		// 2 Grabs token
		token = strtok_r(buf, delim, &buf);
		// 2  removes newline
		token = strtok_r(token, "\n" , &token);
		// 5 allocate and free each index from the count_token function (thanks for the idea Ash)
		// 5 assign each index
		return_var.command_list[i] = token;

	}
	// 5 Last spot = NULL
	return_var.command_list[return_var.num_token] = NULL;
	return return_var;
	/*
	*	#1.X	create command_line variable to be filled and returned
	*	#2.X	count the number of tokens with count_token function, 
	*		set num_token. one can use strtok_r to remove the \n at the end of the line.
	*	#3.X    malloc memory for token array inside command_line variable based on the number of tokens.
	*	#4.X	use function strtok_r to find out the tokens 
    	*       #5.X    malloc each index of the array with the length of tokens, 
	*		fill command_list array with tokens, and fill last spot with NULL.
	*	#6.X    return the variable.
	*/
}


void free_command_line(command_line* command)
{
	free(command->command_list);
}

