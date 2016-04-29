/* Test 2 executable object */
//Sleeps arg amount
#include <stdio.h>

int main (int argc, char* argv[]) {
	int num = 1;
	if (argc >= 2) {
		num = atoi (argv[1]);
	}
	sleep (num);
	return 0;
}