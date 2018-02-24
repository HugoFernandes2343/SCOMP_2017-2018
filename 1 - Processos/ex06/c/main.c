#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main () 
{
    int i;
    int status;

    for (i = 0; i < 4; i++)
    {
		pid_t p = fork ();
        if (p == 0) {
			
            sleep (1);		/*sleep(): unistd.h */
        }
        else
        {
			if(p % 2 == 0){
				waitpid(p);
			}
		}
    }
    printf ("This is the end.\n");
}
