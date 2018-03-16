#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>

#define SIZE 50


int main(){
	
	int status;
	char m1[SIZE] = "Hello World";
	char m2[SIZE] = "goodbye";
	int fd[2];	 
	
	if(pipe(fd)<0){
		 perror("Erro creating pipe \n");
		 exit(EXIT_FAILURE);
		 }
	 
	pid_t p = fork();
	 
	if( p < 0 ){
		 perror("Error at the fork \n");
		 exit(EXIT_FAILURE);
		 }
		 
	if( p != 0){
		close(fd[0]);
	
		if(write(fd[1],m1,SIZE)<0){
			 perror("Error with write\n");
			 exit(EXIT_FAILURE);			 
			 }
		if(write(fd[1],m2,SIZE)<0){
			 perror("Error with write\n");
			 exit(EXIT_FAILURE);			 
			 }
		close(fd[1]);
		wait(&status);
		if(WIFEXITED(status)){
		   printf("PID do filho %d, exit status do filho %d \n",p,WEXITSTATUS(status));
		   }
	}else{
		close(fd[1]);
		
		char p1[SIZE];
		char p2[SIZE];
				
		if(read(fd[0],p1,SIZE)<0){
			perror("Error with read\n");
			 exit(EXIT_FAILURE);
			}
		printf("%s \n",p1);
		if(read(fd[0],p2,SIZE)<0){
			perror("Error with read\n");
			 exit(EXIT_FAILURE);
			}
		printf("%s \n",p2);
		
		close(fd[0]);
		exit(EXIT_SUCCESS);
	}
	
}
