#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include "search.h"

void fail_fork(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
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

int main(void){
	
	shared *data;// pointer para a estrutura
	
	//variaveis de shm for's e forking
	int size = sizeof(shared); 
	pid_t p[10];
	int i;	
	
	//Criar Memoria partilhada
	int fd = shm_open("/shmex09", O_CREAT|O_EXCL|O_RDWR, S_IWUSR|S_IRUSR);
	
	if(fd < 0) {
		perror("Open falhou\n");
	}
	
	ftruncate(fd, size);

	data = (shared*)mmap(NULL, size*10, PROT_WRITE, MAP_SHARED, fd, 0);
	
	
	//Assign da palavra a cada uma das 10 structs	
	strcpy(data->word, "funciona");
	strcpy((data+1)->word, "mais");
	strcpy((data+2)->word, "nove");
	strcpy((data+3)->word, "semana");
	strcpy((data+4)->word, "texto");
	strcpy((data+5)->word, "esta");
	strcpy((data+6)->word, "vazio");
	strcpy((data+7)->word, "world");
	strcpy((data+8)->word, "informatica");
	strcpy((data+9)->word, "free");
	
	//Assing do path para o ficheiro a cada uma das 10 structs
	strcpy(data->path, "./inputs/file1.txt");
	strcpy((data+1)->path, "./inputs/file2.txt");
	strcpy((data+2)->path, "./inputs/file3.txt");
	strcpy((data+3)->path, "./inputs/file4.txt");
	strcpy((data+4)->path, "./inputs/file5.txt");
	strcpy((data+5)->path, "./inputs/file6.txt");
	strcpy((data+6)->path, "./inputs/file7.txt");
	strcpy((data+7)->path, "./inputs/file8.txt");
	strcpy((data+8)->path, "./inputs/file9.txt");
	strcpy((data+9)->path, "./inputs/file10.txt");
	
	//For de execução do programa
	for(i=0; i<10; i++) {
			
		//forks
		p[i] = fork();
		fail_fork(p[i]);
		
		
		if(p[i] == 0) { //Processos filho
			
			search( (data+i)->word, (data+i)->path, (data+i), i+1); //funcao de procura em que lhe passamos a palavra o path pointer para a estrutura e o numero do processo
		
			exit(0);
		}
		
	}
	
	//esperar que filhos acabem
	for(i=0; i<10; i++) {
		wait(NULL);
	}
	
	//Print de resultados
	for(i=0; i<10; i++) { 
		printf("Total para filho %d: %d\n", i+1, (data+i)->appearances);
	}

	
	//Eliminar zona de memória partilhada	
	fail_unmap(munmap(data,size)); //unmapping of the memory
	fail_close(close(fd)); // closing of the memory
	fail_unlink(shm_unlink("/shmex09")); // unlinking of the memory

	return 0;
}
