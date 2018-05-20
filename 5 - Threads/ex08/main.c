#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>

#define SIZE 1000
#define TH 5
pthread_mutex_t mutex;												//mutex para controlar acesso ao index da estrutura
pthread_cond_t cond;												//variavel de condiçao para sincronizar a escrita das threads



typedef struct {					//struct para enviar dados para a thread_func
	int numbers[SIZE];										//array com os valores
	int result[SIZE];										//array que sera preenchido com os resultados	
	int thr_nr;												//nr da thread
	int write;												//variavel para as threads saberem se podem escrever
} data;																												


void* thread_func(void *arg){
	data *value = ((data*) arg);									//converter o argumento para um apontador da struct
	
	pthread_mutex_lock(&mutex);										//faz lock ao mutex
	int thr = value->thr_nr;										//guarda o nr da thread 
	value->thr_nr++;												//incrementa o nr da thread 
	pthread_mutex_unlock(&mutex);									//desbloqueia o mutex

	int start = thr*(SIZE/TH);										//calcula o inicio da pesquisa do array para cada thread de acordo com o seu nr
	int end = start+(SIZE/TH);										//calcula o fim
	
	for(int i = start; i<end; i++){									//faz os calculos
		value->result[i] = value->numbers[i]*2+10;
	}
	
	pthread_mutex_lock(&mutex);										//faz lock ao mutex
	while(value->write != thr){										//se o nr da thread for diferente do nr de write bloqueia
		pthread_cond_wait(&cond, &mutex);
	}
	
	printf("thread nr%i:\n", thr);
	for (int j = start; j < end; j++){								//imprime
		printf("result pos %i: %i\n", j, value->result[j]);
	}
	
	(value->write)++;													//incrementa para o nr da thread que vai escrever a seguir
	
	pthread_mutex_unlock(&mutex);									//desbloqueia o mutex
	pthread_cond_broadcast(&cond);									//desbloqueia condicao
	
	pthread_exit((void*)NULL);
	
}


int main(){
	
	data estr;															//struct para enviar os dados para a thread
	estr.thr_nr = 0;													//thread inical
	estr.write = 0;														//primeira thread a ler
	
	/*
	 * preenche array de nr
	 */
	for(int i = 0; i<SIZE; i++){
		estr.numbers[i] =i;												//preenche o array da struct
	}
	
	/**
	 * Inicia mutex e cond
	 **/
	pthread_mutex_init(&mutex, NULL);									//inicia o mutex
	pthread_cond_init(&cond, NULL);										//inicia a cond
	
	/**
	 * inicia threads
	 **/
	pthread_t threads[TH];
	for(int i = 0; i <TH; i++){
		pthread_create(&threads[i], NULL, thread_func, (void*)&estr);	//cria 5 threads
	}
	
	
	/**
	 * espera que cada thread acabe
	 **/
	for(int i=0; i<TH; i++){
		pthread_join(threads[i], (void*) NULL);							//espera pelas cinco threads
	}
	
	/**
	 * Remove mutex e cond
	 **/ 
	pthread_mutex_destroy(&mutex);										//destroi mutex
	pthread_cond_destroy(&cond);										//destoi condiçao
	
	return 0;
}







