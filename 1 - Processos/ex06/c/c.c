#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>


int main () {
    int i;
    int STATUS;
    int id[4];
   	for (i = 0; i < 4; i++){
		pid_t p = fork ();
        if (p == 0){
			id[i] = getpid();
		   	sleep (1);	/*sleep(): unistd.h */
		 	printf ("This is the end.\n");
          	exit(0);
        }
    }
    for (i = 0; i < 4; i++){
		int p = id[i];
		if(p % 2 == 0){
			waitpid(p,&STATUS,0);
			printf("waiting for %d \n",p);
		}
		
    }
    printf ("This is the end.\n");
}
