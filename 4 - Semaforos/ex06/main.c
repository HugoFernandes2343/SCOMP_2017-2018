#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/mman.h>

#define MESS 15

/**
 * Verifica se a criação do semaforo falhou
 **/
void fail_sem(sem_t *s){
	if(s == SEM_FAILED) {
		perror("Criação do semaforo falhou\n");
		exit(EXIT_FAILURE);
	}
}

/**
 * Verifica se o fork falhou
 **/
void fail_fork(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
	}
}

/**
 * Verifica se a remoçao do semaforo falhou
 **/
void fail_sem_unlink(int s){
	if(s == -1) {
		perror("Remoção do semaforo falhou\n");
		exit(EXIT_FAILURE);
	}
}


int main(){
	
	/**
	 * Declaração variaveis
	 **/
	pid_t p;
	sem_t *sem_pai, *sem_filho;
	
	/**
	 * Inicia semaforos
	 **/
	sem_pai = sem_open("sem_pai", O_CREAT|O_EXCL, 0644, 0);
	fail_sem(sem_pai);
	sem_filho = sem_open("sem_filho", O_CREAT|O_EXCL, 0644, 1);
	fail_sem(sem_filho);
	
	/**
	 * faz o fork
	 **/
	p = fork();
	fail_fork(p);
	
	if(p == 0 ){
		
		for(int i=0; i<MESS; i++){
			sem_wait(sem_filho);// a primeira vez corre logo nas proximas execuções espera que o pai dê post ao sem_filho
			
			printf("Message nr: %d, \"Eu sou o Filho!\"\n", i+1);
			
			sem_post(sem_pai); // faz post ao semaforo pai para que o processo pai possa executar uma iterecação do seu ciclo
		}
		
		exit(0);
		
	}
	
	for(int i = 0; i<MESS; i++){
		sem_wait(sem_pai);// verifica se pode ser inicializada uma execucao do ciclo
		
		printf("Message nr: %d, \"Eu sou o Pai!\"\n", i+1);
		
		sem_post(sem_filho);// faz post ao semaforo do filho para que este possa fazer um iteração do seu ciclo
	}
	
	//unlinks
	fail_sem_unlink(sem_unlink("sem_pai"));
	fail_sem_unlink(sem_unlink("sem_filho"));
	
	wait(0);
	
	return 0;
}









