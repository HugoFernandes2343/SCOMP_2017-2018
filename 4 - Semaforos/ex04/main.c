#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>

//documento que mostra o raciocinio: algoritmo.txt

void fail_sem(sem_t *sem){
	if(sem==SEM_FAILED){
			perror("sem_open falhou");
		exit(1);
	}
}

void fail_Sunlink(int i){
	if(i==-1){
			perror("sem unlink falhou");
			exit(1);
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

typedef struct line
{
	char str[50][80];
	int count;
} line;

int main(){
	
	sem_t *semaforo; //semaforo
	line* ln; //apontador para a estrutura
	int fd=0, size = sizeof(line); // memory sharing variables
	char s[80]; //array para a mensagem
	int i; //Variavel para escolher a escrita ou remocao
	struct timespec ts;

	if (clock_gettime(CLOCK_REALTIME, &ts) == -1){
		/* handle error */
		return -1;
	}

	ts.tv_sec += 12;
	
	//Concatenar a mensagem
	sprintf(s,"I’m the Father - with  PID  %d",getpid());
	
	//memory sharing
	fd = shm_open("/shm", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	ftruncate(fd, size);
	ln = (line*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	//open do semaforo
	semaforo = sem_open("semaforo", O_CREAT, 0644, 1);
	fail_sem(semaforo);
	
	printf("1- Escrever 1 frase \n 2- Remover uma frase");
	scanf("%d",&i);
	
	if(i==1){
	
		sem_wait(semaforo);// muda o semaforo para 0 para que mais nenhum processo possa aceder

		int num = ln->count; //obetem o numero de mensagens que ja foram introduzidas na matriz
	
		strcpy(ln->str[num],s); //introduz uma mensagem na matriz
	
		printf("%s\n",ln->str[num]); //da print ao que foi introduzido na matriz
	
		ln->count++;//incrementa o contador de mensagens
	
		sleep(2);//sleep
	
		sem_post(semaforo); // muda o semaforo para 1 indicando que outros processos vao poder mexer na memoria partilhada
	
	}
	
	if(i==2){
		
		sem_timedwait(semaforo,&ts); // muda o semaforo para 0 se nao conseguir em menos de 12 segundos falha
		
		int num = ln->count; //obtem o numero de mensagens que ja foram introduzidas na matriz
		
		strcpy(ln->str[num],""); //retira da matriz uma frase
		
		ln->count--; // decrementa o contador de mensagens
		
		sleep(2); //sleep	
		
		sem_post(semaforo); // muda o semaforo para 1 indicando que outros processos vao poder mexer
	}
	
	return 0;
}


