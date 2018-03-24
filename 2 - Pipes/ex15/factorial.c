#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>


int main(){
	int number;
	int i;
	if((read(STDIN_FILENO,&number,sizeof(int)))<0){
			perror("Error with read");
			exit(EXIT_FAILURE);	
		}
	
	for (i = number-1; i > 0 ; i--)
	{
		number = number*i;
	}
	
	if((write(STDOUT_FILENO,&number,sizeof(int)))<0){
			perror("Error with write");
			 exit(EXIT_FAILURE);
		}
		
	return 0;
}
