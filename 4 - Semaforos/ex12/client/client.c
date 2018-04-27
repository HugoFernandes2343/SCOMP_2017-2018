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

#define MAX_TICKETS 100

int main(){
	
	// shared int with the other process that saves the ticket number
	int *number;
	
	//shared memory variables
	int fd, size = sizeof(int);
	
	//semaforos
	sem_t *sem_1;
	sem_t *sem_2;
	
	//number of the ticket
	int ticket_number;
	
	time_t t;	/* needed to initialize random number generator (RNG) */
	
	/* intializes RNG (srand():stdlib.h; time(): time.h) */
	srand ((unsigned) time (&t));
	
	
	//memory sharing
	fd = shm_open("/shm", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	ftruncate(fd, size);
	number = (int*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	
	//open dos semaforos created by the server
	sem_1 = sem_open("semaforo1", O_CREAT);
	fail_sem(sem_1);
	
	sem_2 = sem_open("semaforo2", O_CREAT);
	fail_sem(sem_2);
	
	pid_t p;
	
	//forks para correr o "cliente" que é o codigo que esta dentro deste ciclo
	for (int i = 0; i < MAX_TICKETS; i++)	{
		p = fork();
		fail_fork(p);
		
		if(p==0){ //se for o filho executa o filho se for pai continua a criar filhos
			
			sem_wait(sem_1); //espera que o semaforo 1 seja iniciado pelo server /incrementado pelo cliente 
		
			sleep(1 /*+ rand() % 10*/); //sleep para esperar de 1 a 10 segundos
		
			ticket_number = *number; // vai buscar á memoria o numero do bilhete
		
			printf("I got the number %d.\n", ticket_number); //imprime o numero do bilhete
		
			sem_post(sem_2); // incrementa o semaforo 2 para que o server possa passar para o proximo bilhete
			
			if(i < MAX_TICKETS ) { // enquanto nao for o ultimo incrementa o semaforo 1
				sem_post(sem_1);
			}
		
			exit(EXIT_SUCCESS); //exit
			
			
		}
	}
	
	wait(NULL); //espera que todos os processos acabem
	
	return 0;
	
}
