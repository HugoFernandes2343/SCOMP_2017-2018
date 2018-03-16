#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>

int main(){
	 
	 pid_t temp;
	 
	 int fd[2];
	 
	 if(pipe(fd)<0){
		 perror("Erro creating pipe");
		 exit();
		 }
	 
	 pid_t p = fork();
	 
	 if( p < 0 ){
		 perror("Error at the fork");
		 exit();
		 }
		 
	 if(p != 0){
		close(fd[0]);
		temp = p;
		
		 if(write(fd[1],&temp,sizeof(temp))<0){
			 perror("Error with write");
			 exit();			 
			 }
			 
		 printf("o meu id é %d\n",p);	 
		 close(fd[1]);
	 }else{
		close(fd[1]);
		
		if(read(fd[0],&temp,sizeof(temp)) < 0){
			perror("Error with read");
			exit();		
			}
			
		printf("o id do meu pai é %d\n",temp);
		close(fd[0]);
	 }
	 
	return 0;
}
