#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void fail(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
	}
}

int main(){
	int check;
	
	do{
		char path[100];
		printf("Introduza caminho: \n");
		/*fgets(path, 100, stdin);*/
		scanf("%s", path);
		
		int p = fork();
		fail(p);
		if(p == 0){
			int ret;
			ret = execlp ("ls", "ls", "-l", path, (char*) NULL);
			exit(ret);
		}
		int child, succ;
		wait(&child);
		
		succ = WEXITSTATUS(child);
		if(succ == 0){
			printf("O caminho existe.\n");
		} else {
			printf("O caminho não existe.\n");
		}
			
		printf("Verificar outro caminho? (0-não / 1-sim)\n");
		scanf("%d", &check);
	}while(check != 0);
}
	
	
	
	
	
