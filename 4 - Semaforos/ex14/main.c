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

#define R 2
#define W 2

typedef struct {
	int pid;
	char string[100];
	int nr_readers;
	int nr_writers;
} stru;

/**
 * Verifica se a criação do semaforo falhou
 **/
void fail_sem(sem_t *s){
	if(s == SEM_FAILED) {
		perror("Criação do semaforo falhou\n");
		exit(EXIT_FAILURE);
	}
}

void fail_unmap(int r){
	if(r == -1) {
			perror("Unmap falhou");
	}
}

void fail_close(int r){
	if(r == -1) {
			perror("shm close falhou");
	}
}

void fail_unlink(int r){
	if(r == -1) {
			perror("Shm unlink falhou");
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
	
	stru *data;
	int fd, data_size = sizeof(data);
	
	/**
	 * cria memorias partilhadas
	 **/
	/*Criar memoria*/
	fd = shm_open("/shmem", O_CREAT|O_EXCL|O_RDWR, S_IWUSR|S_IRUSR);
	/*Definir tamanho da memoria*/
	ftruncate (fd, data_size);
	/* mapear objeto de memoria partilhada */
	data = (stru*) mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	
	/**
	 * cria semaforos
	 **/
	sem_t *read, *write, *access_read, *access_write, *mutex;
	read = sem_open("read", O_CREAT|O_EXCL, 0644, 1);		
	fail_sem(read);	
	write = sem_open("write", O_CREAT|O_EXCL, 0644, 1);
	fail_sem(write);	
	access_read = sem_open("access_read", O_CREAT|O_EXCL, 0644, 1);		//mutex1
	fail_sem(access_read);
	access_write = sem_open("access_write", O_CREAT|O_EXCL, 0644, 1);	//mutex2
	fail_sem(access_write);
	mutex = sem_open("mutex", O_CREAT|O_EXCL, 0644, 1);					//mutex3
	fail_sem(mutex);
	
	/**
	 * inicia variaveis
	 **/
	data->nr_writers = 0;
	data->nr_readers = 0;
	
	pid_t pr, pw;
	
	/**
	 * fork para executar os writers
	 **/
	for(int j = 0; j < W; j++){
		pw = fork();
		if(pw==0){
			
			execlp("./writer", "writer", (char*) NULL);
			printf("teste\n");
			exit(0);
		}
	}
	
	
	/**
	 * fork para executar os readers
	 **/
	for(int i = 0; i<R; i++){
		pr = fork();
		
		if(pr == 0){
			
			sleep(1);
			execlp("./reader", "reader", (char*) NULL);
			printf("teste\n");
			exit(0);

		}
	}
	
	/**
	 * espera que os filhos terminem
	 **/
	for(int k = 0; k < R+W; k++){
		wait(0);
	}
	
	
	/**
	 * Desliga semaforos
	 **/
	fail_sem_unlink(sem_unlink("read"));
	fail_sem_unlink(sem_unlink("write"));
	fail_sem_unlink(sem_unlink("access_read"));
	fail_sem_unlink(sem_unlink("access_write"));
	fail_sem_unlink(sem_unlink("mutex"));
	
	/**
	 * Desliga o objeto de memoria partilhada, fecha o descritor e remove a memoria partilhada do sistema de ficheiros
	 **/
	fail_unmap(munmap(data, data_size));
	fail_close(close(fd));
	fail_unlink(shm_unlink("/shmem"));
	
	
	return 0;
	
}
