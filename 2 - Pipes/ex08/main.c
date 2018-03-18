#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "struct.h"

#define CHILDS 10

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
	
	pid_t p;
	int i;
	for(i=0; i<CHILDS; i++){
		p = fork();
		fail_fork(p);
		
		if(p == 0){
			s1 round;
			close(fd[1]);
			if(read(fd[0], &round, sizeof(round)) <0){
				perror("Error reading structure");	
			}
			close(fd[0]);
			printf("Round %d: %s\n", round.number, round.string);
			exit(round.number);
		}
	}
		
		
	s1 mess;
	strcpy(mess.string, "Win");;
	close(fd[0]);
	for(i = 0; i<CHILDS; i++){
		sleep(1);
		mess.number = i+1;
		if(write(fd[1], &mess, sizeof(mess)) < 0){
			perror("Error writing structure");	
		}
	}
	close(fd[1]);
	
	int status;
	pid_t child;
	for(i=0; i<CHILDS; i++){
		child = wait(&status);
		printf("Round nr: %i, Winner: %i\n", WEXITSTATUS(status), child);
	}
	
}


