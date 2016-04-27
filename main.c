/* Things needed:
Input Reader
Whitespace Ignores
Buffer Increases
Input Parser
Argument Grabbing
Input Validator
Input Categorizer
Built in Executor
General Executor
Forking
Executing
Background Checks
Child Reaping
SIGCHILD Handler
Signal Installing
Foreground Waits
WNOHANG Reaping
*/
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

#include <std
