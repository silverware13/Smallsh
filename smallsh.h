// Assignment #3 header file.
// 
// Used to define functions,
// variables, and constants.
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
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

// Waits for and does basic validation of user input.
int userInput(char*, size_t, char**);

// Performs a given command.
void perfComm(char**, pid_t*, int*);

// Kills all children and exits the process.
void smallExit();

// Changes working directory to given directory, if none is given changes to HOME directory.
void smallCd(char**);

// Shows exit status or terminating signal of the last foreground process.
void smallStatus(char**, pid_t*, int*);

// Forks and then executes a given command.
void forkExe(char**, pid_t*, int*);

// This function handles interrupt signals.
void catchSIGINT(int);

// This function handles terminal stop signals.
void catchSIGTSTP(int);

// This function handles child terminating signals.
void catchSIGCHLD(int);
