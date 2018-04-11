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

// Não, os valores não serão constantes porque dependeram da ordem a que cada processo é executado, visto que ambos
// estão a efetuar alterações na mesma memória simultaneamente é impossivel garantir estabilidade nos resultados.

int main(void){
	
	int i,fd,num=1000;
	int size = sizeof(int);
	int* sh;
	
	//Criacao da memoria partilhada
	fd = shm_open("/shm", O_CREAT|O_EXCL|O_RDWR, S_IWUSR|S_IRUSR);
	ftruncate(fd, size);
	sh = (int*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	*sh=num;
	
	//fork
	pid_t p;
	p=fork();
	fail_fork(p);
	
	if(p==0){//processo filho
		
		for (i = 0; i < 1000000; i++){
			*sh += 2; //acrescimo
			*sh -= 1; //redução
		}
		printf("Resultado do filho: %d\n",*sh);	
		
	}else{//processo pai
		for (i = 0; i < 1000000; i++){
			*sh += 2; //acrescimo
			*sh -= 1; //redução
		}
		printf("Resultado do pai: %d\n",*sh);	
		
		//espera pelo filho
		wait(NULL);
		
		//fecha e remove a memoria partilhada
		fail_unmap(munmap(sh,size));
		fail_close(close(fd));
		fail_unlink(shm_unlink("/shm"));
		
	}
	
	return 0;
}
