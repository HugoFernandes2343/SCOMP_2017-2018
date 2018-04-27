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

typedef struct {
	char str[10]; //Tamanho do array
	int count;
} shared;

int main(){
	sem_t *sem_c;
	sem_t *sem_s;
	int i =0;
	shared *sh;
	int fd=0, size = sizeof(shared); // memory sharing variables
	
	//memory sharing
	fd = shm_open("/shm", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	ftruncate(fd, size);
	sh = (shared*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	//open dos semaforos
	sem_c = sem_open("semaforo_c", O_CREAT, 0644, 1);
	fail_sem(semaforo);
	
	sem_s = sem_open("semaforo_s", O_CREAT, 0644, 1);
	fail_sem(semaforo);
	
	pid_t p;
	p=fork();
	fail_fork(p);
	
	if(p==0){//while s
		while(sh->count < 10){
			sem_wait(sem_s);
			
			sem_post(sem_c);
		}
		
	}else{//while c
		while(sh->count < 10){
			sem_wait(sem_c);
			
			sem_post(sem_s);
		}
	

	}
}
