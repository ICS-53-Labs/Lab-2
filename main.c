//Change name of this file to: Lab2_ID1_ID2.c"
//"lastname, firstname: studentid"
//My name
//Alex's name
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <signal.h>

 
 #define MAX_CMD_LINE 80
 #define MAX_ARGS 5
 
 #define QUIT "quit"
 
 //Function Prototypes
 void eval (char* cmdline);
 int parseLine (char* cmdline, char* argv[MAX_ARGS]);
 int builtInCommand (char* argv[MAX_ARGS]);
 void signalHandler (int sig);
 
 int main (void) {
	 printf ("This is the process id: %d\n",getpid ());
	 char cmdline[MAX_CMD_LINE];
	 
	 signal (SIGCHLD,&signalHandler);
	 while (1) {
		 printf ("Prompt> ");
		 if (!fgets (cmdline,MAX_CMD_LINE,stdin)) {
			 printf ("Fgets error\n");
		 }
		 if (feof (stdin)) {
			 exit (0);
		 }
		 printf ("%s\n",cmdline);
		 char* argv [MAX_ARGS];
		 int i = parseLine(cmdline,argv);
		 printf ("Parse: %d\n",i);
		 //eval (cmdline);
	 }
	 return 0;
 }
 
 void eval (char* cmdline) {
	 char* argv [MAX_ARGS];
	 int bg;
	 pid_t pid; //Maybe array
	 
	 bg = parseLine (cmdline, argv);
	 if (!builtInCommand (argv)) {
		 if ((pid = fork ()) == 0) {
			 if (execv (argv[0],argv) < 0) {
				 printf ("%s: Command not found.\n",argv[0]);
				 exit (0);
			 }
		 }
	 }
	 if (!bg) {
		 int status;
		 if (waitpid (pid,&status,0) < 0) {
			 //unix_error ("waitfg: waitpid error");
		 }
	 }
	 else {
		 printf ("%d %s",pid,cmdline);
	 }
 }
 
 int parseLine (char* cmdline, char* argv[MAX_ARGS]) {
	 //Parse into tokens
	 const char delim[3] = " \t";
	 int i = 0;
	 //argv is always ended with a NULL
	 argv[i] = strtok (cmdline,delim);
	 printf ("Token @ %d: %s\n",i,argv[i]);
	 while (argv[i] != NULL) {
		 if (i + 1 >= MAX_ARGS - 1) {
			 printf ("%d >= %d\n",i + 1,MAX_ARGS - 1);
			 argv[i] = NULL;
			 break;
		 }
		 argv[++i] = strtok (NULL,delim);
		 printf ("Token @ %d: %s\n",i,argv[i]);
	 }
	 //Check last char for &: ... Arg& or ... Arg &
	 char* last = argv[i - 1];
	 printf ("Last %s\n",last);
	 i = 0;
	 while (last[i] != '\0') {
		 ++i;
	 }
	 printf ("Last C:%c ''\n",last[i - 2]);
	 if (last[i - 2] == '&') {
		 return 1;
	 }
	 else {
		 return 0;
	 }
 }
 
 int builtInCommand (char* argv[MAX_ARGS]) {
	 if (!strcmp (argv[0], QUIT)) {
		 //Terminate and reap background processes OR 
		 //do not quit until all background processes are done
		 exit (0);
	 }
	 else {
		 return 0;
	 }
 }
 
 void signalHandler (int sig) {
	 //safe_printf ("Handling signal %d\n",sig);
	 int status;
	 int res = waitpid (-1,&status,WNOHANG);
	 if (res < 0) {
		 //safe_printf ("waitpid error\n");
	 }
	 else {
		 
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

