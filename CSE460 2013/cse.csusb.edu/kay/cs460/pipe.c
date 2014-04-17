#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

main()
{
	int pd[2];
	int pid;

	if (pipe(pd) == -1) {
		printf("pipe creation failed.\n");
		exit(1);
	}
	if (pid = fork()) { // parent
		printf("Parent says input a character (with pid=%d): ", pid);
		char x;
		scanf("%c", &x);
		if (write(pd[1], &x, 1) <= 0) {
			printf("write error\n");
			exit(2);
		}
		printf("Parent process done\n");
	} else { // child
		printf("Child process with pid = %d\n", pid);
		char get;
		if (read(pd[0], &get, 1) <= 0) {
			printf("read error\n");
			exit(3);
		}
		printf("Child unblocked, got = %c\n", get);
	}
} // main
