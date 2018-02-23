#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(void){
	printf("i'm...\n");
	pid_t p = fork();
	if(p == -1){
		perror(“Fork falhou”); 
		exit(1);
	}
	if(p==0){
		printf("I'll never join you!\n");
		exit(EXIT_SUCCESS);
	}
	printf("the ... \n");
	p = fork();
	if(p == -1){
		perror(“Fork falhou”); 
		exit(1);
	}
	if(p==0){
		printf("I'll never join you!\n");
		exit(EXIT_SUCCESS);
	}
	printf("father \n");
	p = fork();
	if(p == -1){
		perror(“Fork falhou”); 
		exit(1);
	}
	if(p==0){
		printf("I'll never join you!\n");
		exit(EXIT_SUCCESS);
	}
}