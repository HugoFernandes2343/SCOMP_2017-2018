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

typedef struct line
{
	char str[50][80];
	int count;
} line;

int main(){
	
	sem_t *semaforo; //semaforo
	line* ln; //apontador para a estrutura
	int fd=0, size = sizeof(line); // memory sharing variable
	
	//memory sharing
	fd = shm_open("/shm", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	ftruncate(fd, size);
	ln = (line*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	//open do semaforo
	semaforo = sem_open("semaforo", O_CREAT, 0644, 1);
	fail_sem(semaforo);
	
	sem_wait(semaforo);
	
	for(int i = 0; i<ln->count;i++){// imprimir tudo o que estava na estrutura
		printf("%s \n",ln->str[i]);
		
	}
	
	sem_post(semaforo);
	
	fail_Sunlink(sem_unlink("semaforo"));//Unlinking of the semaphore

	fail_unmap(munmap(ln,size)); //unmapping of the memory

	fail_close(close(fd)); // closing of the memory

	fail_unlink(shm_unlink("/shm")); // unlinking of the memory
	
	return 0;
	}
