#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
	

	int p1[2];
	fail_pipe(pipe(p1));

	
	pid_t fo1 = fork();
	fail_fork(fo1);
	
	if(fo1 == 0){
		close(p1[0]);
		dup2(p1[1], 1); /*output para o pipe1*/
		close(p1[1]);
		
		execlp("ls", "ls", "-la", (char*) NULL);
		perror("Exec failed");
		exit(1);
	}
	
	int p2[2];
	fail_pipe(pipe(p2));
	pid_t fo2 = fork();
	fail_fork(fo2);
	
	if(fo2 == 0) {

		dup2(p1[0], 0); /*input do pipe1*/
		dup2(p2[1], 1); /*output para o pipe2*/
		
		close(p1[0]);
		close(p1[1]);
		close(p2[0]);
		close(p2[1]);
		
		execlp("sort", "sort", (char*) NULL);
		perror("Exec failed");
		exit(1);
	}
	
	pid_t fo3 = fork();
	fail_fork(fo3);
	
	if(fo3 == 0){
		
		dup2(p2[0], 0); 	/*input do pipe2*/
		close(p1[0]);
		close(p1[1]);
		close(p2[0]);
		close(p2[1]);
		
		execlp("wc", "wc", "-l", (char*) NULL);
		perror("Exec failed");
		exit(1);
	}
	
	wait(0);
	return 0;	
}






