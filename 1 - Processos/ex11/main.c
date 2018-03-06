#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>

#define ARRAY_SIZE 1000
#define CONT_FILHOS 5

void fail(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
	}
}


int main(){
	int numbers[ARRAY_SIZE];	/* array to lookup */
	int result[ARRAY_SIZE]; 	
	time_t t;					/* needed to initialize random number generator (RNG) */
	int i,j,p;
	int max = INT_MIN;				/* the number to find */
	
	srand ((unsigned) time (&t)); 	/* intializes RNG (srand():stdlib.h; time(): time.h) */
	for (j = 0; j < ARRAY_SIZE; j++){	/* initialize array with random numbers (rand(): stdlib.h) */
		numbers[j] = rand () % 255;
	}
	
	/* max number check with 5 child processes*/
	for(i = 0; i<CONT_FILHOS; i++){
		p = fork();
		fail(p);
		
		if(p==0){
			int in;
			for(in = i*(ARRAY_SIZE/CONT_FILHOS); in<(i*(ARRAY_SIZE/CONT_FILHOS))+(ARRAY_SIZE/CONT_FILHOS); in++){
				if(numbers[in] > max){
					max = numbers[in];
				}
			}
			exit(max);
		}
	}
	
	int k, child;
	for(k = 0; k<CONT_FILHOS; k++){
		wait(&child);
		if(WEXITSTATUS(child) > max){
			max = WEXITSTATUS(child);
		}
	}
	
	/* a) */
	printf("Max number of the array: %i\n", max);
	int l;
	p = fork();
	fail(p);
	
	/* child calc */
	if(p == 0){
		printf("calc filhos: \n");
		for( l = 0; l < ARRAY_SIZE/2; l++ ){
			result[l]=((int) numbers[l]/max)*100;
			printf("%i: %i\n", l, result[l]);
		}
		exit(0);
	} 
	
	for( l = ARRAY_SIZE/2; l < ARRAY_SIZE; l++ ){
			result[l]=((int) numbers[l]/max)*100;
	}
	
	wait(0);
	
	printf("calc pai:\n");
	for( l = ARRAY_SIZE/2; l < ARRAY_SIZE; l++ ){
			printf("%i: %i\n", l, result[l]);
	}
}
