#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_CLIENTI 10
pthread_t cuoco;
pthread_t addetto;
pthread_t cliente[NUM_CLIENTI];
pthread_t cassiere;
pthread_t t0;


sem_t attesa;//addetto - cuoco
sem_t attesa_2;//sblocca addetto
sem_t attesa_cliente;//addetto - cliente
sem_t attesa_3;//sblocca cassa
sem_t attesa_cassa; //cassa-cliente


typedef struct {
	int scaffale;
	int ordine;
}thread_data;

thread_data data;


void* preparazione(void *arg){
	thread_data *data = (thread_data*) arg;
	while(1){
		
		//sem_wait(&attesa);//attendo che l'addetto non serva nessuno
		if(data->scaffale==20){
			//printf("Scaffale pieno\n");
			sem_post(&attesa);//mentre è pieno lo scaffale, addetto servi i clienti
		}

		else {
			
			data->scaffale+=1;
			printf("1 panino aggiunto - panini disponibili adesso: %d\n",data->scaffale);
			sem_post(&attesa); //Libera l'addetto
			
		}
		
	}		
}

void* servizio(void *arg){
	thread_data *data= (thread_data*) arg;
	while(1){
		
		sem_wait(&attesa); //attendi che il cuoco sia libero
		sem_wait(&attesa_2);//attendo ordinazione
		
		
		if (data->scaffale<data->ordine){
			printf("Attendo preparazione %d panini\n",data->ordine-data->scaffale);
			//sem_wait(&attesa_cliente);
			sem_post(&attesa);//attendo che il cuoco mi prepari i panini
			
		}
		else {

			data->scaffale-=data->ordine;
			printf("cliente servito - %d panini rimanenti\n",data->scaffale);
			sem_post(&attesa); //libera cuoco
			sem_post(&attesa_3);//servizio finito
			sem_post(&attesa_cliente);//attendo nuovo cliente
			
			
		}
	}
	
	
}
void* ordinazione(void *arg){
	thread_data *data= (thread_data*) arg;
	sem_wait(&attesa_cassa); //sol dopo aver pagato posso ordinare
	sem_wait(&attesa_cliente);//aspetto che l'addetto si metta in attesa di 1 cliente
	
	
	data->ordine=((rand() % 3)+1);
	printf("ordinato/i %d panini\n",data->ordine);
	
	sem_post(&attesa_2); //il cliente può essere servito
	
}
void* generate (void *arg){
	//Thread_data *data= (Thread_data*) arg;
	int i=0; 
	while(i<NUM_CLIENTI){
		sleep(rand()%5);
		pthread_create(&cliente[i], NULL, &ordinazione, &data);
		
		i++;
	}
	return NULL;
}
void* cassa(void *arg){
	thread_data *data= (thread_data*) arg;
	int i=0;
	while(i<NUM_CLIENTI){
		sem_wait(&attesa_3); //aspetto che il servizio sia finito
		printf("un cliente ha lo scontrino\n");
		
		sem_post(&attesa_cassa);//il pagamento è avvenuto
		i++;
		
	}
}
int main() {
	int i=0;
	data.scaffale=0;
	data.ordine=0;
	
	sem_init(&attesa_2,0,0);
	sem_init(&attesa_cliente,0,1);
	sem_init(&attesa,0,1);
	sem_init(&attesa_cassa,0,0);
	sem_init(&attesa_3,0,1);

	pthread_create(&cuoco,NULL,&preparazione,&data);
	pthread_create(&t0,NULL,&generate,NULL);
	

	pthread_create(&addetto,NULL,&servizio,&data);
	pthread_create(&cassiere,NULL,&cassa,&data);
	
	
	pthread_join(t0,NULL);
	for(i=0;i<NUM_CLIENTI;i++)
		pthread_join(cliente[i],NULL);
	
			
}









