#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

int main(void){
	
	/* Variables and pipes */
	
	int *pipes[5];
	
	int p0[2],p1[2],p2[2],p3[2],p4[2], vec1[1000],vec2[1000],vecRes[1000]; 
	
	int i,j,jump = 200, sumR,sumS;
	time_t t;
	pid_t p;
	
	/* intializes RNG (srand():stdlib.h; time(): time.h) */
	srand((unsigned) time(&t));

	/* initialize array with random numbers (rand(): stdlib.h) */
	for(i = 0; i < 2000; i++){
		vec1[i] = rand() % 1000;
		vec2[i] = rand() % 1000;
	}
	
	/* Pipes */
	if(pipe(p0)<0 || pipe(p1)<0 || pipe(p2)<0 || pipe(p3)<0 || pipe(p4)<0){
		perror("Error creating pipes\n");
		exit(EXIT_FAILURE);
		}

	pipes[0] = p0;
	pipes[1] = p1;
	pipes[2] = p2;
	pipes[3] = p3;
	pipes[4] = p4;
	

	/* Forks */
	
	for (i = 0; i < 5; i++){
		
		p = fork();
				
		if(p == 0){
			
			int *ptr = pipes[i];
			close(*ptr);
			ptr++;
			
			for(j = jump*i; j < jump*(i+1) ;j++){
				sumS = vec1[j] + vec2[j];
				if(write(*ptr,&sumS, sizeof(sumS))<0){
					perror("Error writing");
					exit(EXIT_FAILURE);
				}
				
			}
			
			close(*ptr);
			exit(EXIT_FAILURE);
			
		}else if(p <0){
			perror("Error wiith fork \n");
			exit(EXIT_FAILURE);
		}
			
	}
	
	for(i = 0; i < 5; i++){
		
		int *ptr2 = pipes[i];
		ptr2++;
		close(*ptr2);
		ptr2--;
		
		for(j = jump*i; j < jump * (i+1);j++){
			if(read(*ptr2,&sumR,sizeof(int)) < 0){
					perror("Error reading");
					exit(EXIT_FAILURE);			
			}
			vecRes[j] = sumR;
		}
		
		close(*ptr2);
	}

	for(j = 0; j < 1000; j++){
		printf("%d\n",vecRes[j]);
	}
	
	return EXIT_SUCCESS;
}
