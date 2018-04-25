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



void buy_chips(){
		printf("Bought Chips!\n");
}

void buy_beer(){
		printf("Bought Beer!\n");
}

void eat_and_drink(){
	printf("Eat and Drink!\n");
}

int main(){
	
	/**
	 * Declaração variaveis
	 **/
	pid_t p;
	sem_t *sem_food, *sem_drinks;
	
	/**
	 * Inicia semaforos
	 **/
	sem_food = sem_open("sem_food", O_CREAT|O_EXCL, 0644, 0);
	fail_sem(sem_food);
	sem_drinks = sem_open("sem_drinks", O_CREAT|O_EXCL, 0644, 0);
	fail_sem(sem_drinks);
	
	/**
	 * faz o fork
	 **/
	p = fork();
	fail_fork(p);
	
	
	/**
	 * Em caso de ser o filho
	 **/
	if (p==0){
		
		buy_beer();				//Processo compra cerveja
		sem_post(sem_drinks);	//processo incrementa semaforo da bebida
		sem_wait(sem_food);		//Processo espera pela comida
		eat_and_drink();		//Processo come
		exit(0);
	}
	
	buy_chips();				//Processo compra comida
	sem_post(sem_food);			//Processo incrementa semaforo da comida
	sem_wait(sem_drinks);		//Processo espera pelas bebidas
	eat_and_drink();			//Processo come
	
	/**
	 * Desliga semaforos
	 **/
	fail_sem_unlink(sem_unlink("sem_food"));
	fail_sem_unlink(sem_unlink("sem_drinks"));
	wait(0);
	
	return 0;	
}
