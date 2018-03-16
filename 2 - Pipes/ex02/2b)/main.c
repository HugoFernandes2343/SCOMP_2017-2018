#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "struct.h"

void fail_fork(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
	}
}

void fail_pipe(int p){
	if(p == -1) {
		perror("Pipe falhou\n");
		exit(EXIT_FAILURE);
	}
}

int main(){
	

	
	int fd[2];
	fail_pipe(pipe(fd));
	
	pid_t p = fork();
	fail_fork(p);
	
	if(p == 0){
		close(fd[1]);
		s1 str_r;

		if( read(fd[0],&str_r,sizeof(str_r)) < 0 ){
			 perror("Error reading structure");			 
		}
		
		printf("child number: %i\n", str_r.number);
		printf("child string: %s\n", str_r.string);
	
		close(fd[0]);
		exit(0);
	} 
	
	s1 str_wr;
	printf("Escreva um nr:\n");
	scanf("%i%*c", &str_wr.number);
	printf("Escreva uma string:\n");
	fgets(str_wr.string, 10000, stdin);
	
	close(fd[0]);
	if(write( fd[1], &str_wr, sizeof(str_wr))<0){
		perror("Error writing structure");
	}

	
}


