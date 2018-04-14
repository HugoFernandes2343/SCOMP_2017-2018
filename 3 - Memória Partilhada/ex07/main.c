#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define CHILDS 10
#define NUMBS 10000

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


int main(){
	
	/*Iniciar variaveis*/		
	pid_t fo;
	time_t t;
	srand ((unsigned) time (&t));
	
	/*Preenche array com numeros random*/
	int numbers[NUMBS];
	for(int i= 0; i<NUMBS; i++){
		numbers[i] = rand()%1000;
	}
	
	/*cria pipe*/
	int pi[2];
	fail_pipe(pipe(pi));
	
	
	/*ciclo for que cria cada processo filho*/
	for(int i = 0; i < CHILDS; i++){
		fo = fork();
		if(fo == 0){
			int max = -1;
			/*ciclo for que para cada filho define qual o intervalo de numeros que ele vai percorrer*/
			for (int j = i*NUMBS/CHILDS; j < (i*NUMBS/CHILDS)+NUMBS/CHILDS; j++){
				if(numbers[j] > max){
					max = numbers[j];
				}
			}
			
			close(pi[0]);
			/*escrever no pipe*/
			if(write(pi[1], &max, sizeof(max)) < 0){
				perror("F- Error writing");	
			}
			close(pi[1]);
			exit(0);
		}
	}
	
	close(pi[1]);
	/*calcula o max global, imprime resultado*/
	int max = -1;
	for(int i = 0; i<10; i++){
		int local;
		/*le do pipe*/
		if(read(pi[0], &local, sizeof(local)) <0){
					perror("F- Error reading");
		}
		printf("Local Max nr %i: %i\n", i, local);
		if(local > max){
			max = local;
		}
	}
	printf("Global max: %i\n", max);
	close(pi[0]);
	
	return 0;
	
}












