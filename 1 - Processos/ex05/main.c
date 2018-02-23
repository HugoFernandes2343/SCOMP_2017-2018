#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void fail(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
	}
}

int main (void){
	
	int child_1, child_2;
	
	int p = fork();
	fail(p);
	if(p == 0) {
		printf("filho 1\n");
		sleep(1);
		exit(1);
		printf("end 1\n");
	}
	p = fork();
	fail(p);
	if(p == 0) {
		printf("filho 2\n");
		sleep(2);
		exit(2);
		printf("end 2\n");
	}
	
	wait(&child_1);
	
	if(WIFEXITED(child_1)){
		printf("Valor de saida do filho 1: %d\n", WEXITSTATUS(child_1));
	}
	
	wait(&child_2);
	
	if(WIFEXITED(child_2)){
		printf("Valor de saida do filho 2: %d\n", WEXITSTATUS(child_2));
	}
	
	printf("end pai\n");
	
	
	return 0;
}
