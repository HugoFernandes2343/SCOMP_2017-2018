#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define NR_CHILDS 10

void fail(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
	}
}


int main(){
	int i;
	int p;
	for(i = 0; i<NR_CHILDS; i++){
		p = fork();
		if(p == 0){
			int j;
			for(j = i*100+1; j<(i+1)*100; j++){
				printf("%i  ", j);
			}
			
			printf("\n\n");
			exit(i);
		}
	}
	
	wait(0);
	
}
