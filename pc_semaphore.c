#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int in=0;
int out=0;

sem_t empty_slot;
sem_t data_items;

pthread_mutex_t mutex;

void *producer(void *arg){
	int i, produced_item;
	i = 0, produced_item = 0;
	
	while(i<BUFFER_SIZE){
		
		sem_wait(&empty_slot);
		
		pthread_mutex_lock(&mutex);
		buffer[in] = produced_item;
		printf("Producer produced item : %d in buffer [%d]\n",produced_item,in);
		in = (in+1)%BUFFER_SIZE;
		produced_item++;
		usleep(1);
		i++;
		pthread_mutex_unlock(&mutex);
		sem_post(&data_items);
	}
}

void *consumer(void *arg){
	int i, consumed_item;
	i=0;
	
	while(i<BUFFER_SIZE){
		sem_wait(&data_items);
		pthread_mutex_lock(&mutex);
		consumed_item = buffer[out];
		printf("Consumer consumed item : %d in buffer [%d]\n",consumed_item,out);
		out = (out+1)%BUFFER_SIZE;
		usleep(1);
		i++;
		pthread_mutex_unlock(&mutex);
		sem_post(&empty_slot);
	}
}

int main(){
	int i;
	pthread_t producer_threads[2];
	pthread_t consumer_threads[2];
	
	pthread_mutex_init(&mutex,NULL);
	sem_init(&data_items,0,0);
	sem_init(&empty_slot,0,BUFFER_SIZE);

	pthread_create(&producer_threads[0],NULL,producer,NULL);
	pthread_create(&producer_threads[1],NULL,consumer,NULL);
	pthread_create(&consumer_threads[0],NULL,producer,NULL);
	pthread_create(&consumer_threads[1],NULL,consumer,NULL);

	for(i=0;i<2;i++){
		pthread_join(producer_threads[i],NULL);
		pthread_join(consumer_threads[i],NULL);
	}
	pthread_exit(NULL);
	
	sem_destroy(&empty_slot);
	sem_destroy(&data_items);

	return 0;
	
}









