#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include <limits.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>

#define SIZE 102
#define TH_FILT 3
#define TH_COMP 3

typedef struct {	
	int id_h;														//hypermarket identifier
	int id_p;														//product identifier
	int p;															//product price
} element;

typedef struct {											//struct para enviar dados para a thread_func
	element vec[SIZE];
	int thr_filt_nr;										//id das threads queestao a filtrar
	element *vec1;
	element *vec2;
	element *vec3;
	int size_vec[3];
	int thr_comp_nr;										//id das threads que estao a calcular
	int filt;												//contador que sera incrementado quando uma thread terminar a sua filtracao
} data;	

typedef struct {
	int hyper;
	float avg;
	int wr;
} ret;

pthread_mutex_t mutex;												//mutex 
pthread_cond_t cond_threads;										//variavel de condiçao para sincronizar a escrita das threads
ret calcs;															//variavel global a ser preenchida apos calculos terem sido feitos pelas threads


void* thread_func_filt(void *arg){
	data *value = ((data*) arg);									//converter o argumento para um apontador da struct
	
	pthread_mutex_lock(&mutex);										//faz lock ao mutex
	int thr = value->thr_filt_nr;									//guarda o nr da thread 
	value->thr_filt_nr++;											//incrementa o nr da thread 
	pthread_mutex_unlock(&mutex);									//desbloqueia o mutex
	
	/**
	 * Inicia variaveis da thread
	 **/
	int count=0;
	element *vec = NULL;
	element *temp = NULL;
	
	/**
	 * calcula inicio e fim da pesquisa
	 **/
	int start = thr*(SIZE/TH_FILT);										//calcula o inicio da pesquisa do array para cada thread de acordo com o seu nr
	int end = start+(SIZE/TH_FILT);										//calcula o fim
	
	
	for(int i = start; i<end; i++){
		if(value->vec[i].id_h == thr){									//quando encontra um elemento com um identificador de hypermarket correto
			count++;													//aumenta contador para fazer realloc
			temp = (element*) realloc (vec, count * sizeof(element));	//faz realloc paraa allocar mais memoria para o apontador
			
			if(temp!=NULL){												//se der erro ao alocar faz exit(1)
				vec = temp;												
				vec[count-1] = value->vec[i];
			} else {
				free(vec);
				printf("Erro a alocar memoria\n");
				exit(1);
			}
		}
	}
	
	/**
	 * poe o apontador da estrutura a apontar para o array preenchido
	 **/
	if(thr == 0){
		value->vec1 = vec;
	} else if(thr == 1){
		value->vec2 = vec;
	} else {
		value->vec3 = vec;
	}
	
	pthread_mutex_lock(&mutex);										//faz lock ao mutex
	value->size_vec[thr] = count;
	value->filt++;
	pthread_mutex_unlock(&mutex);									//desbloqueia o mutex
	pthread_cond_broadcast(&cond_threads);							//desbloqueia condicao
	pthread_exit((void*)NULL);
}


void* thread_func_comp(void *arg){
	data *value = ((data*) arg);									//converter o argumento para um apontador da struct

	pthread_mutex_lock(&mutex);										//faz lock ao mutex
	int thr = value->thr_comp_nr;									//guarda o nr da thread 
	value->thr_comp_nr++;											//incrementa o nr da thread 
	while(value->filt != 3){										//espera que o nr de threads que acabaram de filtrar seja 3
		pthread_cond_wait(&cond_threads, &mutex);
	}
	pthread_mutex_unlock(&mutex);									//desbloqueia o mutex
	pthread_cond_broadcast(&cond_threads);							//desbloqueia condicao
	
	/**
	 * Inicia variaveis da thread
	 **/
	element *vec = NULL;
	int prices[5];
	int count[5];
	float average = 0;
	
	/**
	 * prepara vetor de leitura para cada thread
	 **/
	if(thr == 0){													//como cada thread le de um vetor isto identifica qual o vetor a ler conforme o seu nr de thread
		vec = value->vec1;
	} else if(thr == 1){
		vec = value->vec2;
	} else {
		vec = value->vec3;
	}
	
	/**
	 * Calcula
	 **/
	for(int i =0; i < (value->size_vec[thr]); i++){				
		prices[vec[i].id_p] += vec[i].p;								//soma dos precos de cada tipo
		count[vec[i].id_p]++;											//quantidade de cada tipo de produto
	}
	
	for(int i = 0; i<5; i++){											//calcula a media
		if(count[i] != 0){
			average += (prices[i]/count[i]);
		} else {
			average +=0;
		}
	}
	
	/**
	 * Escreve a variavel global
	 **/
	pthread_mutex_lock(&mutex);										//faz lock ao mutex
	if(calcs.avg>average){											//escreve na variavel global se a media for menor
		calcs.avg = average;
		calcs.hyper=thr;
	}
	calcs.wr++;
	pthread_mutex_unlock(&mutex);									//desbloqueia o mutex
	
	/**
	 * faz exit da thread
	 **/
	pthread_exit((void*)NULL);
}



int main(){
	
	/**
	 * Inicia variaveis
	 **/
	data estr;															//struct para enviar os dados para as threads
	estr.thr_filt_nr = 0;
	estr.thr_comp_nr = 0;
	estr.filt = 0;
	estr.vec1 = NULL;
	estr.vec2 = NULL;
	estr.vec3 = NULL;
	calcs.avg = INT_MAX;
	calcs.wr =0;
	
	/*
	 * preenche array de nr
	 */
	for(int i = 0; i<SIZE; i++){
		estr.vec[i].id_h = (i%3);												//preenche o array
		estr.vec[i].id_p = (i%5)+1;
		estr.vec[i].p = i+1;
	}
	
	/**
	 * Inicia mutex e cond
	 **/
	pthread_mutex_init(&mutex, NULL);									//inicia o mutex
	pthread_cond_init(&cond_threads, NULL);										//inicia a cond
	
	/**
	 * inicia threads de filtragem
	 **/
	pthread_t threads_filt[TH_FILT];
	for(int i = 0; i <TH_FILT; i++){
		pthread_create(&threads_filt[i], NULL, thread_func_filt, (void*)&estr);	//cria threads de filtragem
	}
	
	/**
	 * inicia threads de computação
	 **/
	pthread_t threads_comp[TH_COMP];
	for(int i = 0; i <TH_COMP; i++){
		pthread_create(&threads_comp[i], NULL, thread_func_comp, (void*)&estr);	//cria threads de computação
	}
	

	/**
	 * espera que cada thread de filtracao acabe
	 **/
	for(int i=0; i<TH_FILT; i++){
		pthread_join(threads_filt[i], (void*) NULL);							//espera pelas threads
	}
	/**
	 * espera que cada thread de computacao acabe
	 **/
	for(int i=0; i<TH_COMP; i++){
		pthread_join(threads_comp[i], (void*) NULL);							//espera pelas threads
	}
	
	/**
	 * Acede a variavel global para escrever resultado
	 **/
	printf("Hyper id: %i \nPrice average: %f\n", calcs.hyper+1, calcs.avg);
	
	
	/**
	 * Liberta memoria
	 **/
	 free(estr.vec1);
	 free(estr.vec2);
	 free(estr.vec3);
	 
	/**
	 * Remove mutex e cond
	 **/ 
	pthread_mutex_destroy(&mutex);											//destroi mutex
	pthread_cond_destroy(&cond_threads);										//destoi condiçao
	
	return 0;
}







