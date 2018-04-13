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
	int fd, data_size = sizeof(student);
	student *std;
	
	/*Criar memoria*/
	fd = shm_open("/shmem", O_EXCL|O_RDWR,S_IRUSR|S_IWUSR);
	/*Definir tamanho da memoria*/
	ftruncate (fd, data_size);
	/*mapear objeto de memoria partilhada*/
	std = (student*) mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	
	printf("Reader: A ler da memoria partilhada....\n");
	printf("%d\n", std->number);
	printf("%s\n", std->name);
	
	/*Desliga o objeto de memoria partilhada, fecha o descritor e remove a memoria partilhada do sistema de ficheiros*/
	fail_unmap(munmap(std,data_size));
	fail_close(close(fd));
	fail_unlink(shm_unlink("/shmem"));

	return 0;
}

