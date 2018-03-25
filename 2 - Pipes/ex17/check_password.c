#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

#define SIZE 50

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
	
	char name[SIZE],pass[SIZE];
	
	int p0[2];
	fail_pipe(pipe(p0));

	pid_t p = fork();
	
	fail_fork(p);
	
	if(p==0){
		close(p0[1]);
		dup2(p0[0], STDIN_FILENO);
		close(p0[0]);
		
		execlp("./validate","validate", (char *) NULL);
		
		perror("exec error");
		exit(1);
	
	}else{
		
		printf("Enter the ID: \n");
		scanf("%s",name);
		
		if(write(p0[1],name,strlen(name)+1)<0){
			perror("Error writing");
			exit(EXIT_FAILURE);
			
		}
		
		printf("Enter the password: \n");
		scanf("%s",pass);
		
		if(write(p0[1],pass,strlen(pass)+1)<0){
			perror("Error writing");
			exit(EXIT_FAILURE);
			
		}
		
		close(p0[1]);
		int status;
		wait(&status);
		
		if(WIFEXITED(status)){
			if(WEXITSTATUS(status)==2){
				printf("No such user\n");
			}
			if(WEXITSTATUS(status)==0){
				printf("Invalid password\n");
			}
			if(WEXITSTATUS(status)==1){
				printf("Password verified\n");
			}
		}
	}
	return 0;
}
