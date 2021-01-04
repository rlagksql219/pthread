#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "SSU_Sem.h"

#define NUM_THREADS 3
#define NUM_ITER 10

SSU_Sem s[3];


void *justprint(void *data)
{
	int thread_id = *((int *)data);


	SSU_Sem_down(&s[thread_id]);


	for(int i=0; i < NUM_ITER; i++)
	{
		printf("This is thread %d\n", thread_id);

		if(thread_id == 0) {
			SSU_Sem_up(&s[thread_id + 1]);
			SSU_Sem_down(&s[thread_id]);
		}
		if(thread_id == 1) {
			SSU_Sem_up(&s[thread_id + 1]);
			SSU_Sem_down(&s[thread_id]);
		}
		if(thread_id == 2) {
			SSU_Sem_up(&s[thread_id -2]);
			SSU_Sem_down(&s[thread_id]);
		}
	}

	SSU_Sem_up(&s[0]);
	SSU_Sem_up(&s[1]);
	SSU_Sem_up(&s[2]);
	
	return 0;
}


int main(int argc, char *argv[])
{

	pthread_t mythreads[NUM_THREADS];
	int mythread_id[NUM_THREADS];

	SSU_Sem_init(&s[0], 0);
	SSU_Sem_init(&s[1], 0);
	SSU_Sem_init(&s[2], 0);


	for(int i =0; i < NUM_THREADS; i++)
	{
		mythread_id[i] = i;
		pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
	}

	SSU_Sem_up(&s[0]);

	for(int i =0; i < NUM_THREADS; i++)
	{
		pthread_join(mythreads[i], NULL);
	}

	return 0;
}
