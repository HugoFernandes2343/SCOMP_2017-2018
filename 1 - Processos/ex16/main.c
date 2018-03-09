#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>

void my_exec(char *command){
	execlp(command,command,NULL);
	perror("there was an errror with exec");
}
	
	
int main(){
	
	
	
	int status;
	char str[20];
	pid_t p;
	
	printf("Command to run\n");
	scanf("%s",str);
	
	p=fork();
	
	
	if(p==-1){
		perror("Error in fork\n");
	}
	
	if(p==0){
		my_exec(str);
	}else{
	   wait(&status);
	   if(WIFEXITED(status)){
		   printf("fim do filho %d \n",WEXITSTATUS(status));
		   }
	}
	
	return EXIT_SUCCESS;
	
}	
