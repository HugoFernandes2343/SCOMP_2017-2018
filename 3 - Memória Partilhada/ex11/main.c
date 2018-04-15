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

typedef struct circular_buffer
{
    int numbers[10];     // data buffer
    int write;       // index to write
    int read;       // index to read
} circular_buffer;

int main(void){
	int p0[2]; //array for later piping
	circular_buffer cb;// circular buffer struct
	
	//buffer starting point
	cb.numbers[0]=0;
	cb.write=0;
	cb.read=0;
	
	//pipe
	fail_pipe(pipe(p0));
	
	//fork
	pid_t p = fork();
	fail_fork(p);
	
	if(p==0){//processo filho consumer
		for (int i = 0; i < 30; i++)//ciclo for para ler do array e dar print dos 30 valores
		{	
			/*READ*/			
			//este read lé o array numbers escrito pelo producer
			if(read(p0[0],cb.numbers,sizeof(int)*10)<0){
				perror("Error reading");
				exit(EXIT_FAILURE);
			}
			
			//coloca na variavel num o numero presente na posicao de leitura, no array que foi lido anteriormente
			int num = cb.numbers[cb.read];

			//print do numero
			printf("Numero %d foi lido\n",num);
			
			if(cb.read==9){//verifica se o indice de leitura indica a posicao final do array
				
				cb.read=0;//se indicar a posição final volta a colocar se na primeira posição
			
			}else{
				
				cb.read+=1;//se nao estiver incrementa-se esse mesmo indice
			
			}
			
		}
		
		close(p0[0]);
		close(p0[1]);
		
		exit(EXIT_SUCCESS);
		
	}else{//processo  pai producer
		for (int i = 0; i < 30; i++)// ciclo for para criar os valores e escreverlos no array antes de passar para o consumer
		{	
						
			cb.numbers[cb.write] = (i+1)*2;//escrita de um valor no array numbers,na posicao indicada no indice de escrita (write)
			
			/*WRITE*/
			// escreve no pipe o array numbers apos a alteracao da posicao
			if(write(p0[1],cb.numbers,sizeof(int)*10)<0){
				perror("Error writing");
				exit(EXIT_FAILURE);
			}
			
			if(cb.write==9){//verifica se o indice de escrita se encontra na posicao final do array
				
				cb.write=0; //se indicar a posicao final volta a colocar se na primeira posicao
			
			}else{
			
				cb.write +=1; //se nao estiver incrementa-se esse mesmo indice
			}
			
		}
		
		close(p0[0]);
		close(p0[1]);
		
		wait(NULL);
	}
	return 0;
}
