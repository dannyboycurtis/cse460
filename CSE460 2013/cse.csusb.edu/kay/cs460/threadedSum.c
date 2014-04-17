/************************************************************************
 * threadedSum.c
 * adapted from "OS Concepts" by Silberschatz etc. Fig 4.9
 * main thread spawns a child thread that computes the sum
 * compile:         $ g++ -pthread threadedSum.c
 * run:             $ a.out 5
 * returns/outputs: 15
 ************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *runner(void *param); /* the thread */
int sum;                   /* shared data */

int main(int argc, char **argv)
{
	pthread_t tid;
	pthread_attr_t attr;

	if (argc != 2) {
		fprintf(stderr, " Usage: a.out <integer value>\n");
		return -1;
	}
	if (atoi(argv[1]) < 0) {
		fprintf(stderr, "%d must be >= 0\n", atoi(argv[1]));
		return -1;
	}

	pthread_attr_init(&attr);                     /* get the default attributes such as stack size and scheduling info */
	pthread_create(&tid, &attr, runner, argv[1]); /* create the thread */
	pthread_join(tid, NULL);                      /* wait for the thread to exit */

	printf("sum = %d\n", sum);
}

/* the thread will begin execution in this function */
void *runner(void *param)
{
	int i, upper = atoi((char *)param);
	sum = 0;

	for (i = 1; i <= upper; i++)
		sum += i;

	pthread_exit(0);
}
