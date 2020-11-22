# pandc

#### 1.Producer/Consumer with no_semaphore
```cpp
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE]; //data 담을 buffer
int in = 0;
int out = 0;

```
Producer/Consumer with no_semaphore 프로그램 구현을 위한 헤더파일, 매크로, 전역변수 선언
```cpp
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
```
세마포 기능이 없는 producer function이다. 아이템은 0부터 순차적으로 증가하며 버퍼에 저장된다. Race condition을 만들기 위해 usleep(1)을 통해 잠시 sleep하도록 했다. Ring buffer 형식으로 버퍼를구현해서 아이템이 비워지고 채워지도록 하였다.
```cpp
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
```
세마포 기능이 없는 consumer function이다. 버퍼에서 아이템을 꺼내 와서 소비한 뒤, 그 메시지를 출력한다.
```cpp
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
```
main function이다. Producer thread 2개와 Consumer thread 2개를 생성해서 join해 race condition이 발생하는지 확인하도록 했다.
#### 2.Producer/Consumer with semaphore
```cpp
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
```
Producer/Consumer with semaphore 프로그램 구현을 위한 헤더파일, 매크로, 전역변수 선언
```cpp
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
```
기존의 Producer와 다르게, sem_wait과 sem_post 등의 세마포 기능과, pthread_mutex_lock(&mutex) 락을 통해 세마포 기능을 구현했다. Race condition이 발생하는 동일 조건에서 정상적으로 동작한다.
```cpp
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
```
Producer와 마찬가지로 Consumer도 세마포 함수들과 mutex를 이용해서 세마포 기능을 구현했다. 마찬가지로 Race condition이 발생하더라도 정상적으로 동작한다.
