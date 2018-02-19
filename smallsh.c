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

#include "smallsh.h"

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
			
			// Perform a command based on user input.
			// Exit the process, terminate any processes we have stared.
			if(strcmp(command,"exit") == 0){

				// Terminate any process we started.
				exit(0);

			// Change directory, go to HOME if no arguments defined.
			} else if(strcmp(command,"cd") == 0){

				if(strcmp(arg[1],"") == 0){
					
					// If cd has no args go to HOME.
					chdir(getenv("HOME"));				

				} else {
			
					// We go to the directory specified. 
					if(chdir(arg[1]) != 0){
						printf("%s: No such file or directory.\n", arg[1]);
					}	
				}
			
			// Show either the exit status or the terminating signal of the last foreground process. 
			} else if(strcmp(command,"status") == 0){
				
				// First check if any child process has exited.
				pid_t childPID = -5;
				int childExitMethod = -5; 
				childPID = waitpid(-1, &childExitMethod, WNOHANG);
				
				// Make sure wait did not fail.
				if(childPID == -1){
					perror("Wait failed");
					exit(1);
				}
			
				// No processes have terminated.	
				if(childPID == 0){
					printf("No foreground processes have terminated.\n");
					
				// Check if exited normally or terminated by signal.	
				} else if(WIFEXITED(childExitMethod) != 0) { 
					int exitStat = WEXITSTATUS(childExitMethod);
					printf("exit value %d\n", exitStat);	
				} else {
					int termSig = WTERMSIG(childExitMethod);
					printf("terminated by signal %d\n", termSig);
				}

			// If the user did not enter a built in command we fork the process then execute.
			} else {
				
				pid_t spawnPID = -5;
				int childExitMethod = -5;
				
				// We fork our current process, creating a child.
				spawnPID = fork();

				// If this current process is the child we execute the given command.
				switch(spawnPID) {
				
					// If fork fails throw error.	
					case -1: {
						perror("Fork failed.\n");
						exit(1);
						break;
					}
					
					// This process is the child, execute command.
					case 0: {
						execlp(command, command, NULL); // Execute command.
						perror("Execlp failed.\n"); // Only get error if process never executed.
						exit(2);
						break;	
					}
					
					// This is the parent, wait for child to finish.
					default: {
						waitpid(spawnPID, &childExitMethod, 0); // Wait for child.
						break;
					}
							
				}
	 
			}

		}

		// Flush stdout. 
	        fflush(stdout);	

	}

}
