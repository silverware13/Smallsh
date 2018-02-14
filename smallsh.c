// Program 3
// 
// Smallsh:
//
// Custom shell.
//
// Name: Zachary Thomas
// E-mail: thomasza@oregonstate.edu
// Date: 2/14/2018
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void userInput(); // Wait and accept user input.

int main (int argc, char **argvi) {

	// Wait for user input.
	userInput();	

}

// Wait for and accept user input.
void userInput() {

	// Buffer info.
	char *buffer;
	size_t bufSize = 2048;
	size_t characters;
	buffer = (char *)malloc(bufSize * sizeof(char));
	
	while(1) {
	
		// Get user input as a string.
		buffer = getline(&buffer, &bufSize, stdin);

		// Print user input as a string.
		printf("%s", buffer);

		// Show prompt.
		printf("Smallsh:");		

		// Flush stdout. 
	        fflush(stout);	

	}

}
