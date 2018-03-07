#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>

int main(){
	
  int numbers[2000];	/* array to lookup */
  int n;			/* the number to find */
  time_t t;			/* needed to initialize random number generator (RNG) */
  int i,j;
  int numberPosition;
  int status;
  int indice;

  /* intializes RNG (srand():stdlib.h; time(): time.h) */
  srand((unsigned) time(&t));

  /* initialize array with random numbers (rand(): stdlib.h) */
  for(i = 0; i < 2000; i++){
    numbers[i] = rand() % 1000;
	}
  /* initialize n */
  n = rand() % 1000;
	
	
	pid_t p;
    
   	for(i = 0; i < 10; i++){
		p = fork();
		
		if(p == -1){
			printf("Error unable to fork.\n");
			exit(1);			
		}
		
        if (p == 0){
			p = getpid();
			for(j = 0; j < 200 ; j++){
				if(numbers[j + i*200] == n){
					indice = j;
					exit(indice);}
			}
          	exit(255);
        }
    }
	for (i = 0; i < 10; i++){
		wait(&status);
		numberPosition = WEXITSTATUS(status);
		if(numberPosition != 255 && WIFEXITED(status)){
			printf("The child number is %d and it found the number %d at the position %d \n",i,n,numberPosition);
		}
	}
	return EXIT_SUCCESS;	
}

