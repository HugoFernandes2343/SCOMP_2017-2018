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

typedef struct {
	char str[30]; //Tamanho do array
	int needed_s; //numero de s a menos, se for 1 é porque existe mais um c que s -1 mais um s que C
	int needed_c; //numero de c a menos, se for 1 é porque existe mais um s que c -1 mais um c que s
	int letters; //numero de letras totais
} shared;

int main(){
	
	sem_t *sem_write; // semaforo que define a permição de iniciar as verificações e subsquente escrita
	sem_t *sem_mutex; // semaforo que define a permicao de dita escrita
	
	shared *sh;
	
	// memory sharing variables
	int fd=0, size = sizeof(shared); 
	
	//memory sharing
	fd = shm_open("/shm", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	ftruncate(fd, size);
	sh = (shared*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	//open dos semaforos
	sem_write = sem_open("semaforo1", O_CREAT, 0644, 1);
	fail_sem(sem_write);
	
	sem_mutex = sem_open("semaforo2", O_CREAT, 0644, 1);
	fail_sem(sem_mutex);
	
	//iniciar variaveis da estrutura
	sh->needed_c = 0;
	sh->needed_s = 0;
	sh->letters = 0;
	
	//primeiro fork
	pid_t p;
	p=fork();
	fail_fork(p);
	
	if(p==0){// escreve s
		
		while(1){//ciclo while constante ate que o array fique cheio
			
			sem_wait(sem_write); //verifica se o processo pode fazer as verificacoes e o seu ciclo
			
			if(sh->letters == 30){ //verifica se o array ja esta cheio se ja estiver sai com sucesso
				exit(EXIT_SUCCESS);
			}
			
			if(sh->needed_s==0 || sh->needed_s == 1){ // verifica se é preciso um s ( == 1) ou se sequer se pode por um s ( ==0) se alguma destas condicooes for verdadeira escreve s
				
				sem_wait(sem_mutex); //espera por permição para escrever na memoria partilhada
				
				strcpy(&sh->str[sh->letters],"S"); //adiciona s ao array
				
				//altera as variaveis da memoria
				sh->needed_s--; //diminui a necessidade de um s
				sh->letters++; //aumenta o numero de letras 
				sh->needed_c++; //aumenta a necessidade de um c
				
				// para ver a inserção de cada linha descomentar a linha seguinte
			//	printf("Letras %d add %s \n",sh->letters,"S");
				
				sem_post(sem_mutex); //indica que parou de escrever na memoria partilhada 
			}
			
			
			sem_post(sem_write); //indica que uma rotação do ciclo acabou deixa outro ciclo ser iniciado
			
			sleep(0.2); //este sleep existe porque sem ele os resultados obtidos nos arrays muito raramente tinham variacao alguma com ele verificam se outputs corretos e variados
			
		}
		
	}
	
	//segundo fork
	p = fork();
	fail_fork(p);
	
	if(p==0){// escreve c
		
		while(1){//ciclo while constante ate que o array fique cheio
			
			sem_wait(sem_write);//verifica se o processo pode fazer as verificacoes e o seu ciclo
			
			
			if(sh->letters == 30){ //verifica se o array ja esta cheio se ja estiver sai com sucesso
				exit(EXIT_SUCCESS);
			}
			
			if(sh->needed_c ==0 || sh->needed_c == 1){ // verifica se é preciso um c ( == 1) ou se sequer se pode por um c ( ==0) se alguma destas condicooes for verdadeira escreve c
				
				sem_wait(sem_mutex); //espera por permição para escrever na memoria partilhada
				
				strcpy(&sh->str[sh->letters],"C"); //adiciona s ao array
				
				//altera as variaveis da memoria
				sh->needed_c--; //diminui a necessidade de um c
				sh->letters++; //aumenta o numero de letras 
				sh->needed_s++; //aumenta a necessidade de um s
			
				// para ver a inserção de cada linha descomentar a linha seguinte
			//	printf("Letras %d add %s\n",sh->letters,"C");
			
				sem_post(sem_mutex); //indica que parou de escrever na memoria partilhada 
			}
			
			
			sem_post(sem_write); //indica que uma rotação do ciclo acabou deixa outro ciclo ser iniciado
			sleep(0.2); //este sleep existe porque sem ele os resultados obtidos nos arrays muito raramente tinham variacao alguma com ele verificam se outputs corretos e variados
		}
		
	}
	
	wait(NULL); //espera que os processos acabem
	
	//Pai imprime a string
	sem_wait(sem_mutex);
	printf("A mensagem é %s /n ",sh->str);
	fflush(stdout);
	sem_post(sem_mutex);
	
	//terminating memory and semaphores
	fail_unmap(munmap(sh,size)); //unmapping of the memory
	fail_close(close(fd)); // closing of the memory
	fail_unlink(shm_unlink("/shm")); // unlinking of the memory
	fail_Sunlink(sem_unlink("semaforo1"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo2"));//Unlinking of the semaphore
}
