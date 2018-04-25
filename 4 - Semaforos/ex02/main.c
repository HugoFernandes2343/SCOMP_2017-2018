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
#include <semaphore.h>
#include <sys/mman.h>

#define PROCESS 8
#define LINES 200

/**
 * Verifica se o ficheiro a abrir existe
 **/
void checkFile(FILE *f){
		if( f == NULL ){
			perror("File does not exist teste");
		}
}

/**
 * Verifica se a criação do semaforo falhou
 **/
void fail_sem(sem_t *s){
	if(s == SEM_FAILED) {
		perror("Criação do semaforo falhou\n");
		exit(EXIT_FAILURE);
	}
}

/**
 * Verifica se a remoçao do semaforo falhou
 **/
void fail_sem_unlink(int s){
	if(s == -1) {
		perror("Remoção do semaforo falhou\n");
		exit(EXIT_FAILURE);
	}
}

/**
 * Verifica se o fork falhou
 **/
void fail_fork(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
	}
}


int main(){
	
	/**
	 * Iniciar variaveis
	 **/
	FILE *from, *to;
	pid_t p;
	sem_t *sem[8];
	
	/**
	 * Cria semaforos
	 **/
	char *sem_name[] = {"sem1","sem2","sem3", "sem4", "sem5", "sem6", "sem7", "sem8"};
	for(int i = 0; i<8; i++){
		sem[i] = sem_open(sem_name[i], O_CREAT|O_EXCL, 0644, 0);
	}
	
	/**
	 * Abrir ficheiros e correr verificações
	 **/
	from = fopen("Numbers.txt", "r");
	checkFile(from);
	to = fopen("Output.txt", "w");
	checkFile(to);
	
	/**
	 * Incrementa o primeiro semaforo de forma a este ter acesso aos ficheiros
	 **/
	sem_post(sem[0]);
	
	/**
	 * Inicia ciclo de criação de processos (forks)
	 **/	 
	for(int i = 0; i < PROCESS; i++){
		/* cria processo */
		p = fork();			
		
		/* cada processo filho */
		if(p == 0){
			/* faz wait ao semaforo, bloqueando o acesso aos ficheiros para o processo atual */
			sem_wait(sem[i]);
			
			char numb[20];				/* Variavel que ira conter linha que foi lida do ficheiro */
			char line[200];				/* Variavel que servira para se prroceder a formatação do input */
			
			/* ciclo que para cada processo vai ler o nr correto de linhas */
			for(int j = 0; j < LINES;j++){
					/*le do ficheiro */	
					fgets(numb, sizeof(numb), from);

					/* formata input */
					sprintf(line, "Process: %i, line nr: %i, %s", i, j, numb);

					/* escreve no ficheiro de output */
					fprintf (to, "%s", line);
			}
			
			fflush(to);
			/* faz post ao semaforo do processo seguinte, desbloqueando assim o acesso aos ficheiros de forma ao outro processo poder aceder aos mesmos */
			sem_post(sem[i+1]);
			exit(0);
		}
	}
	
	/**
	 * Espera que todos os processos filho acabem
	 **/
	for(int i=0; i<PROCESS; i++){
		wait(NULL);
	}
	
	/**
	 * Fecha os ficheiros
	 **/
	fclose(from);
	fclose(to);
	
	/**
	 * Desliga os semaforos
	 **/
	for(int i = 0; i<8; i++){
		fail_sem_unlink(sem_unlink(sem_name[i]));
	}
	
	/**
	 * Mostra o conteudo do ficheiro de output
	 **/
	execlp("cat", "cat", "Output.txt", NULL);
	printf("exec falhou\n");
	exit(1);
}

