/* Test 1 executable object */
//Print statements from 1 to arg
#include <stdio.h>

int main (int argc, char* argv[]) {
	int num = 0;
	if (argc >= 2) {
		num = atoi (argv[1]);
	}
	int i = 0;
	while(i <= num) {
		printf ("Printing at %d\n",i);
		sleep (1);
		++i;
	}
	return 0;
}