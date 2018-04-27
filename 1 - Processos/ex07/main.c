#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define ARRAY_SIZE 1000

void fail(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
	}
}


int main ()
{
	int numbers[ARRAY_SIZE];	/* array to lookup */

	time_t t;					/* needed to initialize random number generator (RNG) */
	int j;
	
	/* intializes RNG (srand():stdlib.h; time(): time.h) */
	srand ((unsigned) time (&t));

	/* initialize array with random numbers (rand(): stdlib.h) */
	for (j = 0; j < ARRAY_SIZE; j++){
		numbers[j] = rand () % 10000;
	}
	
	int i;
	int n;						/* the number to find */
	int count = 0;				/* how many times the number n is found */

	n = rand () % 10000;		/* initialize n */
	
	int p = fork();
	fail(p);
	
	/*child process*/
	if(p == 0){
		for(i=ARRAY_SIZE/2; i<ARRAY_SIZE;i++){
			if(numbers[i] == n){
				count++;
			}
		}
		exit(count);
	}
	
	for(i=0;i<ARRAY_SIZE/2;i++){
		if(numbers[i] == n){
			count++;
		}
	}
	
	int child;
	wait(&child);
	
	printf("O número %i aparece %i vezes.\n", n, count + WEXITSTATUS(child));
	  
}
