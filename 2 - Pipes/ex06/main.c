#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ARRAY_SIZE 100
#define CHILD_PROCESSES 5

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
	
	int index, vec1[ARRAY_SIZE], vec2[ARRAY_SIZE];
	for(index=0; index<ARRAY_SIZE; index++){
		vec1[index] = 1;
		vec2[index] = 2;
	}
		
	int fd[2];
	fail_pipe(pipe(fd));
	
	int i;
	for(i=0;i<CHILD_PROCESSES;i++){
		
		pid_t p = fork();
		fail_fork(p);
		
		if(p == 0){
			close(fd[0]);
			int a;
			int tmp =0;
			for(a=i*(ARRAY_SIZE/CHILD_PROCESSES); a<(i*(ARRAY_SIZE/CHILD_PROCESSES))+(ARRAY_SIZE/CHILD_PROCESSES); a++){
				tmp += vec1[a] + vec2[a];
			}
			if(write( fd[1], &tmp, sizeof(tmp))<0){
				perror("Error writing");
			}
			close(fd[1]);
			exit(0);
		}
	}
		
	int tmp, sum =0;
	close(fd[1]);
	for(i = 0; i<CHILD_PROCESSES; i++){
		if( read(fd[0], &tmp, sizeof(tmp)) < 0 ){
				perror("Error reading");			 
		}
		sum += tmp;
	}
	close(fd[0]);
	
	printf("Soma: %d\n", sum);
}






