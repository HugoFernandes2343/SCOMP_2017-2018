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
#include "struct.h"

#define RECORDS 100

/**
 * Verifica se a criação do semaforo falhou
 **/
void fail_sem(sem_t *s){
	if(s == SEM_FAILED) {
		perror("Criação do semaforo falhou\n");
		exit(EXIT_FAILURE);
	}
}

void fail_unmap(int r){
	if(r == -1) {
			perror("Unmap falhou");
	}
}

void fail_close(int r){
	if(r == -1) {
			perror("shm close falhou");
	}
}

void fail_unlink(int r){
	if(r == -1) {
			perror("Shm unlink falhou");
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

void consult (db *dt, sem_t *sem[]){
	
	/* pede ao utilizador para introduzir o nr a pesquisar*/
	int nr;
	printf("Introduza o nr a pesquisar:\n");
	scanf("%d", &nr);
	
		int rec = -1, indx;
		printf("nr a pesquisar: %i\n", nr);
		for(int i =0; i<RECORDS; i++){			/*ciclo que passa por todos os records*/
			
			sem_wait(sem[i]);		/*tenta fazer wait ao semaforo de cada record*/
			//if(check == 0){							/*caso tenha sido bem sucedido o wait (se nao tiver conseguido fazer wait vai ignorar este record) */
				if(nr == dt->data[i].number){			/*caso o nr a procurar seja igual ao do ciclo */
					rec = dt->data[i].number;				/*guarda index do record*/
					indx = i;
				} else {								/*se o nr a procurar for diferente*/
					sem_post(sem[i]);						/*faz post ao semaforo*/
				}
			//}
		}
		
		if(rec >-1 ){							/* caso tenha sido encontrado o record pretendido */
			printf("Number:\t\t%i \nName:\t\t%s \nAddress:\t%s\n", dt->data[rec].number, dt->data[rec].name, dt->data[rec].address);
			sem_post(sem[indx]);					/*faz post ao semaforo do record no fim de aceder ao mesmo*/
		} else {
			printf("Não encontrado\n");
		}		
}

int insert (db *dt, sem_t *sem[], int indx){
	
	int nr;
	char nm[50], add[500];
	printf("Introduza o nr: \n");
	scanf("%d", &nr);
	printf("Introduza o nome: \n");
	scanf("%s", nm);
	printf("Introduza a morada:\n");
	scanf("%s", add);
	
	printf("record a introduzir em memoria: \nNR: %d \nNome: %s \nMorada: %s\n", nr, nm, add);
	
	sem_wait(sem[indx]);
	
	dt->data[indx].number=nr;
	strcpy(dt->data[indx].name, nm);
	strcpy(dt->data[indx].address, add);
	
	sem_post(sem[indx]);
	
	return indx+1;
}

void consultAll(db *dt, sem_t *sem[], int indx){
	
	for(int i=0;i<indx; i++){
			printf("Rec %i\n", i);
			
			sem_wait(sem[i]);
			
			printf("Number: %d\n", dt->data[i].number);
			printf("Name: %s\n", dt->data[i].name);
			printf("Address: %s\n", dt->data[i].name);
			
			sem_post(sem[i]);
	}
	
}



int main(){
	
	/**
	 * Iniciar variaveis
	 **/
	char names[RECORDS][10];
	char baseName[5] = "sem";
	for(int i = 0; i< RECORDS; i++){
		char temp_string[10];
		sprintf(temp_string, "%s%i", baseName, i);	
		strcpy(names[i], temp_string);
	}
	sem_t *sem[RECORDS];
	
	int fd, data_size = sizeof(db);	
	db *dataBase;
	
	int writeIndex=0;

	/**
	 * Cria semaforos
	 **/
	for(int k = 0; k<RECORDS; k++){
		sem[k] = sem_open(names[k], O_CREAT|O_EXCL, 0644, 1);
		fail_sem(sem[k]);
	}
	
	/**
	 * cria memoria partilhada
	 **/
	/*Criar memoria*/
	fd = shm_open("/shmem", O_CREAT|O_EXCL|O_RDWR, S_IWUSR|S_IRUSR);
	/*Definir tamanho da memoria*/
	ftruncate (fd, data_size);
	/*mapear objeto de memoria partilhada*/
	dataBase = (db*) mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	
	
	int menu;
	do{
		printf("---------\nEscolha a operação: \nConsult - 1 \nInsert - 2 \nConsult All - 3 \nExit - 0\n");
		scanf("%d", &menu);
		switch(menu){
			
			case 1:
					consult(dataBase, sem);
					break;
			case 2:
					writeIndex = insert(dataBase, sem, writeIndex);
					break;
			case 3:
					consultAll(dataBase, sem, writeIndex);
					break;
			default:
					break;
		}
	}while (menu !=0); 
	
	
	/**
	 * Desliga os semaforos
	 **/
	for(int j = 0; j<RECORDS; j++){
		fail_sem_unlink(sem_unlink(names[j]));
	}
	
	/**
	 * Desliga o objeto de memoria partilhada, fecha o descritor e remove a memoria partilhada do sistema de ficheiros
	 **/
	fail_unmap(munmap(dataBase, data_size));
	fail_close(close(fd));
	fail_unlink(shm_unlink("/shmem"));
	
	return 0;
}








