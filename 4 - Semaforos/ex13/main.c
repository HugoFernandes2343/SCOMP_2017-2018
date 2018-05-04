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
#include "struct.h"

#define BUF_SIZE 10


/**
 * Verifica se a criação do semaforo falhou
 **/
void fail_sem(sem_t *s){
	if(s == SEM_FAILED) {
		perror("Criação do semaforo falhou\n");
		exit(EXIT_FAILURE);
	}
}

/**
 * Verifica se ounmap da shmem falhou
 **/
void fail_unmap(int r){
	if(r == -1) {
			perror("Unmap falhou");
	}
}

/**
 * Verifica se o fecho da shmem falhou
 **/
void fail_close(int r){
	if(r == -1) {
			perror("shm close falhou");
	}
}

/**
 * Verifica se o unlink da shmem falhou
 **/
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
	
	sem_t *canWrite, *canRead, *index_access;
	int read_index = 0;
	
	stru *data;
	int fd, data_size = sizeof(data);	
	
	pid_t p;

	/**
	 * cria semaforos
	 **/
	canWrite = sem_open("canWrite", O_CREAT|O_EXCL, 0644, 10);				//semaforo para os filhos verificarem se podem escrever na memoria partilhada
	fail_sem(canWrite);														
	canRead = sem_open("canRead", O_CREAT|O_EXCL, 0644, 0);					//verificação para o processo pai de quando pode escrever
	fail_sem(canRead);
	index_access = sem_open("index_access", O_CREAT|O_EXCL, 0664, 1);		//deixa os processos filhos aceder ao index para saberem onde escrever
	fail_sem(index_access);
	
	/**
	 * cria memorias partilhadas
	 **/
	/*Criar memoria*/
	fd = shm_open("/shmem", O_CREAT|O_EXCL|O_RDWR, S_IWUSR|S_IRUSR);
	/*Definir tamanho da memoria*/
	ftruncate (fd, data_size);
	/*mapear objeto de memoria partilhada*/
	data = (stru*) mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	
	data->current = 0;
	data->write_index = 0;
	
	for(int i = 0; i <2; i++){
		
		p = fork();								//faz fork
		
		if(p == 0){
			
			for(int i = 0; i<15; i++){		
				
				
				sem_wait(canWrite);										//verifica se pode escrever na memoria partilhada
				sem_wait(index_access);									//verifica se pode aceder ao index
				if(data->write_index == BUF_SIZE){							//se o index for igual ao buffer
					data->write_index = 0;								//faz reset ao buffer
				}		
				data->numbers[data->write_index] = data->current;		//coloca o nr atual a no sitio correto da memoria
				data->current++;										//incrementa o numero atual
				data->write_index++;									//incrementa o index de escrita
				
				sem_post(index_access);						//faz post ao semaforo de forma a permitir a outro processo aceder ao index de escrita
				sem_post(canRead);							//faz post ao semaforo de forma ao leitor comecar a ler
			}
			
			exit(0);
		}
	
	}
		
	for(int i = 0; i < 30 ;i++){										//para os 30 nr
		
			if(read_index == BUF_SIZE){									//se o index de leitura for igual ao tamanho do buffer
				read_index = 0;											//faz reset ao index
				for(int j = 0; j <BUF_SIZE; j++){								//faz post ao semaforo de escrita para cada lugar livre do buffer
					sem_post(canWrite);
				}
			}
			
			sem_wait(canRead);											//verifica se pode ler
			printf("%iº nr: %i\n", i, data->numbers[read_index]);		//imprime
			read_index++;												//aumenta o index de leitura
	}
	
	/**
	 * espera pelos filhos
	 **/
	wait(0);
	wait(0);
		
	/**
	 * Desliga semaforos
	 **/
	fail_sem_unlink(sem_unlink("canWrite"));
	fail_sem_unlink(sem_unlink("canRead"));
	fail_sem_unlink(sem_unlink("index_access"));
		
	/**
	 * Desliga o objeto de memoria partilhada, fecha o descritor e remove a memoria partilhada do sistema de ficheiros
	 **/
	fail_unmap(munmap(data, data_size));
	fail_close(close(fd));
	fail_unlink(shm_unlink("/shmem"));
		
		
	return 0;

}
	
	
	
	
	
	
	
	
	
	
	
	

