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
	sem_t *read, *write, *access_read, *mutex;
	read = sem_open("read", O_EXCL);									//semaforo que verifica se pode ler
	fail_sem(read);	
	write = sem_open("write", O_EXCL);									//semaforo que verifica se pode escrever
	fail_sem(write);	
	access_read = sem_open("access_read", O_EXCL);						//bloqueia acesso a variavel nr_readers (mutex1)
	fail_sem(access_read);	
	mutex = sem_open("mutex", O_EXCL);									//impede que multiplos leitores acedam azona critica do leitor, de forma a nao bloquear o escritor (mutex3)
	fail_sem(mutex);
	
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
	 * Area critica do leitor, se mais do que um leitor entrar nesta parte o escritor fica bloqueado
	 **/
	sem_wait(mutex);													//bloqueia acesso a area critica do leitor			
	sem_wait(read);														//bloqueia acesso dos leitores
	sem_wait(access_read);												//bloqueia acesso a fila de leitores
	(data->nr_readers)++;												//incrementa a fila de readers
	if(data->nr_readers == 1){											//se a fila for igual a 1
		sem_wait(write);												//faz os escritores esperar
	}
	sem_post(access_read);												//desbloqueia acesso a fila
	sem_post(read);														//desbloqueia acesso dos leitores
	sem_post(mutex);													//desbloqueia acesso a area critica do leitor
	
	/**
	 * leitura
	 **/
	printf("reader pid: %i, string: %s, nr readers: %i \n", data->pid, data->string, data->nr_readers); //imprime
	
	/**
	 *  retirar leitor do contador
	 **/
	sem_wait(access_read);												//bloqueia acesso a fila de leitores
	(data->nr_readers)--;												//remove o leitor da fila
	if(data->nr_readers == 0){											//se a fila chegar a zero
		sem_post(write);												//liberta escritores
	}
	sem_post(access_read);												//liberta acesso a fila
	
	
	
	return 0;
}
