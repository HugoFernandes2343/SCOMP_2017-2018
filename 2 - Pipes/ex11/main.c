#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>


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

int greater(int i,int j){
		if(i>j){
			return i;
		}else{
			return j;
		}
}
	
int main(){
	
	/* Variaveis e pipes */
	
	time_t t;
	pid_t p;
	int pipes [6][2];
	int i;
	int number=-1;
	int maxNumber=0;
	
	/* intializes RNG (srand():stdlib.h; time(): time.h) */
	srand((unsigned) time(&t));
	
	for (i = 0; i<6; i++){
		int temp[2];
		fail_pipe(pipe(temp));
		pipes[i][0] = temp[0];
		pipes[i][1] = temp[1];
	}
	
	number = rand() % 500;
	printf("The first number is %d \n",number);
	/*Forks*/
	
	
	for (i = 1; i < 6; i++){
		
		p = fork();
		fail_fork(p);
		
		if(p == 0){
			
			int send=-1, comp=-1;
			
			number = rand() % 500;
			
			printf("Im child number %d, my PID is %d, and my number was %d \n",i,getpid(),number);
			
			close(pipes[i][0]);
			close(pipes[i-1][1]);
				
			/*READ*/			
			
			if(read(pipes[i-1][0],&comp,sizeof(int))<0){
				perror("Error reading");
				exit(EXIT_FAILURE);
			}
			
			/*COMPARISON*/
			
			send = greater(comp,number);
			
			/*WRITE*/
			
			if(write(pipes[i][1],&send,sizeof(int))<0){
				perror("Error writing");
				exit(EXIT_FAILURE);
			}

							
			close(pipes[i][1]);
			
			close(pipes[i-1][0]);
				
			exit(EXIT_SUCCESS);
			
			}
			
		}

	close(pipes[0][0]);
	
	/*WRITE*/
	if(write(pipes[0][1],&number,sizeof(int))<0){
			perror("Error writing");
			exit(EXIT_FAILURE);
	}
	
	close(pipes[0][1]);
	
	close(pipes[5][1]);
	
	if(read(pipes[5][0],&maxNumber,sizeof(int))<0){
			perror("Error reading");
			exit(EXIT_FAILURE);
	}
			
	close(pipes[5][0]);
			
	printf("The max number is %d\n",maxNumber);
			
			
	return 0;
}
