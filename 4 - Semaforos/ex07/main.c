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
	
	sem_t *sem_12;
	sem_t *sem_23;
	sem_t *sem_31;
	
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
		
		printf("Sistemas ");
		fflush(stdout);
		sem_post(sem_12);
		
		sem_wait(sem_31);
		
		printf("a ");
		fflush(stdout);
		
		sem_post(sem_12);
		
		exit(EXIT_SUCCESS);
	}
	
	//fork 2
	p2 = fork();
	fail_fork(p2);
		
	if(p2==0){//processo 2
		sem_wait(sem_12);
		
		printf("de ");
		fflush(stdout);
		
		sem_post(sem_23);
		sem_wait(sem_12);
		
		printf("melhor ");
		fflush(stdout);
		
		sem_post(sem_23);
		
		exit(EXIT_SUCCESS);
	}
	
	//fork 3
	p3 = fork();
	fail_fork(p3);
		
	if(p3==0){//processo 3
		
		sem_wait(sem_23);
		printf("Computadores - ");//print da primeira frase do terceiro filho
		fflush(stdout);
		sem_post(sem_31);

		sem_wait(sem_23);
		printf("disciplina. ");
		fflush(stdout);
		
		exit(EXIT_SUCCESS);
	}
	
	fail_Sunlink(sem_unlink("semaforo12"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo23"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo31"));//Unlinking of the semaphore
	
	return 0;
	
}
