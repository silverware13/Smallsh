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
	size_t bufSize = 2048;
	size_t characters;
	buffer = (char *)malloc(bufSize * sizeof(char));
	
	// Stores user command.
	char command[50];
	memset(command, '\0', sizeof(command));

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
		
			// React to command.
			sscanf(buffer, "%s", command);	
			if(strcmp(command,"exit") == 0){
				exit(0);
			} else {
				printf("%s: command not found.\n", command);
			}
		}

		// Flush stdout. 
	        fflush(stdout);	

	}

}
