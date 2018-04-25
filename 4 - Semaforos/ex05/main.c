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
	sem_t *sem_pai;
	
	/**
	 * Inicia semaforos
	 **/
	sem_pai = sem_open("sem_pai", O_CREAT|O_EXCL, 0644, 0);
	fail_sem(sem_pai);

	
	/**
	 * faz o fork
	 **/
	p = fork();
	fail_fork(p);
	
	/**
	 * em caso de ser processo filho
	 **/
	if(p == 0 ){
		
		printf("Eu sou o Filho!\n");	//Imprime mensagem
		
		sem_post(sem_pai);				//faz post do semaforo de forma ao processo pai poder escrever a sua mensagem
		
		exit(0);
	}
	
	sem_wait(sem_pai);					//verifica se pode escrever fazendo wait
	
	printf("Eu sou o Pai!\n");
	
	fail_sem_unlink(sem_unlink("sem_pai"));
	
	wait(0);
	
	return 0;
}









