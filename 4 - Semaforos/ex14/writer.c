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

int main(){
	
	/**
	 * abre semaforos
	 **/
	sem_t *read, *write, *access_write;
	read = sem_open("read", O_EXCL);									//semaforo que verifica se pode ler
	fail_sem(read);	
	write = sem_open("write", O_EXCL);									//semaforo que verifica se pode escrever
	fail_sem(write);	
	access_write = sem_open("access_write", O_EXCL);					//bloqueia acesso a variavel nr_writers (mutex2)
	fail_sem(access_write);
	
	/**
	 * abre memorias partilhadas
	 **/
	stru *data;
	int fd, data_size = sizeof(data);
	
	/*Criar memoria*/
	fd = shm_open("/shmem", O_RDWR, S_IWUSR|S_IRUSR);
	/*Definir tamanho da memoria*/
	ftruncate (fd, data_size);
	/* mapear objeto de memoria partilhada */
	data = (stru*) mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

	/**
	 * aumenta nr de writers, pondo todos os readers a espera
	 **/
	sem_wait(access_write);												//faz wait ao acesso a fila de escritores
	(data->nr_writers)++;												//marca lugar do writer na fila
	if(data->nr_writers == 1){											//se a fila for igual a 1
		sem_wait(read);													//poe os leitores em espera
	}
	sem_post(access_write);												//liberta o acesso a fila de escritores
	
	/**
	 * escreve na memoria
	 **/
	sem_wait(write);													//faz wait a escrita
	time_t t;															//inicia variavel de tempo						
    time(&t);
	strcpy(data->string, ctime(&t));									//copia tempo do sistema para a shmem
	data->pid = getpid();												//poe o pid do writer na shmem
	printf("writer nr: %i, r/w: %i/%i\n", data->pid, data->nr_readers, data->nr_writers);		//imprime nr de leitores/escritores
	sem_post(write);													//faz post a escripa
	
	/**
	 * diminui nr de writers, se fila ficar vazia liberta readers
	 **/
	 sem_wait(access_write);											//faz wait ao acesso a fila de escritores
	 (data->nr_writers)--;												//remove lugar do writer na fila
	 if(data->nr_writers == 0){											//se a fila for igual a 0
		sem_post(read);													//liberta leitores
	}
	sem_post(access_write);												//liberta o acesso a fila de escritores
	 
	
	return 0;
	
}
