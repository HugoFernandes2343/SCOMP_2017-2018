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
#include <sys/mman.h>
#include "struct.h"


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

int main(){
	
	/*Iniciar variaveis*/
	int fd, data_size = sizeof(array);			
	array *numbers;				
	
	
	/*Criar memoria*/
	fd = shm_open("/shmem", O_CREAT|O_EXCL|O_RDWR, S_IWUSR|S_IRUSR);
	/*Definir tamanho da memoria*/
	ftruncate (fd, data_size);
	/*mapear objeto de memoria partilhada*/
	numbers = (array*) mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	
	printf("Writer: A escrever na memoria partilhada...\n");
	
	/*Preenche array da memoria partilhada com numeros random*/
	time_t t;
	srand ((unsigned) time (&t));
	
	for(int i = 0; i < 10; i++){
		numbers->arr[i] = (rand()%20) + 1;
	}
	
	/*Executa programa que vai ler da memoria partilhada*/
	execlp("./reader","reader", (char *) NULL);
	
	/*caso o exec falhe:*/
	/*Desliga o objeto de memoria partilhada, fecha o descritor e remove a memoria partilhada do sistema de ficheiros*/
	fail_unmap(munmap(numbers, data_size));
	fail_close(close(fd));
	fail_unlink(shm_unlink("/shmem"));
	perror("exec error");
	
	exit(1);

}
