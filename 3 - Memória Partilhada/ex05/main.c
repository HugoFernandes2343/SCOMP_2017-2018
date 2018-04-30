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

#define CHILDS 10
#define NUMBS 10000

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

void fail_fork(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
	}
}


int main(){
	
	/*Iniciar variaveis*/
	int fd, data_size = sizeof(array);			
	array *localMax;
	pid_t fo;
	time_t t;
	srand ((unsigned) time (&t));
	
	/*Preenche array com numeros random*/
	int numbers[NUMBS];
	for(int i= 0; i<NUMBS; i++){
		numbers[i] = rand()%1000;
	}
	
	/*Criar memoria*/
	fd = shm_open("/shmem", O_CREAT|O_EXCL|O_RDWR, S_IWUSR|S_IRUSR);
	
	/*Definir tamanho da memoria*/
	ftruncate (fd, data_size);
	
	/*mapear objeto de memoria partilhada*/
	localMax = (array*) mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	
	/*ciclo for que cria cada processo filho*/
	for(int i = 0; i < CHILDS; i++){
		fo = fork();
		if(fo == 0){
			int max = -1;
			/*ciclo for que para cada filho define qual o intervalo de numeros que ele vai percorrer*/
			for (int j = i*NUMBS/CHILDS; j < (i*NUMBS/CHILDS)+NUMBS/CHILDS; j++){
				if(numbers[j] > max){
					max = numbers[j];
				}
			}
			
			/*escrever na memoria partilhada*/
			localMax->arr[i] = max;
			exit(0);
		}
	}
	
	/*Espera por todos os filhos*/
	for(int i = 0; i<10; i++){
		wait(0);
	}
	
	/*calcula o max global, imprime resultado*/
	int max = -1;
	for(int i = 0; i<10; i++){
		printf("Local Max nr %i: %i\n", i, localMax->arr[i]);
		if(localMax->arr[i] > max){
			max = localMax->arr[i];
		}
	}
	printf("Global max: %i\n", max);
	
+	/*Desliga o objeto de memoria partilhada, fecha o descritor e remove a memoria partilhada do sistema de ficheiros*/
	fail_unmap(munmap(localMax, data_size));
	fail_close(close(fd));
	fail_unlink(shm_unlink("/shmem"));
	
	return 0;
	
}












