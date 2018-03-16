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
	
	int fd[2];
	fail_pipe(pipe(fd));
	
	pid_t p = fork();
	fail_fork(p);
	
	if(p == 0){
		close(fd[1]);
		int c;
		do {
			if( read(fd[0], &c, sizeof(c)) < 0 ){
				perror("Error reading char");			 
			}
			if(c != EOF){
				putchar(c);
			}
		} while (c != EOF);
		
		close(fd[0]);
		exit(0);
	}
	
	int c;
	FILE *file;
	file = fopen("test.txt", "r");
	
	close(fd[0]);
	if (file) {
		while ((c = getc(file)) != EOF){
			if(write( fd[1], &c, sizeof(c))<0){
				perror("Error writing char");
			}			
		} 	
		if(write( fd[1], &c, sizeof(c))<0){
			perror("Error sending file end");
		}
		
		fclose(file);
		close(fd[1]);
	}
	
	wait(0);
}


