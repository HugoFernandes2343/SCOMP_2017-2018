#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

#define READ_END 0

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
		
	int numberIn=1, numberOut=0;
	
	/*PIPES*/
	int p0[2],p1[2];
	pipe(p0);
	pipe(p1);
	
	/*FORK*/
	pid_t p;
	p = fork();
	fail_fork(p);
	
	if(p==0){
		close(p0[1]);
		dup2(p0[0],0); 
		close(p0[0]);
		
		close(p1[0]);
		dup2(p1[1],1);
		close(p1[1]);
		
		execlp("./fact.exe","./fact",(char*) NULL);
		perror("exec failed");
		exit(1);
		
	
	}else{
		close(p0[0]);
		close(p1[1]);
		
		printf("Enter the number:\n");
		scanf("%d",&numberIn);
		
		if((write(p0[1],&numberIn,sizeof(int)))<0){
			perror("Error with write");
			 exit(EXIT_FAILURE);
		}
		close(p0[1]);
		wait(NULL);
		
		if((read(p1[0],&numberOut,sizeof(int)))<0){
			perror("Error with read");
			exit(EXIT_FAILURE);	
		}
		close(p1[0]);
		printf("The factorial is %d\n",numberOut);
	}
	return 0;
}
