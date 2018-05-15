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

typedef struct{
	int number;
	char name[100];
	char address[100];
	
} data;


void* thread_func(void *arg){
	data value = *((data*)arg);
	printf("Nr: %i, Name: %s, Address: %s\n", value.number, value.name, value.address);
	pthread_exit((void*)NULL);
}


int main(){
	
	pthread_t threads[5];
	data arr[5];
	
	/**
	 * preenche array de estruturas com informação
	 **/
	for(int i = 0; i<5; i++){
		char temp[100];
		sprintf(temp, "%s%i", "teste",i);
		arr[i].number=i;
		strcpy(arr[i].name, temp);
		strcpy(arr[i].address, temp);
	}
	
	/**
	 * inicia threads
	 **/
	for(int i = 0; i <5; i++){
		pthread_create(&threads[i], NULL, thread_func, (void*)&arr[i]);
	}
	
	/**
	 * espera que cada thread acabe
	 **/
	for(int i=0;i<5;i++){
		pthread_join(threads[i], NULL);
	}
	
	return 0;
}











