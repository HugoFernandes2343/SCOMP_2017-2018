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

//documento que mostra o raciocinio: algoritmo.txt

void fail_sem(sem_t *sem){
	if(sem==SEM_FAILED){
			perror("sem_open falhou");
		exit(1);
	}
}

void fail_Sunlink(int i){
	if(i==-1){
			perror("sem unlink falhou");
			exit(1);
		}
}

void fail_fork(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
	}
}

int main(){
	
	sem_t *sem_12; //semaforo que o processo 1 altera para que o processo 2 possa imprimir
	sem_t *sem_23; //semaforo que o processo 2 altera para que o processo 3 possa imprimir
	sem_t *sem_31; //semaforo que o processo 3 altera para que o processo 1 possa imprimir 
	
	//open do semaforos
	sem_12 = sem_open("semaforo12", O_CREAT, 0644, 0);
	fail_sem(sem_12);

	sem_23 = sem_open("semaforo23", O_CREAT, 0644, 0);
	fail_sem(sem_23);
	
	sem_31 = sem_open("semaforo31", O_CREAT, 0644, 0);
	fail_sem(sem_31);
	
	//forks
	pid_t p1,p2,p3;
	
	//fork 1
	p1 = fork();
	fail_fork(p1);
		
	if(p1==0){//processo 1
		
		//primeiro print
		printf("Sistemas ");
		fflush(stdout);
		
		//passa para o segundo e espera que o terceiro escreva
		sem_post(sem_12);
		sem_wait(sem_31);
		
		//segundo print
		printf("a ");
		fflush(stdout);
		
		//passa para o segundo e acaba o processo numero 1
		sem_post(sem_12);
		
		exit(EXIT_SUCCESS);
	}
	
	//fork 2
	p2 = fork();
	fail_fork(p2);
		
	if(p2==0){//processo 2
		
		//espera que o processo 1 de permissao de inicio
		sem_wait(sem_12);
		
		//primeiro print do proceeso 2
		printf("de ");
		fflush(stdout);
		
		//passa para o processo 3 a permissao e espera pela permissao vinda do primeiro novamente
		sem_post(sem_23);
		sem_wait(sem_12);
		
		//segundo print do processo 2
		printf("melhor ");
		fflush(stdout);
		
		//passa para o processo 3 e termina o processo 2
		sem_post(sem_23);
		
		exit(EXIT_SUCCESS);
	}
	
	//fork 3
	p3 = fork();
	fail_fork(p3);
		
	if(p3==0){//processo 3
		
		//inicia se num estado de espera pela permicao vinda do processo 2
		sem_wait(sem_23);
		
		//primeiro print do processo 3
		printf("Computadores - ");//print da primeira frase do terceiro filho
		fflush(stdout);
		
		//passa permissao ao processo 1 e espera pela permissao proveniente do processo 2
		sem_post(sem_31);
		sem_wait(sem_23);
		
		//segundo print do processo 3
		printf("disciplina. ");
		fflush(stdout);
		
		//termina o processo
		exit(EXIT_SUCCESS);
	}
	
	fail_Sunlink(sem_unlink("semaforo12"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo23"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo31"));//Unlinking of the semaphore
	
	return 0;
	
}
