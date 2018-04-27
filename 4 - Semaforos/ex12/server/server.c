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

#define MAX_TICKETS 100;

int main(){
	
	// shared int with the other process that saves the ticket number
	int *number;
	
	//shared memory variables
	int fd, size = sizeof(int);
	
	//semaforos
	sem_t *sem_1;
	sem_t *sem_2;
	
	//memory sharing
	fd = shm_open("/shm", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	ftruncate(fd, size);
	number = (int*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	//open dos semaforos
	sem_1 = sem_open("semaforo1", O_CREAT, 0644, 1);
	fail_sem(sem_1);
	
	sem_2 = sem_open("semaforo2", O_CREAT, 0644, 0);
	fail_sem(sem_2);
	
	//starts the ticket number at 1 
	*number = 1;
	
	for (int i = 0; i < 100; i++)//waits for a client and increments the number until there are 100 tickets
	{
		sem_wait(sem_2);//waits for the aperance of a client
		
		*number += 1;// increments the number of tickets
	}

	//terminating memory and semaphores
	fail_unmap(munmap(number,size)); //unmapping of the memory
	fail_close(close(fd)); // closing of the memory
	fail_unlink(shm_unlink("/shm")); // unlinking of the memory
	fail_Sunlink(sem_unlink("semaforo1"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo2"));//Unlinking of the semaphore
	
	return 0;
}
