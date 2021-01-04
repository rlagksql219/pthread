#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include "SSU_Sem.h"

int front = 0, rear = 0;


void SSU_Sem_init(SSU_Sem *s, int value)
{
	s->value = value;
	pthread_mutex_init(&(s->mutex1), NULL);
	pthread_cond_init(&(s->cond1), NULL);
}

void SSU_Sem_down(SSU_Sem *s) 
{
	pthread_t id = 0;
	id = pthread_self();

	pthread_mutex_lock(&(s->mutex1));
	s->value--;

	if(s->value < 0) { //enqueue
		pthread_cond_wait(&(s->cond1), &(s->mutex1));
		s->queue[rear] = id;
		rear = ++rear % 100;
	}
	pthread_mutex_unlock(&(s->mutex1));
}


void SSU_Sem_up(SSU_Sem *s) 
{
	pthread_t id = 0;

	pthread_mutex_lock(&(s->mutex1));
	s->value++;
	pthread_cond_signal(&(s->cond1));

	//dequeue
	id = s->queue[front];
	front = ++front % 100;
	pthread_mutex_unlock(&(s->mutex1));
}
