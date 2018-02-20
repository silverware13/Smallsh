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
	
	// Stores arguments.
	//char args[MAX_ARGS][50];
	char** args = calloc(MAX_ARGS, sizeof(char*));		

	// Stores the last child process id and exit.
	pid_t lastPID = 0;
	int lastExit = 0;
	
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
			
			// Save arguments.
			//memset(args, '\0', sizeof(args)); // We clear args before reading in.
			//char nextString[50];
			//memset(nextString, '\0', sizeof(nextString));
			//int bytesConsumed = 0, bytesNow = 0; // Keep track of the number of bytes we have travled in the string.
			//for(int i = 0; i < MAX_ARGS && sscanf(buffer + bytesConsumed, "%s%n", nextString, & bytesNow) == 1; i++) {
			//	
			//	bytesConsumed += bytesNow;
			//}	
		
			// Save arguments.
			char* word;

			// Get the command.
			word = strtok(buffer, " ");
			printf("%s#\n", word);

			while( word != NULL) {
				word = strtok(NULL, " ");
			        printf("%s#\n", word);
			}
				
			// Perform a command based on user input.
			// Exit the process, terminate any processes we have stared.
			if(strcmp(args[0], "exit") == 0){

				// Terminate any process we started.
				exit(0);

			// Change directory, go to HOME if no arguments defined.
			} else if(strcmp(args[0], "cd") == 0){

				if(strcmp(args[1],"") == 0){
					
					// If cd has no args go to HOME.
					chdir(getenv("HOME"));				

				} else {
			
					// We go to the directory specified. 
					if(chdir(args[1]) != 0){
						printf("%s: No such file or directory.\n", args[1]);
					}	
				}
			
			// Show either the exit status or the terminating signal of the last foreground process. 
			} else if(strcmp(args[0], "status") == 0){
				
				// Make sure wait did not fail.
				if(lastPID == -1){
					perror("Wait failed");
					exit(1);
				}
			
				// No processes have terminated.	
				if(lastPID == 0){
					printf("No foreground processes have terminated.\n");
					
				// Check if exited normally or terminated by signal.	
				} else if(WIFEXITED(lastExit)) { 
					int exitStat = WEXITSTATUS(lastExit);
					printf("exit value %d\n", exitStat);	
				} else {
					int termSig = WTERMSIG(lastExit);
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
						// Perform any needed input / output redirection.
						// use dup2()? Don't pass dest/source into the exec.
								
						execvp(&args[0], args); // Execute command.
						perror("Command could not be executed.\n"); // Only get error if process never executed.
						exit(1);
						break;	
					}
					
					// This is the parent, wait for child to finish.
					default: {
						lastPID = waitpid(spawnPID, &childExitMethod, 0); // Wait for child.
						lastExit = childExitMethod; // Save the exit info from last process.
						break;
					}
							
				}
	 
			}

		}

		// Flush stdout. 
	        fflush(stdout);	

	}

}
