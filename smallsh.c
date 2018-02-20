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

	// Buffer info.
	char *buffer;
	size_t bufSize = MAX_CHARS;
	size_t characters;
	buffer = (char *)malloc(bufSize * sizeof(char));
	
	// Stores arguments.
	char **args = calloc(MAX_ARGS, sizeof(char*));		

	// Stores child process info.
	pid_t rngProcs[100];
	memset(rngProcs, '\0', 100 * sizeof(pid_t));		
	pid_t lastPID = 0;
	int lastExit = 0;
		
	// Lets us know if the given command is valid.
	int validComm;	

	// Loop until we exit manualy.
	while(1){ 
		
		// Get user input.
		validComm = userInput(buffer, bufSize, args);	
	
		// Perform command if a valid command was given.
		if(validComm == 1) {
			perfComm(args, &lastPID, &lastExit, rngProcs);
		}
	
	}	

	// Free memory.
	free(buffer);
	free(args);

	return 0;

}

// Wait for and accept user input.
int userInput(char *buffer, size_t bufSize, char **args) {

	// Show prompt.
	printf(":");		
	fflush(stdout);
		
	// Get user input as a string.
	getline(&buffer, &bufSize, stdin);

	// Clear new lines from our string.
	strtok(buffer, "\n");		
	
	// Check if input is just whitespace.
	int emptyEnter = 1;
	for(int i = 0; i < MAX_CHARS; i++) {
				
		// Check for non-white space chars.
		if( buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\0' && buffer[i] != '\n' ){
			emptyEnter = 0;
			break;				
		}

		// If we have a null char we end early.
		if(buffer[i] == '\0'){
			break;
		}

	}			

	// Ignore the input if it is a comment or whitespace.
	// Otherwise we store the arguments in an array.
	if(buffer[0] != '#' && emptyEnter == 0){	
			
		// Save arguments.
		memset(args, '\0', MAX_ARGS * sizeof(char*));		
		char* word;

		// Get the command.
		word = strtok(buffer, " ");

		// Get the other args.
		for(int i = 0; word != NULL; i++) {
			args[i] = word;
			word = strtok(NULL, " ");
		}
	
		// We have recevied a command
		// we return 1.
		return 1;
			
	} else {

		// If the input is not a command
		// then we return 0.
		return 0;

	}
}

// Perform a command based on user input.
void perfComm(char **args, pid_t *lastPID, int *lastExit, pid_t rngProcs[100]) {

	// Exit the process, terminate any processes we have stared.
	if(strcmp(args[0], "exit") == 0){
	
		smallExit();
		
	// Change directory, go to HOME if no arguments defined.
	} else if(strcmp(args[0], "cd") == 0){

		smallCd(args);		
	
	// Show either the exit status or the terminating signal of the last foreground process. 
	} else if(strcmp(args[0], "status") == 0){
				
		smallStatus(args, lastPID, lastExit);

	// If the user did not enter a built in command we fork the process then execute.
	} else {

		forkExe(args, lastPID, lastExit, rngProcs);			

	}

	// Flush stdout. 
	fflush(stdout);	
}

void smallExit() {

	// Terminate any process we started.
	exit(0);

}

void smallCd(char **args) {

	if(args[1] == '\0'){

		// If cd has no args go to HOME.
		chdir(getenv("HOME"));				

	} else {
			
		// We go to the directory specified. 
		if(chdir(args[1]) != 0){
			printf("%s: No such file or directory.\n", args[1]);

		}	

	}

}

void smallStatus(char **args, pid_t *lastPID, int *lastExit) {
		
	// Make sure wait did not fail.
	if(*lastPID == -1){
		perror("Wait failed");
		exit(1);
	}
			
	// No processes have terminated.	
	if(*lastPID == 0){
		printf("No foreground processes have terminated.\n");
					
		// Check if exited normally or terminated by signal.	
	} else if(WIFEXITED(*lastExit)) { 
		int exitStat = WEXITSTATUS(*lastExit);
		printf("exit value %d\n", exitStat);	
	} else {
		int termSig = WTERMSIG(*lastExit);
		printf("terminated by signal %d\n", termSig);
	}

}

void forkExe(char **args, pid_t *lastPID, int *lastExit, pid_t rngProcs[100]) {

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
							
			execvp(args[0], args); // Execute command.
			perror("Command could not be executed.\n"); // Only get error if process never executed.
			exit(1);
			break;
	
		}
					
		// This is the parent, wait for child to finish.
		default: {
		
			// Store the child process in an array.
			for(int i = 0; i < 100; i++){
			
				if(rngProcs[i] == '\0'){
					rngProcs[i] = spawnPID;
					//printf("STORED PID %ld\n", (long) spawnPID);
					break;
				}		

			}

			*lastPID = waitpid(spawnPID, &childExitMethod, 0); // Wait for child.
			*lastExit = childExitMethod; // Save the exit info from last process.
			
			// Remove the child process from array.
			for(int i = 0; i < 100; i++){
			
				if(rngProcs[i] == spawnPID){
					rngProcs[i] = '\0';
					//printf("REMOVE PID %ld\n", (long) spawnPID);
					break;
				}		

			}

			break;

		}

	}

}
