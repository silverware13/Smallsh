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
			perfComm(args, &lastPID, &lastExit);
		}
	
	}	

	// Free memory.
	free(buffer);
	free(args);

	return 0;

}


// Wait for and perform simple validation of user input.
//
// Input:
// 1: Buffer that will store stdin.
// 2: Size of the buffer.
// 3: Memory to store arguments of command given by user.
//
// Return: Returns 0 if the input is not a command, 
// otherwise returns 1.
int userInput(char *buffer, size_t bufSize, char **args) {

	// Show prompt.
	printf(":"); fflush(stdout);
		
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
			
		// Make sure arguments start as null.
		memset(args, '\0', MAX_ARGS * sizeof(char*));		
		char* word;

		// Get the command.
		word = strtok(buffer, " ");

		// Variables to expand $$.
		int curPID = getpid(); // Holds the current process id.
		char tempBuff[MAX_CHARS]; // Holds expanded argument.
		char* ptrExp; // Points to any instance of $$ in input.

		// Get the arguments.
		for(int i = 0; word != NULL; i++) {
			
			// Store arguments in array.
			args[i] = word;
			word = strtok(NULL, " ");
			
			// If we find a $$ we replace it with the process id.
			if(strstr(args[i], "$$") != NULL){

				// We get the location of the first char of $$
				// in our current argument.
				ptrExp = strstr(args[i], "$$");
			
				// We start by copying just the string before $$ into
				// our temp buffer, then we add the current process id
				// and the last half of the string back into our temp buffer,
				// lastley we copy it back into our current argument.
				strncpy(tempBuff, args[i], ptrExp - args[i]);
				sprintf(tempBuff+(ptrExp - args[i]), "%d%s", curPID, ptrExp+2);
				strcpy(args[i],tempBuff);

			}

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
//
// Input:
// 1: Memory to store arguments of command given by user.
// 2: Process id of last foreground process that terminated.
// 3: Exit status or terminating signal of last process that terminated.
void perfComm(char **args, pid_t *lastPID, int *lastExit) {

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

		forkExe(args, lastPID, lastExit);			

	}
	
}

// Terminate all children and exit the process.
void smallExit() {

	// Terminate any process we started.
	exit(0);

}

// Changes the working directory to given directory. 
// If no directory is given, changes to HOME directory.
//
// Input:
// 1: Arguments of command given by user.
void smallCd(char **args) {

	// We check to see if any directory was specified
	// with the cd command (We ignore &).
	if(args[1] == '\0' || strcmp(args[1], "&") == 0){

		// If cd has no args go to HOME.
		chdir(getenv("HOME"));				

	} else {
			
		// We go to the directory specified. 
		if(chdir(args[1]) != 0){
		
			printf("%s: No such file or directory.\n", args[1]); fflush(stdout);
			
		}	

	}

}

// Prints out either the exit status or the terminating signal of the last foreground process.
//
// Input:
// 1: Arguments of command given by user.
// 2: Process id of last foreground process that terminated.
// 3: Exit status or terminating signal of last process that terminated.
void smallStatus(char **args, pid_t *lastPID, int *lastExit) {
		
	// No processes have terminated.	
	if(*lastPID == 0){
		printf("No foreground processes have terminated.\n"); fflush(stdout);
					
		// Check if exited normally or terminated by signal.	
	} else if(WIFEXITED(*lastExit)) { 
		int exitStat = WEXITSTATUS(*lastExit);
		printf("exit value %d\n", exitStat); fflush(stdout);	
	} else {
		int termSig = WTERMSIG(*lastExit);
		printf("terminated by signal %d\n", termSig); fflush(stdout);
	}

}

// Forks and executes a given command.
//
// Input:
// 1: Arguments of command given by user.
// 2: Process id of last foreground process that terminated.
// 3: Exit status or terminating signal of last process that terminated.
void forkExe(char **args, pid_t *lastPID, int *lastExit) {

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

			// We look through arguments for an input/output redirect.
			for(int i = 0; i < MAX_ARGS; i++) {
			        
				// If the next argument is NULL break.
				if( args[i] == '\0') {
					break;
				}	

				// Check for output redirect.
				if( strcmp(args[i], ">") == 0 ){
					// Open output file.
					int targetFD = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
					if (targetFD == -1) { perror("open()"); exit(1); }
					// Redirect output.
					int result = dup2(targetFD, 1);
					if (result == -1) { perror("dup2"); exit(2); }
					// Set the redirect argument to NULL so we can pass args to exe.
					args[i] = '\0';
					break;
				}
				
				// Check for input redirect.
				if( strcmp(args[i], "<") == 0 ){
					// Open input file.
					int targetFD = open(args[i+1], O_RDONLY);
					if (targetFD == -1) { perror("open()"); exit(1); }
					// Redirect input.
					int result = dup2(targetFD, 0);
					if (result == -1) { perror("dup2"); exit(2); }
					// Set the redirect argument to NULL so we can pass args to exe.
					args[i] = '\0';
					break;
				}

			}			

			//!! Make sure we close and reopen files so they aren't the same in parent.
			
			//!! Setup background processes if they are called.
			
			execvp(args[0], args); // Execute command.
			perror("Command failed to execute"); // Only get error if process never executed.
			exit(1);
			break;
	
		}
					
		// This is the parent, wait for child to finish.
		default: {
		
			*lastPID = waitpid(spawnPID, &childExitMethod, 0); // Wait for child.
			*lastExit = childExitMethod; // Save the exit info from last process.
			break;

		}

	}

}
