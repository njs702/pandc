#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

void *producer(void* arg){
	int i, produced_item;
	i=0;
	produced_item = 0;
	while(i<BUFFER_SIZE){
		buffer[in] = produced_item;
		printf("Producer produced item : %d in buffer[%d]\n",produced_item,in);
		in = (in+1)%BUFFER_SIZE;
		produced_item++;
		usleep(1);
		i++;
	}
}

void *consumer(void* arg){
	int i, consumed_item;
	i=0;
	while(i<BUFFER_SIZE){
		consumed_item = buffer[out];
		printf("Consumer consumed item : %d in buffer[%d]\n",consumed_item,out);
		out = (out+1)%BUFFER_SIZE;
		usleep(1);
		i++;
	}
}

int main(){
	int i;
	pthread_t producer_threads[2];
	pthread_t consumer_threads[2];
	pthread_create(&producer_threads[0],NULL,producer,NULL);
	pthread_create(&producer_threads[1],NULL,consumer,NULL);
	pthread_create(&consumer_threads[0],NULL,producer,NULL);
	pthread_create(&consumer_threads[1],NULL,consumer,NULL);

	for(i=0;i<2;i++){
		pthread_join(producer_threads[i],NULL);
		pthread_join(consumer_threads[i],NULL);
	}
	pthread_exit(NULL);
	return 0;
}
