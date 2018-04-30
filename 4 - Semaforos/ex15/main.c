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

void fail_unmap(int r){
	if(r == -1) {
			perror("Unmap falhado");
	}
}

void fail_close(int r){
	if(r == -1) {
			perror("Close falhado");
	}
}

void fail_unlink(int r){
	if(r == -1) {
			perror("Shm unlink falhou");
	}
}

#define PROC 23
int main(){
	
	sem_t *sem_visitors;
	sem_t *sem_visiting;
	sem_t *sem_room;
	
	int value;
	
	//open dos semaforos
	sem_visitors = sem_open("semaforo1", O_CREAT, 0644, 5);
	fail_sem(sem_visitors);
	
	sem_visiting = sem_open("semaforo2", O_CREAT, 0644, 0);
	fail_sem(sem_visiting);
	
	sem_room = sem_open("semaforo3", O_CREAT, 0644, 0);
	fail_sem(sem_room);
	
	
	pid_t p;
	int num_proc = PROC;
	//forks
	for (int i = 0; i < num_proc; i++){
		p = fork();
		fail_fork(p);
		if(p==0){
			break;
		}
	}
	
	if(p==0){//filho / visitor
		while(1){
			
			//verifica ha lugares dos 5 iniciais
			sem_wait(sem_visitors);
			
			//verifica se a sala ta aberta quando estiver entra, e para isso durante uns momentos impede outro visitante de entrar
			sem_wait(sem_room);
			
			//faz post depois de entrar para que outros visitantes possam entrar
			sem_post(sem_room);
			
			printf("I'm in %d\n",getpid());
			
			//fica num estado de visitar
			sem_wait(sem_visiting);
			
			printf("I'm out %d\n",getpid());
			
			exit(EXIT_SUCCESS);
		}
		
	}else if(p>0){
	
		int j=0; //count
	
		//pai/room
		while( j <= ((num_proc/5)+1)){//como o maximo que pode tar na sala a um determinado ponto sao 5 pessoas dividindo por 5 e somando 1 faz com que sejam sempre correto o numero de vezes que abre e fecha a sala
		
			//abre a sala 
			printf("Open room\n"); 
			sem_post(sem_room);
			sleep(2);
			
			//fecha a sala
			printf("close room\n");
			sem_wait(sem_room);
			sleep(2);
			
			//usa o valor de visitantes para saber quantas vezes tem que fazer post a variavel que os permite sair e á variavel que volta a repor as vagas
			sem_getvalue(sem_visitors,&value);
			for (int i = value; i <5 ; i++){
				sem_post(sem_visiting);//para poderem sair
				sem_post(sem_visitors);//para corrigir as vagas
			}
		
			j++;//count
		}
	}
	
	//wait pelos filhos
	for (int i = 0; i < num_proc; i++){
		wait(NULL);
	}
	
	printf("End.\n");//termino do programa
	
	//unlink
	fail_Sunlink(sem_unlink("semaforo1"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo2"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo3"));//Unlinking of the semaphore
	
	return 0;
}
