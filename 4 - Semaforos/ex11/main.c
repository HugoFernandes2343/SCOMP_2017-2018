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

//documento que mostra o raciocinio: algoritmo.txt

void fail_sem(sem_t *sem){
	if(sem==SEM_FAILED){
			perror("sem_open falhou");
		exit(1);
	}
}


void fail_fork(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
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

typedef struct {
	int numeroPessoas;
	int lugaresDisponiveis;
} shared;

int main(){
	
	// shared int with the other process that saves the number of people
	shared *data;
	
	//shared memory variables
	int fd, size = sizeof(shared);
	
	//semaforos
	sem_t *sem_entrada;
	sem_t *sem_portas;
	sem_t *sem_mutex;
	sem_t *sem_saida;
	
	//forks
	pid_t p;
	
	//memory sharing
	fd = shm_open("/shm", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	ftruncate(fd, size);
	data = (shared*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	data->numeroPessoas = 1;
	data->lugaresDisponiveis = 200;
	
	//open dos semaforos
	sem_entrada = sem_open("semaforo1", O_CREAT, 0644, 0);
	fail_sem(sem_entrada);
	
	sem_portas = sem_open("semaforo2", O_CREAT, 0644, 3);
	fail_sem(sem_portas);
	
	sem_mutex = sem_open("semaforo3", O_CREAT, 0644, 1);
	fail_sem(sem_mutex);
	
	sem_saida = sem_open("semaforo4", O_CREAT, 0644, 0);
	fail_sem(sem_saida);
	
	//forking
	do{
		p=fork();
		fail_fork(p);
		
		if(p==0){//clientes
			sem_wait(sem_mutex);
						
			//se a variavel pessoas chegar a 200 fazer up ao mutex e sair 
			if(data->numeroPessoas==300){
				sem_post(sem_mutex);
				exit(EXIT_SUCCESS);
			}

			//se nao ouver portas livres esperar ate existir portas livres
			sem_wait(sem_portas);
			
			data->numeroPessoas++;
			data->lugaresDisponiveis--;
			
			
			sem_post(sem_mutex);
			
			sem_post(sem_entrada);
			printf("Entrei sou o processo %d \n",getpid());
			fflush(stdout);
			///////////////////

			sem_wait(sem_saida);
			
			sem_wait(sem_portas);
			
			sem_wait(sem_mutex);
			data->lugaresDisponiveis++;
			printf("Saí sou o processo %d \n",getpid());
			fflush(stdout);
			sem_post(sem_mutex);
			
			sem_post(sem_portas);
			
			exit(EXIT_SUCCESS);
			
		}
			
	}while(data->lugaresDisponiveis>0);
	
	//entradas
	do{
		
	sem_wait(sem_entrada);
	
	sem_post(sem_portas);
	
	sleep(2);
	
	sem_post(sem_saida);
	
	}while(data->numeroPessoas <300);
	
	for (int i = 0; i < 301; i++)
	{
		wait(NULL);
	}
	
	//terminating memory and semaphores
	fail_unmap(munmap(data,size)); //unmapping of the memory
	fail_close(close(fd)); // closing of the memory
	fail_unlink(shm_unlink("/shm")); // unlinking of the memory
	fail_Sunlink(sem_unlink("semaforo1"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo2"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo3"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo4"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo5"));//Unlinking of the semaphore

	return 0;
}
