//Change name of this file to: Lab2_ID1_ID2.c"
//"lastname, firstname: studentid"
//Park, Joseph: 24136956
//ALEX NAME, ID
 
/* TODO: 
 * Output Format Issues - Especially when a background processes outputs
 * Test more thoroughly
 * Remove DEBUGging prints when done
 * Rename file to Lab2_ID1_ID2
 * Alex adds his name and ID
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

//Array size values
#define MAX_CMD_LINE 80
#define MAX_ARGS 3

//Char. and Token values
#define BACKGROUND_SET '&'
#define TOKEN_DELIM " \t"

//Built-in Command values
#define QUIT "quit"
#define ZNUM "znum"

//Debug Set
#define DEBUG 0

//Function Prototypes
/* Eval evaluates the cmdline input, creating child processes,
 * executing files, waiting and reaping proceses
 */
void eval (char* cmdline);
/* parseLine parses the cmdline input into ARGS,
 * determines whether it is background or foreground
 */
int parseLine (char* cmdline, char* argv[MAX_ARGS]);
/* builtinCommand determines which builtin command is specified, if any
 * executes the built in command if needed
 * QUIT kills and reaps remaining children before exiting
 */
int builtInCommand (char* argv[MAX_ARGS]);
/* Signal handler for SIGCHILD
 * Called whenever a Child terminates
 * Reaps all terminated background children
 * However, it is also called when a foreground child terminates
 * Because it does not block the parent process and the foreground child is already dead
 * , it has no effect
 */
void signalHandler (int sig);

//Global variable count for non-reaped children
int numChild;

int main (void) {
	if (DEBUG) printf ("Main pid: %d\n",getpid ());
	char cmdline[MAX_CMD_LINE];
	numChild = 0; /* Initialize the global variable */
	//Install Signal Handler
	if (signal (SIGCHLD,&signalHandler) == SIG_ERR) {
		printf ("Signal Error\n");
	}
	while (1) {
		printf ("PROMPT> "); /* Ask for prompt */
		if (!fgets (cmdline,MAX_CMD_LINE,stdin)) { /* Grab input */
			printf ("Fgets error\n");
		}
		if (feof (stdin)) {
			exit (0);
		}
		eval (cmdline); /* Eval input */
		sleep (1); /* small delay so that it does not mess up output format */
	}
	return 0;
}
 
 void eval (char* cmdline) {
	char* argv [MAX_ARGS];
	int bg;
	pid_t pid; //Maybe array

	bg = parseLine (cmdline, argv); /* Parse input command line */
	if (!builtInCommand (argv)) { /* Not Built in */
		if ((pid = fork ()) == 0) { /* Create child process */
			if (DEBUG) printf ("CHILD: Created %d\n",getpid ());
			if (execv (argv[0],argv) < 0) { /* Execute file */
				printf ("%s: Command not found.\n",argv[0]);
				exit (0);
			}
		}
		else {
			++numChild; /* Increase the number of children processes */
			if (DEBUG) printf ("PARENT: %d has Child %d\n",getpid(),pid);
		}
		if (!bg) { /* Foreground */
			int status;
			if (waitpid (pid,&status,0) < 0) { /* Have parent wait for child */
				//unix_error ("waitfg: waitpid error");
				printf ("PARENT: waitpid error by parent in foreground\n");
			}
			else {
				--numChild; /* Wait reaps child on termination, thus decrease */
				if (DEBUG) printf ("PARENT: Child %d reaped in foreground\n",pid);
			}
		}
		else { /* If background, do nothing and continue */
			if (DEBUG) printf ("PARENT: Background Child %d, Executing %s\n",pid,cmdline);
		}
	}
 }
 
