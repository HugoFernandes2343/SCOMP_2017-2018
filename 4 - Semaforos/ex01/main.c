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

void fail_unmap(int r){
	if(r == -1) {
			perror("Unmap falhado");
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
	pid_t p;
	sem_t *sem;
	FILE *from, *to;
	
	/**
	 * Cria semaforo
	 **/
	sem = sem_open("sem1", O_CREAT|O_EXCL, 0644, 1);
	
	
	/**
	 * Abrir ficheiros e correr verificações
	 **/
	from = fopen("Numbers.txt", "r");
	checkFile(from);
	to = fopen("Output.txt", "w");
	checkFile(to);
	
	/**
	 * Inicia ciclo de criação de processos (forks)
	 **/
	for(int i = 0; i < PROCESS; i++){
		/* cria processo */
		p = fork();
		
		/* cada processo filho */
		if(p == 0){
			/* faz wait ao semaforo, bloqueando o acesso aos ficheiros para o processo atual */
			sem_wait(sem);
			
			char numb[10];				/* Variavel que ira conter linha que foi lida do ficheiro */
			char line[200];				/* Variavel que servira para se proceder a formatação do input */
			
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
			/* faz post ao semaforo, desbloqueando assim o acesso aos ficheiros de forma a outro processo poder aceder aos mesmos */
			sem_post(sem);
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
	 * Desliga o semaforo
	 **/
	fail_sem_unlink(sem_unlink("sem1"));
	
	
	/**
	 * Mostra o conteudo do ficheiro de output
	 **/
	execlp("cat", "cat", "Output.txt", NULL);
	printf("exec falhou\n");
	exit(1);
}

