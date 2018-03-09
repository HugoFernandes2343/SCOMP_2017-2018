#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>

int spawn_childs(int number){
	
	
	pid_t p;
	
	for (int i = 0; i < number; i++)
	{
		p = fork();
		
		if(p ==-1){
			perror("error at fork");
			}
		if(p==0){
			int ordem = i + 1;
			return ordem;
			}
	}
	return 0;
		
}

int main(){
	
	int i = 0,indice,status;
	while(i < 1){
		printf("Quantos filhos criar?\n");
		scanf("%d",&i);
		if(i < 1){
			printf("introduza um valor apropriado\n");
			}
	}
	
	indice = spawn_childs(i);
	if(indice > 0){
		exit(indice*2);
	}
	
	if ( indice == 0 ){
		printf("the parent returns %d \n",indice);
	}
	for (int j = 0; j < i; j++){
		wait(&status);
		if(WIFEXITED(status)){
			printf("The child number %d returns the value %d \n",j+1,WEXITSTATUS(status));
		}
	}	
}
	
	
	
	
	

