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
	char *inStr;
	inStr = (char *)malloc(bufSize * sizeof(char)); 
	
	while(1) {
	
		// Show prompt.
		printf(":");		
		
		// Get user input as a string.
		*inStr = getline(&buffer, &bufSize, stdin);
		
		// Print user input as a string.
		printf("%s", buffer);

		// Flush stdout. 
	        fflush(stdout);	

	}

}
