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
#define VALOR_A_PROCURAR 205
pthread_mutex_t mutex;												//mutex para controlar acesso ao index da estrutura



typedef struct {													//struct para enviar dados para a thread_func
	int numbers[1000];
	int val;
	int thr_nr;
} data;																

typedef struct {													//	struct para retornar dados da thread_func
	int res;
} ret;												


void* thread_func(void *arg){
	data *value = ((data*) arg);									//converter o argumento para um apontador da struct
	
	pthread_mutex_lock(&mutex);										//faz lock ao mutex
	int thr = value->thr_nr;										//guarda o nr da thread 
	value->thr_nr++;												//incrementa o nr da thread 
	pthread_mutex_unlock(&mutex);									//desbloqueia o mutex
	
	int start = thr*(SIZE/TH);										//calcula o inicio da pesquisa do array
	int end = start+(SIZE/TH);										//calcula o fim
	
	for(int i = start; i<end; i++){									
		if(value->numbers[i] == value->val){						//caso encontre o nr
			ret *rtn = (ret*) malloc(sizeof(ret));					//prepara variavel para retorno
			printf("Valor %i encontrado no index %i\n",value->val, i);	//imprime descoberta
			rtn->res = thr+1;										//retorna o nr da thread currente
			pthread_exit((void*) rtn);								//retorna
		}
	}
	pthread_exit((void*)NULL);
	
}


int main(){
	
	data estr;															//struct para enviar os dados para a thread
	estr.val = VALOR_A_PROCURAR;										//valor a procurar
	estr.thr_nr = 0;													//thread inical
	
	/*
	 * preenche array de nr
	 */
	for(int i = 0; i<SIZE; i++){
		estr.numbers[i] =i;												//preenche o array da struct
	}
	
	/**
	 * Inicia mutex
	 **/
	pthread_mutex_init(&mutex, NULL);									//inicia o mutex
	
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
	ret exit_val;
	void *rtn;
	for(int i=0; i<TH; i++){
		pthread_join(threads[i], (void*) &rtn);							//espera pelas cinco threads
		if(rtn != (void*) NULL){										//verifica variavel de retorno
			exit_val = *((ret*) rtn);									//converte de apontador void para estrutura
			free(rtn);													//liberta memoria alocada
			printf("Thread que encontrou foi a nrº%i com o id %lu\n", exit_val.res, threads[i]);
		}
	}
	
	/**
	 * Remove mutex
	 **/ 
	pthread_mutex_destroy(&mutex);										//destroi mutex
	
	return 0;
}