//Tokenizes cmdline into argv based on whitespace delimiters.
//Checks for & at the end of input no matter what
//There is a limit to the number of tokens that can be stored
int parseLine (char* cmdline, char* argv[MAX_ARGS]) {
	const char delim[3] = TOKEN_DELIM;
	int bg = 0;
	//Find size of cmdline
	int i = 0;
	while (cmdline[i] != '\0') {
		++i;
	}
	
	//From the end, find the first non-whitespace characters
	--i; //Ignore null
	cmdline[i] = '\0';
	--i; //Ignore carriage return
	//Check last char for &: ... Arg& or ... Arg &
	while (cmdline[i] == ' ' || cmdline[i] == '\t') {
		--i;
	}
	if (cmdline[i] == BACKGROUND_SET) {
		bg = 1;
		cmdline[i] = '\0'; //Remove the ampersand
	} 
	//argv is always ended with a NULL
	//Parse into tokens
	i = 0;
	argv[i] = strtok (cmdline,delim);
	while (argv[i] != NULL) {
		if (i >= MAX_ARGS - 1) {
			argv[i] = NULL;
			break;
		}
		argv[++i] = strtok (NULL,delim);
	}
	return bg;
}

int builtInCommand (char* argv[MAX_ARGS]) {
	if (!strcmp (argv[0], QUIT)) { /* QUIT */
		//If there are remaining child processes
		if (numChild > 0) {
			if (DEBUG) printf ("MAIN: Child processes not yet reaped: %d\n",numChild);
			if (DEBUG) printf ("MAIN: Killing and reaping remaining child processes\n");
			//Kill all children but not parent
			signal(SIGQUIT,SIG_IGN);
			if (kill (0,SIGQUIT) < 0) {
				printf ("MAIN: Error killing all children\n");
			}
			//Reap all zombie children
			int status;
			while (numChild > 0) {
				int res = waitpid (-1,&status,0);
				if (res > 0) {
					if (DEBUG) printf ("MAIN: Child %d reaped with status %d\n",res,status);
					--numChild;
				}
				else {
					printf ("MAIN: Waitpid Error\n");
				}
			}
		}
		//By now, all children should be terminated and reaped
		if (DEBUG) printf ("MAIN: Remaining children: %d\n",numChild);
		exit (0);
	}
	else if (!strcmp (argv[0], ZNUM)) { /* ZNUM to check number of non-reaped children */
		printf ("MAIN: Number of potential zombie child processes (not yet reaped): %d\n",numChild);
		return 1;
	}
	else {
		return 0;
	}
}

void signalHandler (int sig) {
	if (DEBUG) printf ("PARENT: Handling signal %d\n",sig);
	int status;
	int res = waitpid (-1,&status,WNOHANG);
	if (res < 0) {
		//printf ("PARENT: Child %d exited with status %d\n",res,status);
	}
	//While Loop so that in case a new child termination signal 
	//is sent while reaping, it can reap that one too
	while (res > 0) {
		--numChild;
		if (DEBUG) printf ("PARENT: Child %d exited with status %d\n",res,status);
		if (DEBUG) printf ("PARENT: Child %d reaped in background\n",res);
		res = waitpid (-1,&status,WNOHANG); /* WNOHANG option so that it does not block the parent */
	}
}
 
/* Process
INITIALIZE:
Install Signal Handler
-Signal Handler: If SIGCHILD captured, wait WNOHANG to reap Child
INTERPRET:
Read Input
-Get User Input
-Ignore Whitespaces if necessary
-Increase Buffer size if necessary
Parse Input
-Validate input
-Categorize input
Execute Command
-If Built in, Perform appropriate built in command
-QUIT: GO TO TERMINATE
-If General, 
-Begin New Process, Store child pid
-Execute File with Arguments
-Check if Background, & regardless
-Foreground: Parent Wait for Child then Reap it.
-Background: Move on.
TERMINATE
-IF Background Child exists, Kill them
-Reap Killed Children
-Free Malloc Mem
-Exit

NOTE: Input format errors ~Ignore or Reject
-Ignore extra whitespaces and lines
-Produce same output as tcsh
-Max lines on screen is 25
-Max characters per line is 80
-Commands terminated by \n
-Commands are of the form COMMAND [arg] [arg] ... \n

Outputs:
Print after command
Print errors
Print after killing, reaping
Print after forking, exec
*/

