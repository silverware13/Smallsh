// Assignment #3
// 
// Smallsh:
//
// Custom shell.
//
// Name: Zachary Thomas
// E-mail: thomasza@oregonstate.edu
// Date: 2/14/2018
//

#define _GNU_SOURCE
#define MAX_CHARS 2048
#define MAX_ARGS 512

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void userInput(); // Wait and accept user input.

int main (int argc, char **argvi) {

	// Wait for user input.
	userInput();	
	
	return 0;

}

// Wait for and accept user input.
void userInput() {

	// Buffer info.
	char *buffer;
	size_t bufSize = MAX_CHARS;
	size_t characters;
	buffer = (char *)malloc(bufSize * sizeof(char));
	
	// Stores user command.
	char command[50];
	
	// Stores arguments.
	char arg[MAX_ARGS][50];
	
	while(1) {
	
		// Show prompt.
		printf(":");		
		
		// Get user input as a string.
		getline(&buffer, &bufSize, stdin);
	
		// Check if input is just whitespace.
		int emptyEnter = 1;
		for(int i = 0; i < bufSize; i++) {
	
			// Check for non-white space chars.
			if( buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\0' && buffer[i] != '\n' ){
				emptyEnter = 0;
				break;				
			}

			// If we have a null char we end early.
			if(buffer[i] != '\0'){
				break;
			}

		}			
	
		// Ignore the input if it is a comment or whitespace.
		if(buffer[0] != '#' && emptyEnter == 0){	
			
			// Save command and arguments.
			memset(command, '\0', sizeof(command)); // We clear command before reading in.
			memset(arg, '\0', sizeof(arg[0][0]) * MAX_ARGS * 50); // We clear args before reading in.
			int bytesConsumed = 0, bytesNow = 0; // Keep track of the number of bytes we have travled in the string.
 			sscanf(buffer, "%s", command);
			for(int i = 0; i < MAX_ARGS && sscanf(buffer + bytesConsumed, "%s%n", arg[i], & bytesNow) == 1; i++) {
				bytesConsumed += bytesNow;
			}	
			
			// React to command.
			if(strcmp(command,"exit") == 0){
				// Terminate any process we started.
				exit(0);
			} else if(strcmp(command,"exit") == 0){
				exit(0);
			} else if(strcmp(command,"cd") == 0){
				exit(0);
			} else if(strcmp(command,"status") == 0){
				exit(0);
			} else {
				printf("%s: command not found.\n", command);
			}

		}

		// Flush stdout. 
	        fflush(stdout);	

	}

}
