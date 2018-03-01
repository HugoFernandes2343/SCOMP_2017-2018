#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>


int main () 
{
    int i;
   	for (i = 0; i < 4; i++){
		pid_t p = fork ();
        if (p == 0){
		    		sleep (1);		/*sleep(): unistd.h */
		    		printf ("This is the end.\n");
            		exit(0);
        }
	}	
    	printf ("This is the end.\n");
}
