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

typedef struct circular_buffer
{
    int numbers[10];     // data buffer
    int count;     // number of items in the buffer
    int write;       // index to write
    int read;       // index to read
} circular_buffer;

int main(void){
	
	circular_buffer* cb; //pointer to circular buffer
	int fd, size = sizeof(circular_buffer); // memory sharing variables
	
	//memory sharing
	fd = shm_open("/shm", O_CREAT|O_EXCL|O_RDWR, S_IWUSR|S_IWUSR);
	ftruncate(fd, size);
	cb = (circular_buffer*)mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
	
	//buffer starting point definition
	cb->numbers[0]=0;
	cb->write = 0;
	cb->read = 0;
	cb->count=0;
	
	//fork
	pid_t p = fork();
	fail_fork(p);

	if(p==0){ //processo filho, consumer
	
		for (int i = 0; i < 30; i++){//ciclo for para ler do array e dar print dos 30 valores
			
			while(cb->count==0){//verificação da flag count
			}
			
			int num = cb->numbers[cb->read]; //colocamos o numero presente na posicao de leitura do array numbers na variavel num
			
			cb->count -= 1;//alteração do numero de valores "presente" no array, esta redução nao indica mesmo a remoção de um valor, mas significa que este valor ja foi lido por isso ja se pode escrever nesta posicao
			 
			printf("Numero %d foi lido\n",num);//print do numero lido
			
			if(cb->read==9){//verifica se o indice de leitura indica a posicao final do array
				
				cb->read=0; //se indicar a posição final volta a colocar se na primeira posição
			
			}else{
				
				cb->read+=1;//se nao estiver incrementa-se esse mesmo indice
			
			}
			
		}
		
		exit(EXIT_SUCCESS);
		
	}else{ //processo pai, producer
		
		for (int i = 0; i < 30; i++){// ciclo for para criar os valores e escreverlos no array antes de passar para o consumer
		
			while(cb->count==10){//verificação da flag count
			}
			
			cb->numbers[cb->write] = (i+1)*2;//escrita de um valor no array numbers,na posicao indicada no indice de escrita (write)
			
			cb->count += 1; // incremento do numero de valores que estao "presentes" no array, ou seja o numero de valores no array que ainda nao foram lidos e portanto nao se pode escrever nas suas posicoes.
			
			if(cb->write==9){ //verifica se o indice de escrita se encontra na posicao final do array
				
				cb->write=0; //se indicar a posicao final volta a colocar se na primeira posicao
			
			}else{
			
				cb->write +=1; //se nao estiver incrementa-se esse mesmo indice
			
			}
		}
		
		wait(NULL); // colocar o processo pai em espera ate que o filho acabe para apagar a memoria partilhada
		
		fail_unmap(munmap(cb,size)); //unmapping of the memory
		fail_close(close(fd)); // closing of the memory
		fail_unlink(shm_unlink("/shm")); // unlinking of the memory
	
	}
	
	return 0;
}
