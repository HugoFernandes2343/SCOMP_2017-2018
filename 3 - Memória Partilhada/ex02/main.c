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

#define SIZE 1000000

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

int randomNum() { // funcao para obter um numero aleatorio de 0 a 10

	int number;
	time_t t;
	
	srand((unsigned)time(&t));
	
	number = rand() % 10;
	
	return number;
	
}
typedef struct {
	int values[1000000]; //Tamanho do array
	int newData; //flag
} shared;

int main(void){
	int fd, size = sizeof(shared);
	shared *sh1; //primeira struct usada para testar a passagem atraves de memoria partilhada
	clock_t shared1,shared2;
	double dif1; //clocks usados para temporizar a transferencias de dados
	pid_t p;
	
	//partilha da memoria
	fd = shm_open("/shm",O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR);
	ftruncate(fd,size);
	sh1 = (shared*)mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	
	// fork
	p=fork();
	fail_fork(p);
	
	if(p>0){ 	//processo pai
		
		
		for(int i = 0; i< SIZE-1;i++){
				sh1->values[i] = randomNum(); //introducao do numero aleatorio na posicao i de values
		}
		sh1->newData=1; // alteraçao da flag
		
		shared1 = clock();//clock inicial sharedmemory
		sh1->newData=0;
		
		wait(NULL); //espera pelo processo filho
		
		shared2 =clock(); //clock final sharedmemory
		dif1 = (double)(shared2-shared1)/CLOCKS_PER_SEC; //diferenca
		printf("Tempo de passar por memoria partilhada em miliesimos de segundo %f\n", dif1*1000);
		
	}else{	//processo filho
		while(sh1->newData==0){ //verificação da flag
		}
		exit(EXIT_SUCCESS);
	}
	
	//fecho da memoria partilhada e subsequente remoção
	fail_unmap(munmap(sh1,size));
	fail_close(close(fd));
	fail_unlink(shm_unlink("/shm"));
	
	shared sh2;// segunda struct para testar o modo de trasnferencia por pipes
	sh2.newData=0; //flag
	clock_t pipe1,pipe2; //clocks para temporização da partilha por pipes
	double dif2; //valor para guardar a diferenca dos clocks que sera utilizada para mostrar os milesimos de segundo que demora a transferencia
	
	//pipe
	int p0[2];
	fail_pipe(pipe(p0));
	
	//fork
	p=fork();
	fail_fork(p);
	
	if(p>0){//proceso pai
		
		close(p0[0]); // fecho da leitura
		
		//colocar os numeros aleatorios em values, e alteração da flag
		for(int i = 0; i< SIZE-1;i++){
				sh2.values[i] = randomNum();
		}
		sh2.newData=1;
		
		pipe1 = clock();// clock inicial
		
		if(write(p0[1],&sh2,sizeof(shared))<0){// escrita para o filho
				perror("Error writing");
				exit(EXIT_FAILURE);
		}
		close(p0[1]);//fecho da escrita
		
		wait(NULL);//espera pelo filho
		
		pipe2 = clock();//clock final
		dif2 = (double)(pipe2-pipe1)/CLOCKS_PER_SEC; //diferenca de tempo em segundos
		
		printf("Tempo de passar por pipes em miliesimos de segundo %f\n", dif2*1000);
		
	}else{
		
		close(p0[1]); // fecho da escrita
		
		if((read(p0[0],&sh2,sizeof(shared)))<0){ //leitura
			perror("Error with read");
			exit(EXIT_FAILURE);	
		
		}
		close(p0[0]); //fecho da leitura
		
		exit(EXIT_SUCCESS);
	}
	
	return 0;
}	
