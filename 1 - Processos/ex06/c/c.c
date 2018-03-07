#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>


int main () {
    int i;
    int STATUS;
    pid_t id[4];
    
   	for (i = 0; i < 4; i++){
		id[i] = fork();
		
		if(id[i] == -1){
			printf("Error unable to fork.\n");
			exit(1);			
		}
		
        if (id[i] == 0){
			id[i] = getpid();
		   	sleep (1);	/*sleep(): unistd.h */
		 	printf ("This is the end.\n");
          	exit(0);
        }
    }
    for (i = 0; i < 4; i++){
		int p = id[i];
		if(p % 2 == 0){
			waitpid(id[i],&STATUS,0);
			printf("waiting for %d \n",id[i]);
		}
		
    }
    printf ("This is the end.\n");
}
