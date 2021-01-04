#include <pthread.h>

typedef struct SSU_Sem {
	int value;
	pthread_mutex_t mutex1;
	pthread_cond_t cond1;
	pthread_t queue[100];
} SSU_Sem;

void SSU_Sem_init(SSU_Sem *, int);
void SSU_Sem_up(SSU_Sem *);
void SSU_Sem_down(SSU_Sem *);
