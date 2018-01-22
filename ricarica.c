#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_CLIENTI 10
#define RICARICA_10EURO 0
#define RICARICA_20EURO 1

typedef struct {
	int ricarica_0;
	int ricarica_1;
}Thread_data;

pthread_t generatore; //genera i clienti in tempi casuali
pthread_t cliente[NUM_CLIENTI]; //preleva un taglio da 10 o 20
pthread_t gestore; //carica ricariche nel distributore

sem_t manutenzione; //viene decrem se è in corso
sem_t attesa; //attesa tra un thread e l'altro

Thread_data data1;

void* preleva(void *arg){ //permette ad un consumatore di prelevare una ricarica del taglio desiderato;
	Thread_data *data = (Thread_data*) arg;
	
	sem_wait(&attesa);

	int i = rand()%2;

	if(i == RICARICA_10EURO && data->ricarica_0 >= 1){
		data->ricarica_0--;
		printf("Prelevata ricarica da 10€\n");
	}
	else if(i == RICARICA_20EURO && data->ricarica_1 >= 1){
		data->ricarica_1--;
		printf("Prelevata ricarica da 20€\n");
	}
	
	
	int r = ((rand()%10)+1);
	if (r>=9){ //10% probabilità manutenzione
		//sem_wait(&attesa);
		sem_post(&manutenzione);
	}
	else
		sem_post(&attesa);	
	
	pthread_exit(NULL);
	
	
}

void* carica(void *arg){ //permette al gestore di ripristinare il contenuto del distributore
	Thread_data *data = (Thread_data*) arg;
	while(1){
		sem_wait(&manutenzione);
		printf("In manutenzione\n");
		printf("Disponibilità: ricariche da 10€: %d, ricariche da 20€: %d\n",data->ricarica_0, 
		data->ricarica_1);

		if (data->ricarica_0<100)
			data->ricarica_0=100;

		if (data->ricarica_1<100)
			data->ricarica_1=100;
		printf("Disponibilità: ricariche da 10€: %d, ricariche da 20€: %d\n",data->ricarica_0, 
		data->ricarica_1);
		printf("manutenzione terminata\n");
		sem_post(&attesa);
	}

}

void* genera(void *arg){
	int i = 0;
	
	while (i<NUM_CLIENTI){
		sleep(rand()%5);
		pthread_create(&cliente[i],NULL,&preleva,&data1);
		i++;
	}
	pthread_exit(NULL);
}

int main(){
	int i;
	data1.ricarica_0=100;
	data1.ricarica_1=100;

	sem_init(&attesa,0,1);
	sem_init(&manutenzione,0,0);
	
	pthread_create(&gestore,NULL,&carica,&data1);
	pthread_create(&generatore,NULL,&genera,NULL);
	
	pthread_join(generatore,NULL);
	for (i=0; i<NUM_CLIENTI; i++)
		pthread_join(cliente[i],NULL);
}






















