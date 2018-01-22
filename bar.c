#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NMAX 10


pthread_t barista;
pthread_t cliente[NMAX];
pthread_t generatore;

sem_t s0; //semaforo tifosi locali
sem_t s1; //semaforo tifosi ospiti
sem_t s2; //attesa
sem_t s3; //attesa_barista

typedef struct{
	int appartenenza[NMAX];
	int presenza;
	int count;
	int pulizie;
}Thread_data;

Thread_data data1;

void* pulizia(void *arg){
	Thread_data* data = (Thread_data*) arg;
	while(1){	
		sem_wait(&s3);
		sem_wait(&s2);
		printf("Pulizie in corso..\n");
		
		printf("Pulizie terminate\n");
		data->pulizie=1;
		if(data->presenza==0){	
			data->presenza=1;
			sem_post(&s1);
		}
		else{
			data->presenza=0;
			sem_post(&s0);
		}
		sem_post(&s2);
	}	
	
}
void* consuma(void* arg){
	Thread_data* data = (Thread_data*) arg;
		
	sem_wait(&s2);
	data->count++;
	
	sem_post(&s2);
	
	if(data->appartenenza[data->count] == 1 && data->presenza==0){
		printf("Ingresso non consentito al tifoso ospite, attesa..\n");
		
		sem_wait(&s1);
		sem_post(&s2);
		sem_post(&s1);
		
	}	
	else if(data->appartenenza[data->count] == 0 && data->presenza==1){
		printf("Ingresso non consentito al tifoso locale, attesa..\n");
		
		sem_wait(&s0);
		sem_post(&s2);
		sem_post(&s0);
	}
	
	
	printf("Consumazione avvenuta\n");		
	//sem_post(&s2);
	if(data->count == NMAX-1 && data->pulizie != 1){ 
		
		sem_post(&s3);
		
		
	}
	pthread_exit(NULL);
	
}

void* genera(void* arg){
	Thread_data* data = (Thread_data*) arg;
	int j = 0;
	while(j<NMAX){
		sleep(rand()%5);
		data->appartenenza[j]= rand()%2;
		
		pthread_create(&cliente[j],NULL,&consuma,&data1);
		j++;
	}
	pthread_exit(NULL);

}
int main(){
	int i;
	data1.presenza = 0; //inizialmente accetto solo tifosi locali
	data1.count = -1;
	data1.pulizie=0; //pulizie non effettuate
	sem_init(&s0,0,0);
	sem_init(&s1,0,0);
	sem_init(&s2,0,1);
	sem_init(&s3,0,0);
	pthread_create(&generatore,NULL,&genera,&data1);
	pthread_create(&barista,NULL,&pulizia,&data1);
	pthread_join(generatore,NULL);
	for(i=0;i<NMAX;i++){
		pthread_join(cliente[i],NULL);
	}
}



















