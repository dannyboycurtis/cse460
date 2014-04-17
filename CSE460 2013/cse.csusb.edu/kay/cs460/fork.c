#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

main()
{
	int x;
	if ((x = fork()) != 0) { 
		// parent
		printf("In parent fork() returned = %d\n", x);
		printf("Parent process external process id = %d\n", getpid());
	} else { 
		// child
		printf("In child fork() returned = %d\n", x);
		printf("Child process external process id = %d\n", getpid());
	}
} // main
