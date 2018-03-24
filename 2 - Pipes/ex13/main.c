#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

#define SIZE 100

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
	char buffer[SIZE];
	pid_t p;
	int p0[2],n;
	
	fail_pipe(pipe(p0));
	
	p=fork();
	fail_fork(p);
	
	if(p==0){
		close(p0[0]);
		dup2(p0[1],1);
		close(p0[1]);
		
		execlp("sort", "sort", "fx.txt", (char*)NULL);
		perror("Exec failed");
		exit(1);
	
	}else{
		
		printf("Processo pai \n");
		
		close(p0[1]);
		
		while((n=read(p0[0],buffer,SIZE))!=0){
			buffer[n-1] = 0;
			printf("Resultado do comando: \n %s\n",buffer);
		}
		
		close(p0[0]);
		wait(NULL);
	}	
	return 0;
}
