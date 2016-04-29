//Change name of this file to: Lab2_ID1_ID2.c"
//"lastname, firstname: studentid"
//Park, Joseph: 24136956
//ALEX NAME, ID
 
/* TODO: 
 * Output Format Issues - Especially when a background processes outputs
 * Test more thoroughly
 * Remove debugging prints when done
 * Rename file to Lab2_ID1_ID2
 * Alex adds his name and ID
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

 
#define MAX_CMD_LINE 80
#define MAX_ARGS 3

#define BACKGROUND_SET '&'
#define TOKEN_DELIM " \t"

#define QUIT "quit"
#define ZNUM "znum"

//Function Prototypes
void eval (char* cmdline);
int parseLine (char* cmdline, char* argv[MAX_ARGS]);
int builtInCommand (char* argv[MAX_ARGS]);
void signalHandler (int sig);

//Global variable count for non-reaped children
int numChild;

int main (void) {
	printf ("Main pid: %d\n",getpid ());
	char cmdline[MAX_CMD_LINE];
	numChild = 0;
	if (signal (SIGCHLD,&signalHandler) == SIG_ERR) {
		printf ("Signal Error\n");
	}
	while (1) {
		printf ("PROMPT> ");
		if (!fgets (cmdline,MAX_CMD_LINE,stdin)) {
			printf ("Fgets error\n");
		}
		if (feof (stdin)) {
			exit (0);
		}
		/*
		printf ("cmdline: %s\n",cmdline);
		char* argv [MAX_ARGS];
		int i = parseLine(cmdline,argv);
		printf ("Parse: %d\n",i);
		int j = 0;
		while (argv[j] != NULL) {
		printf ("argv @ %d: %s\n",j,argv[j]);
		++j;
		}
		int r = builtInCommand (argv);
		printf ("r: %d\n",r);
		*/
		eval (cmdline);
		sleep (1);
	}
	return 0;
}
 
 void eval (char* cmdline) {
	char* argv [MAX_ARGS];
	int bg;
	pid_t pid; //Maybe array

	bg = parseLine (cmdline, argv);
	/*
	printf ("Arguments:\n");
	int j = 0;
	while (argv[j] != NULL) {
	 printf ("argv @ %d: %s\n",j,argv[j]);
	 ++j;
	}
	*/
	if (!builtInCommand (argv)) {
		if ((pid = fork ()) == 0) {
			printf ("CHILD: Created %d\n",getpid ());
			if (execv (argv[0],argv) < 0) {
				printf ("%s: Command not found.\n",argv[0]);
				exit (0);
			}
		}
		else {
			++numChild;
			printf ("PARENT: %d has Child %d\n",getpid(),pid);
		}
		if (!bg) {
			 
			int status;
			if (waitpid (pid,&status,0) < 0) {
				//unix_error ("waitfg: waitpid error");
				printf ("PARENT: waitpid error by parent in foreground\n");
			}
			else {
				--numChild;
				printf ("PARENT: Child %d reaped in foreground\n",pid);
			}
		}
		else {
			printf ("PARENT: Background Child %d, Executing %s\n",pid,cmdline);
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
	//printf ("size of cmdline:%d\n",i);
	//From the end, find the first non-whitespace characters
	--i; //Ignore null
	cmdline[i] = '\0';
	--i; //Ignore carriage return
	while (cmdline[i] == ' ' || cmdline[i] == '\t') {
		//printf ("char: %c\n",cmdline[i]);
		--i;
	}
	//printf ("first nonc:%c, %d\n",cmdline[i],i);
	if (cmdline[i] == BACKGROUND_SET) {
		bg = 1;
		cmdline[i] = '\0'; //Remove the ampersand
	} 
	//argv is always ended with a NULL
	//Parse into tokens
	i = 0;
	argv[i] = strtok (cmdline,delim);
	//printf ("Token @ %d: %s\n",i,argv[i]);
	while (argv[i] != NULL) {
		if (i >= MAX_ARGS - 1) {
			//printf ("%d >= %d\n",i,MAX_ARGS - 1);
			argv[i] = NULL;
			break;
		}
		argv[++i] = strtok (NULL,delim);
		//printf ("Token @ %d: %s\n",i,argv[i]);
	}
	//Check last char for &: ... Arg& or ... Arg &
	/*
	char* last = argv[i - 1];
	//printf ("Last %s\n",last);
	i = 0;
	while (last[i] != '\0') {
	++i;
	}
	//printf ("Last C:%c ''\n",last[i - 2]);
	if (last[i - 2] == '&') {
	return 1;
	}
	else {
	return 0;
	}
	*/
	return bg;
}

int builtInCommand (char* argv[MAX_ARGS]) {
	if (!strcmp (argv[0], QUIT)) {
		//Terminate and reap background processes OR 
		//do not quit until all background processes are done
		if (numChild > 0) {
			printf ("MAIN: Child processes not yet reaped: %d\n",numChild);
		}
		exit (0);
	}
	else if (!strcmp (argv[0], ZNUM)) {
		printf ("MAIN: Number of potential zombie child processes (not yet reaped): %d\n",numChild);
		return 1;
	}
	else {
		return 0;
	}
}

void signalHandler (int sig) {
	printf ("PARENT: Handling signal %d\n",sig);
	int status;
	int res = waitpid (-1,&status,WNOHANG);
	if (res < 0) {
		//printf ("Child %d exited with status %d\n",res,status);
		//printf ("waitpid error\n");
	}
	while (res > 0) {
		--numChild;
		printf ("PARENT: Child %d exited with status %d\n",res,status);
		printf ("PARENT: Child %d reaped in background\n",res);
		res = waitpid (-1,&status,WNOHANG);
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

