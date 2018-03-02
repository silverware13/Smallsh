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

	// Setup signal handling.
	struct sigaction SIGINT_action = {0}, SIGTSTP_action = {0}, SIGQUIT_action = {0}, SIGCHLD_action = {0};	

	// Set values for SIGINT handling.
	SIGINT_action.sa_handler = catchSIGINT;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = SA_RESTART; // Cause interruptible functions to restart.	
	
	// Set values for SIGSTP handling.
	SIGTSTP_action.sa_handler = catchSIGTSTP;
	sigfillset(&SIGTSTP_action.sa_mask);
	SIGTSTP_action.sa_flags = 0;	
	
	// Set values for SIGCHLD handling.
	SIGCHLD_action.sa_handler = catchSIGCHLD;
	sigfillset(&SIGCHLD_action.sa_mask);
	SIGCHLD_action.sa_flags = 0;	
	
	// Set values for SIGQUIT handling.
	SIGQUIT_action.sa_handler = SIG_IGN;
	
	sigaction(SIGINT, &SIGINT_action, NULL);
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);
	sigaction(SIGQUIT, &SIGQUIT_action, NULL);
	//sigaction(SIGCHLD, &SIGCHLD_action, NULL);
	
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
	pid_t childPID;
	int childExitMethod;
		
	// Lets us know if the given command is valid.
	int validComm;	

	// Loop until we exit manualy.
	while(1){ 

		// Check if any background process has exited.	
		childPID = waitpid(-1, &childExitMethod, WNOHANG);
		if(childPID > 0){
			
			printf("Background pid %d is done: ", childPID); fflush(stdout);
			
			// Check if exited normally or terminated by signal.	
			if(WIFEXITED(childExitMethod)) { 
				int exitStat = WEXITSTATUS(childExitMethod);
				printf("exit value %d\n", exitStat); fflush(stdout);	
			} else {
				int termSig = WTERMSIG(childExitMethod);
				printf("terminated by signal %d\n", termSig); fflush(stdout);
			}		
		}

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

	// Clear input first.
	clearerr(stdin);
	memset(args, '\0', MAX_ARGS * sizeof(char*));		
	memset(buffer, '\0', bufSize * sizeof(char));

	// Show prompt.
	printf(":"); fflush(stdout);
			
	// Get user input as a string.
	int numChars = getline(&buffer, &bufSize, stdin);
	if(numChars == -1) {clearerr(stdin);} // If we have an error with getline clear buffer. 

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

	// Terminate all children before we exit.
	// We send the SIGQUIT signal to parent
	// who ignores it, but children die to it.
	pid_t parent_pid;
	parent_pid = getpid();
	kill(-parent_pid, SIGQUIT);
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

	// Check to see if this will be a background process.
	// Don't allow background process if we are in foreground only mode.
	int bckProc = 0;
	for(int i = 0; i < MAX_ARGS && frgMode == 0; i++) {
				
		// If the next argument is NULL break.
		if( args[i] == '\0') {
			break;
		}	

		// If the last argument is &, this is a background process.
		if(strcmp(args[i], "&") == 0 && args[i+1] == '\0') {
				
			// If in foreground only mode ignore &.	
			if(frgMode == 0){
				bckProc = 1; // Let us know that this is a background process.
			}
			
			args[i] = '\0'; // Don't pass & to exe.	
			break;
				
		}
	}

	// Prepare to fork.
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
			
			// Make stdout, sterr, and stdin all point to null if this is a background process.
			if(bckProc) {
				freopen("/dev/null", "w", stdout);
 				freopen("/dev/null", "w", stderr);
	 			freopen("/dev/null", "r", stdin);
			}

			// Set SIGINT handling to default or ignore if background process.	
			struct sigaction SIGINT_action = {0}, SIGQUIT_action = {0};
			if(bckProc) {	
				SIGINT_action.sa_handler = SIG_IGN;
			} else {
				SIGINT_action.sa_handler = SIG_DFL;
			}
			sigaction(SIGINT, &SIGINT_action, NULL);
			
			// SIGQUIT should always kill children.
			SIGQUIT_action.sa_handler = SIG_DFL;
			sigaction(SIGQUIT, &SIGQUIT_action, NULL);

			int i_out = -1; // Will store the location of output redirection.
			int i_in = -1; // Will store the location of input redirection.

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
					// Get ready to set the redirect argument to NULL so we can pass args to exe.
					i_out = i;
				}
				
				// Check for input redirect.
				if( strcmp(args[i], "<") == 0 ){
					// Open input file.
					int sourceFD = open(args[i+1], O_RDONLY);
					if (sourceFD == -1) { perror("open()"); exit(1); }
					// Redirect input.
					int result = dup2(sourceFD, 0);
					if (result == -1) { perror("dup2"); exit(2); }
					i_in = i;
				}	

			}	
			
			// Check if we used redirection, if we did prevent it from being passed to our execute.
			if(i_out != -1){ args[i_out] = '\0'; }
			if(i_in != -1){ args[i_in] = '\0'; }
			
			execvp(args[0], args); // Execute command.
			perror("Command failed to execute"); // Only get error if process never executed.
			exit(1);
			break;
	
		}
					
		// This is the parent, wait for child to finish.
		default: {
		
			// We wait for foreground processes.
			if(bckProc == 0){
				*lastPID = waitpid(spawnPID, &childExitMethod, 0); // Wait for child.
				*lastExit = childExitMethod; // Save the exit info from last process.
				
				// Make sure that a signal  didn't terminate this process, if it did display signal.
				if(WEXITSTATUS(*lastExit)) {
					int termSig = WTERMSIG(*lastExit);
					printf("terminated by signal %d\n", termSig); fflush(stdout);
				}
			
			// we don't wait for background processes.
			} else {
				printf("Background pid is %d\n", spawnPID); fflush(stdout);
			}	
			break;

		}

	}

}

// This function handles interrupt signals.
//
// Input:
// 1: The signal number.
void catchSIGINT(int signo) {

	// Don't do anything.	

}

// This function handles terminal stop signals.
//
// Input:
// 1: The signal number.
void catchSIGTSTP(int signo) {

	// Switch between foreground only mode on use.	
	if(frgMode){
		char* frgOff = "Exiting foreground-only mode\n";
		write(STDOUT_FILENO, frgOff, 29);	
		frgMode = 0;
	} else {
		char* frgOn = "Entering foreground-only mode (& is now ignored)\n";
		write(STDOUT_FILENO, frgOn, 49);	
		frgMode = 1;
	}

}

// This function handles child terminating signals.
//
// Input:
// 1: The signal number.
void catchSIGCHLD(int signo) {
	
	// Get info about termination and clean up child.	
	pid_t childPID;
	int childExitMethod;
 
	// We use a while loop incase multiple children exit at the same time.	
        while ((childPID = waitpid(-1, &childExitMethod, WNOHANG)) > 0) {
		char* message = "terminated by signal 2\n";
		write(STDOUT_FILENO, message, 23);
	}
}
