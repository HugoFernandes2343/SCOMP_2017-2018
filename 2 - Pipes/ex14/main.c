#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

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
		dup2(fd[0], 0);		/*input do pipe*/
		close(fd[0]);
		close(fd[1]);
		
		execlp("more", "more", (char*) NULL);
		perror("Exec failed");
		exit(1);
	}
	
	close(fd[0]);
	for(int i = 1; i<=100; i++){
		char temp [100] = "\n Line ";
		char line [100];
		sprintf(line, "%s%d", temp, i);
		if(write(fd[1], line, sizeof(line)) < 0){
				perror("Error writing");	
		}
	}
	
	close(fd[1]);
	wait(0);
	return 0;
}
