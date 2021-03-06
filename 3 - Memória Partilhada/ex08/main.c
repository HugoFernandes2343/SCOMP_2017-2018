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

#define STR_SIZE 50
#define NR_DISC 10

typedef struct aluno {
	int numero;
	char nome[STR_SIZE];
	int disciplinas[NR_DISC];
	int flag;
}aluno;

void fail_fork(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
	}
}

void fail_unmap(int r){
	if(r == -1) {
			perror("Unmap falhado");
	}
}

void fail_close(int r){
	if(r == -1) {
			perror("Close falhado");
	}
}

void fail_unlink(int r){
	if(r == -1) {
			perror("Shm unlink falhou");
	}
}

int main(void){
	
	aluno* al; //criacao do pointer para a estrutura aluno 
	int fd, size = sizeof(aluno),i; // calculo do size e int para criacao da memoria e for's
	
	// criação da memoria partilhada
	fd = shm_open("/shm",O_CREAT|O_EXCL|O_RDWR, S_IWUSR|S_IWUSR);
	ftruncate(fd, size);
	al = (aluno*)mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
	
	// fork
	pid_t p = fork();
	fail_fork(p);
	
	if(p==0){//processo filho
		
		// verificção da flag
		while(al->flag == 0) {	
		}
		
		//variaveis do filho
		int high=0,low=21,avg=0;
		
		for (i = 0; i < NR_DISC; i++){//percorrer e verificar  a maior e menor nota, assim como a media
			
			printf("Nota da disciplina numero %d: %d\n",i+1,al->disciplinas[i]);//print da nota de uma disciplina 
			
			if(al->disciplinas[i] < low) { //alteração do minimo
				low = al->disciplinas[i];
			}
			
			if(al->disciplinas[i] > high) { //alteração do maximo
				high = al->disciplinas[i];
			}
					
			avg += al->disciplinas[i]; //alteração da media
			
		}
		
		printf("Pior nota obtida: %d\n",low); //Print da menor nota
		
		printf("Melhor nota obtida: %d\n",high); //Print da maior nota
		
		printf("Média das notas obtidas: %d\n", (avg/NR_DISC)); //Print da media das notas
		
		exit(EXIT_SUCCESS);
		
	}else{//processo pai
		
		//pedido do nome
		printf("Nome do aluno: ");
		fgets(al->nome, STR_SIZE, stdin);
		
		//pedido do numero
		printf("Numero mecanografico: ");
		scanf("%d", &al->numero);
		
		for(i = 0; i < NR_DISC; i++) {// percorre pedindo a nota de uma discplina (notas devem ser de 0 a 20)
			printf("Disciplina %d: ", i+1);
			scanf("%d", &al->disciplinas[i]);
		}
		al->flag = 1; // alteração da flag que indica a alteraçao nos dados
		
		wait(NULL); // colocar o processo pai em espera ate que o filho acabe para apagar a memoria partilhada
		
		fail_unmap(munmap(al,size)); //unmapping of the memory
		fail_close(close(fd)); // closing of the memory
		fail_unlink(shm_unlink("/shm")); // unlinking of the memory
	}
}
