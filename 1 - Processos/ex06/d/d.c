#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>


int main () {
    int i;
    int status;
    pid_t id[4];
    
   	for (i = 0; i < 4; i++){
		id[i] = fork();
		
		if(id[i] == -1){
			printf("Error unable to fork.\n");
			exit(1);			
		}
		
        if (id[i] == 0){
		   	sleep (1);	/*sleep(): unistd.h */
		 	printf("This is the end of the process created in order %d. \n", i+1);
          	exit(i+1);
        }
    }
    for (i = 0; i < 4; i++){
		if(id[i] % 2 == 0){
			waitpid(id[i],&status,0);
			printf("Ended the wait for %d (child number %d)\n",id[i],WEXITSTATUS(status));
		}
		
    }
    printf("This is the end.\n");
}
