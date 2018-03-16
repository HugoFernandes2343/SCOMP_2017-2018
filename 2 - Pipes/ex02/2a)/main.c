#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
	int number;
	char string[1000];

	
	int fd[2];
	fail_pipe(pipe(fd));
	
	pid_t p = fork();
	fail_fork(p);
	
	if(p == 0){
		close(fd[1]);

		if( read(fd[0],&number,sizeof(number)) < 0 ){
			 perror("Error reading number");			 
		}
		printf("child number: %i\n", number);
		
		if( read(fd[0], &string, sizeof(string)) < 0){
			perror("Error reading string");
		}
		printf("child string: %s\n", string);
	
		close(fd[0]);
		exit(0);
	} 
	
	printf("Escreva um nr:\n");
	scanf("%i%*c", &number);
	printf("Escreva uma string:\n");
	fgets(string, 10000, stdin);
	
	close(fd[0]);
	if(write( fd[1], &number, sizeof(number))<0){
		perror("Error writing number");
	}
	if(write( fd[1], &string, sizeof(string))<0){
		perror("Error writing number");
	}

	wait(0);
	
}


