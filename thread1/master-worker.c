#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;


pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;

int item_to_produce, curr_buf_size;
int master_buf_cursor = 0, worker_buf_cursor = 0;
int current_item = 0;
int total_items, max_buf_size, num_workers, num_masters;
int consume_mark = 0;

int* buffer;

void print_produced(int num, int master) {

	printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

	printf("Consumed %d by worker %d\n", num, worker);

}


//produce items and place in buffer
//modify code below to synchronize correctly
void* generate_requests_loop(void* data)
{
	int thread_id = *((int*)data);


	while (1)
	{


		pthread_mutex_lock(&lock);
		if (item_to_produce >= total_items) {
			pthread_mutex_unlock(&lock);
			break;
		}
		if (current_item >= max_buf_size)
		{
			pthread_cond_broadcast(&cond2);
			pthread_cond_wait(&cond3, &lock);
			pthread_mutex_unlock(&lock);
			continue;
		}
		pthread_mutex_lock(&lock3);
		buffer[master_buf_cursor] = item_to_produce;
		print_produced(item_to_produce, thread_id);
		item_to_produce++;

		master_buf_cursor = (master_buf_cursor + 1) % max_buf_size;
		current_item++;

		pthread_mutex_unlock(&lock3);
		pthread_mutex_unlock(&lock);
	}

	pthread_cond_broadcast(&cond2);
	return 0;
}


void* execute_worker_thread(void* data)
{
	int thread_id = *((int*)data);




	while (1)
	{
		pthread_mutex_lock(&lock2);

		if (current_item == 0 && item_to_produce >= total_items)
		{
			pthread_mutex_unlock(&lock2);
			break;
		}

		if (current_item <= 0)
		{
			pthread_cond_broadcast(&cond3);
			pthread_cond_wait(&cond2, &lock2);
			pthread_mutex_unlock(&lock2);
			continue;
		}

		pthread_mutex_lock(&lock3);
		print_consumed(buffer[worker_buf_cursor], thread_id);
		buffer[worker_buf_cursor] = 0;
		worker_buf_cursor = (worker_buf_cursor + 1) % max_buf_size;

		current_item--;
		pthread_mutex_unlock(&lock3);
		pthread_mutex_unlock(&lock2);
	}


	return 0;
}


//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item

int main(int argc, char* argv[])
{
	int* master_thread_id;
	pthread_t* master_thread;
	int* worker_thread_id;
	pthread_t* worker_thread;
	item_to_produce = 0;
	curr_buf_size = 0;

	int i, j;

	if (argc < 5) {
		printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
		exit(1);
	}
	else {
		num_masters = atoi(argv[4]);
		num_workers = atoi(argv[3]);
		total_items = atoi(argv[1]);
		max_buf_size = atoi(argv[2]);
	}

	buffer = (int*)malloc(sizeof(int) * max_buf_size);

	//create master producer threads
	master_thread_id = (int*)malloc(sizeof(int) * num_masters);
	master_thread = (pthread_t*)malloc(sizeof(pthread_t) * num_masters);
	for (i = 0; i < num_masters; i++)
		master_thread_id[i] = i;

	for (i = 0; i < num_masters; i++)
		pthread_create(&master_thread[i], NULL, generate_requests_loop, (void*)&master_thread_id[i]);

	//create worker consumer threads
	worker_thread_id = (int*)malloc(sizeof(int) * num_workers);
	worker_thread = (pthread_t*)malloc(sizeof(pthread_t) * num_workers);
	for (j = 0; j < num_workers; j++)
		worker_thread_id[j] = j;

	for (j = 0; j < num_workers; j++)
		pthread_create(&worker_thread[j], NULL, execute_worker_thread, (void*)&worker_thread_id[j]);

	//wait for all threads to complete
	for (i = 0; i < num_masters; i++)
	{
		pthread_join(master_thread[i], NULL);
		printf("master %d joined\n", i);
	}
	pthread_cond_broadcast(&cond2);
	for (j = 0; j < num_workers; j++)
	{
		pthread_join(worker_thread[j], NULL);
		printf("worker %d joined\n", j);
	}

	/*----Deallocating Buffers---------------------*/
	free(buffer);
	free(master_thread_id);
	free(master_thread);
	free(worker_thread_id);
	free(worker_thread);

	return 0;
}
