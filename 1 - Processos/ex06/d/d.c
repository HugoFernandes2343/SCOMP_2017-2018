#include <stdio.h>
#include <unistd.h>	

int main () 
{
    int i;
    int status;
	int childnumber = 0;
	
    for (i = 0; i < 4; i++){
		pid_t p = fork ();
        if (p == 0){
		    		sleep (1);		/*sleep(): unistd.h */
		    		printf ("This is the end.\n");
            		exit(0);
        }else{
			if(p % 2 == 0){
				waitpid(p,NULL,0);
				printf("waiting for %d \n",p);
			}
		}
    }
        if (fork () == 0) {
			childnumber = childnumber + 1;
			printf("Child number %d \n",childnumber);
            sleep (1);		/*sleep(): unistd.h */
        }
    }
    printf ("This is the end.\n");
}
