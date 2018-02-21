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

// Function declarations.
int userInput(char*, size_t, char**);
void perfComm(char**, pid_t*, int*);
void smallExit();
void smallCd(char**);
void smallStatus(char**, pid_t*, int*);
void forkExe(char**, pid_t*, int*);
