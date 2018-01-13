#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX 10
#define NUM_THREADS 5

typedef struct {
	int array[MAX];
	int front;
	int rear;
	int itemCount;
	} Coda;

typedef struct {
	Coda coda;
	int tid;
	} Thread_data;

Thread_data data1;
pthread_t t0;
pthread_t t_id[NUM_THREADS];
pthread_t tc;
pthread_t pthread_self(void); //funzione utile a sapere il thread_id

sem_t s;

int isEmpty(Coda coda){
	return coda.itemCount == 0;
}
int isFull(Coda coda){
	return coda.itemCount == MAX;
}
Coda enqueue(Coda coda, int data){
	if (!isFull(coda)){
		if(coda.rear == MAX-1) {
		 	coda.rear = -1;            
	      	}
		coda.array[++coda.rear];
		coda.itemCount++;
		return coda;
	}
}

Coda dequeue(Coda coda) {
   	if (!isEmpty(coda)) {
		if(coda.front == MAX) {
		      coda.front = 0;
		}
		int data = coda.array[coda.front++];
	   	coda.itemCount--;
		return coda;
   	} 
}

void* control (void *arg){
	Thread_data *data= (Thread_data*) arg;
	sem_wait(&s);
	data->tid=pthread_self();
	printf("Thread %d:\n",data->tid);
	data->coda=enqueue(data->coda, data->tid);
	printf("Inserimento elemento coda avvenuto\n");
	sem_post(&s);
}

void* generate (void *arg){
	//Thread_data *data= (Thread_data*) arg;
	int i=0; 
	while(i<NUM_THREADS){
		sleep(rand()%5);
		pthread_create(&t_id[i], NULL, &control, &data1);
		printf("Creato %d° thread\n",i+1);
		i++;
	}
}

void* view (void *arg){
	
	Thread_data *data= (Thread_data*) arg;
	sem_wait(&s);
	while(!isEmpty(data->coda)){
		data->coda= dequeue(data->coda);
		printf("Eliminazione elemento coda avvenuto\n");
	}
	sem_post(&s);
}
int main(){
	int j;
	sem_init(&s,0,1);
	pthread_create(&t0, NULL, &generate, NULL); //creo t0
	printf("Creato thread t0\n");
	pthread_join(t0,NULL);
	for(j=0;j<NUM_THREADS;j++)
		pthread_join(t_id[j],NULL);
	pthread_create(&tc, NULL, &view, &data1); // creo tc
	printf("Creato thread tc\n");
	pthread_join(tc,NULL);
	
	
	
}












