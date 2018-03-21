#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include "struct.h"

void fail_fork(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
	}
}

void fail_pipe(int p){
	if(p == -1) {
		perror("Pipe falhou\n");
		exit(EXIT_FAILURE);
	}
}

void incrementRecords(record* sales){
	
	int i;
	time_t t;
	
	/* intializes RNG (srand():stdlib.h; time(): time.h) */
	srand((unsigned) time(&t));
	
	for (i = 0; i <50000; i++){
		sales[i].customer_code = rand() % 100;
		sales[i].product_code = rand() % 20;
		sales[i].quantity = rand() % 40;
	}
	
	}

int main(){
	
	/* Variables and pipes */
	record sales[50000];
	
	incrementRecords(sales);
	
	int i,j, jump = 5000;
	
	int p0[2];
	
	pid_t p;
	
	
	/* Pipes */
	
	fail_pipe(pipe(p0));
	
	
	/* Forks */
	
	for (i = 0; i < 10; i++){
		fail_fork(p=fork());
		
		if( p == 0){
			
			close(p0[0]);
			
			for (j = jump*i ; j < jump*(i+1) ; j++){
				
				if( sales[j].quantity > 20 ){
				
					if(write(p0[1],&sales[j].product_code, sizeof(int))<0){
						perror("Error writing");
						exit(EXIT_FAILURE);
					}
				}
				
			}
			
			close(p0[1]);
			exit(EXIT_FAILURE);
			}
	}
	
	close(p0[1]);
	int products[50000];
	i = 0;
	while(read(p0[0],&products[i],sizeof(int))>0){
		i++;		
	}
	
	for(j=0;j<i;j++){
		
		printf("%d;",products[j]);
		
		}
	printf("\n numeros imprimidos = %d",i);
	return 0;
	
	




}

