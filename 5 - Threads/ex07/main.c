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

#define SIZE 10000
#define TH 10
pthread_mutex_t mutex;												//mutex para controlar acesso ao index da estrutura

typedef struct {											//estrutura que contem uma chave do totoloto
	int picks[5];
} key;

typedef struct {											//struct para enviar dados para a thread_func
	key numbers[SIZE];										//array com as chaves a analisar
	int thr_nr;												//nr da thread
} database;	

typedef struct {											//struct para retornar dados das threads
	int probs[49];											//probabilidades, cada posicao vai ter a quantidade de vezes que esse numero aparece nas chaves
} ret;																								


void* thread_func(void *arg){
	database *value = ((database*) arg);									//converter o argumento para um apontador da struct
	
	pthread_mutex_lock(&mutex);										//faz lock ao mutex
	int thr = value->thr_nr;										//guarda o nr da thread 
	value->thr_nr++;												//incrementa o nr da thread 
	pthread_mutex_unlock(&mutex);									//desbloqueia o mutex

	int start = thr*(SIZE/TH);										//calcula o inicio da pesquisa do array para cada thread de acordo com o seu nr
	int end = start+(SIZE/TH);										//calcula o fim
	ret *result = (ret*) malloc(sizeof(ret));						//prepara variavel para retorno
	
	for(int i = 0; i<49;i++){										//inicia array de resultados a 0
			result->probs[i]=0;
	}
	
	for(int i = start; i<end; i++){									//faz os calculos
		for(int j = 0; j <5; j++){
			(result->probs[value->numbers[i].picks[j]])++;
		}
	}
	
	pthread_exit((void*)result);
}


int main(){
	
	database db;														//struct para enviar os dados para a thread
	int resultados[49];													//array que contera os resultados
	for(int i = 0; i<49;i++){
			resultados[i]=0;
	}
	db.thr_nr = 0;
	
	/**
	 * preenche array de nr
	 **/
	for(int i = 0; i<SIZE; i++){
		for(int j = 0; j<5; j++){
			db.numbers[i].picks[j] = j;												//preenche o array da struct
		}
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
		pthread_create(&threads[i], NULL, thread_func, (void*) &db);		//cria 10 threads
	}
	
	
	/**
	 * espera que cada thread acabe
	 **/

	for(int i=0; i<TH; i++){
		ret exit_val;
		void *rtn;
		if(pthread_join(threads[i], (void*) &rtn)){							//espera pelas threads
			printf("Error receiving data\n");
		}
		exit_val = *((ret*) rtn);										//converte de apontador void para estrutura
		free(rtn);														//liberta memoria alocada
		
		for(int j = 0; j<49; j++){										//junta todas as estatisticas de cada thread
			resultados[j]+=exit_val.probs[j];
		}
	}
	
	/**
	 * Imprime 	
	 **/
	printf("Frequencia de cada numero:\n");
	for(int i = 0; i<49; i++){
		printf("NR %i: %i vezes em 10000 chaves\n", i+1, resultados[i]);
	}
	
	/**
	 * Remove mutex
	 **/ 
	pthread_mutex_destroy(&mutex);										//destroi mutex
	
	return 0;
}







