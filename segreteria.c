#include <stdio.h>
#include <stdlib.h> //rand()
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX 10
#define NUM_THREADS 20

typedef struct {
	int array[MAX];
	int front;
	int rear;
	int itemCount;
}Coda;

typedef struct {
    	Coda coda;
	int appartenenza;

}Thread_data;

Thread_data data; //dati thread coda A
Thread_data data1;//dati thread coda B
sem_t s;

int isEmpty(Coda coda) {
	return coda.itemCount == 0;
}

int isFull(Coda coda) {
   	return coda.itemCount == MAX;
}

int size(Coda coda) {
   	return coda.itemCount;
}  

Coda insert(int data,Coda coda) {

	if(!isFull(coda)) {
      		if(coda.rear == MAX-1) {
		 	coda.rear = -1;            
	      	}     
      	coda.array[++coda.rear] = data;
      	coda.itemCount++;
	return coda;
   	}
}

Coda removeData(Coda coda) {
   	if (!isEmpty(coda)) {
		if(coda.front == MAX) {
		      coda.front = 0;
		}
	int data = coda.array[coda.front++];
   	coda.itemCount--;
	return coda;
   	} 
}

void* gestione(void *arg){
	Thread_data *data= (Thread_data*) arg;

	sem_wait(&s); //attendi fino a che la coda non è in uso

	data->coda=insert(1, data->coda);
	
	sem_post(&s); //sblocca semaforo

	//printf("Rear: %d Size: %d ", data->coda.rear, size(data->coda));
	printf("Fila: %d - Nuovo Studente in fila\n", data->appartenenza);
	
	sleep(5);	
	
	sem_wait(&s); //attendi fino a che la coda non è in uso

	data->coda=removeData(data->coda); //attendi fino a che la coda non è in uso

	sem_post(&s);

	//printf("Front: %d Size: %d\n", data->coda.front, size(data->coda));
	printf("Fila: %d - Esce uno Studente dalla fila\n", data->appartenenza);
}

int main() {
	int i = 0;
	data.appartenenza=1;
	data1.appartenenza=2;
	
	pthread_t thread_id[NUM_THREADS];
	sem_init(&s, 0, 1);
	while(i<NUM_THREADS){
		sleep(rand() % 5); //creazione random tra 0-5 sec
		switch(rand()%2){//selezione random tra 0 e 1
		case 0:
			pthread_create (&thread_id[i], NULL, &gestione, &data); //utente coda A
			i++;
			break;
		case 1:
			pthread_create (&thread_id[i], NULL, &gestione, &data1); //utente coda B
			i++;
			break;
		
		}
		
	}

	for (i=0; i < NUM_THREADS; i++)
		pthread_join (thread_id[i], NULL);

	sem_destroy(&s); //rilascia risorse per il semaforo
	
}







