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

//documento que mostra o raciocinio: algoritmo.txt
//problema, se reduzir o sleep para menos de 1 segundo nao funciona tentamos resolver mas nao conseguimos por a funcionar com um sleep niferior a 1 segundo


void fail_sem(sem_t *sem){
	if(sem==SEM_FAILED){
			perror("sem_open falhou");
		exit(1);
	}
}


void fail_fork(int p){
	if(p == -1) {
		perror("Fork falhou\n");
		exit(EXIT_FAILURE);
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

typedef struct {
	int numeroPessoas;
	int lugaresDisponiveis;
} shared;

int main(){
	
	// shared int with the other process that saves the number of people
	shared *data;
	
	//shared memory variables
	int fd, size = sizeof(shared);
	
	//semaforos
	sem_t *sem_entrada; //diz que um cliente entrou	
	sem_t *sem_saida; //diz que um cliente pode sair
	sem_t *sem_mutex; //variavel de acesso a memoria
	sem_t *sem_portas; //diz quantas portas estao livres
	
	//forks
	pid_t p;
	
	//memory sharing
	fd = shm_open("/shm", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	ftruncate(fd, size);
	data = (shared*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	data->numeroPessoas = 0;
	data->lugaresDisponiveis = 200;
	
	//open dos semaforos
	sem_entrada = sem_open("semaforo1", O_CREAT, 0644, 0);
	fail_sem(sem_entrada);
	
	sem_portas = sem_open("semaforo2", O_CREAT, 0644, 3);
	fail_sem(sem_portas);
	
	sem_mutex = sem_open("semaforo3", O_CREAT, 0644, 1);
	fail_sem(sem_mutex);
	
	sem_saida = sem_open("semaforo4", O_CREAT, 0644, 0);
	fail_sem(sem_saida);
	
	p=fork(); //primeiro fork, um processo vai criar filhos que serao clentes o outro vai gerir o comboio
	fail_fork(p);
	
	if(p==0){//processo  que vai ter filhos que serao clientes
		
		//ciclo para criar filhos que simulam os clientes
		for(int i=0;i<300;i++){
			
			p=fork();
			fail_fork(p);
			
			if(p==0){//clientes
				
				while(1){//ciclo infinito ate sair do comboio
					
					sem_wait(sem_mutex); //espera para poder aceder a memoria
							
					//se nao houver lugares da post a memoria e volta a executar o ciclo
					if(data->lugaresDisponiveis==0){
						sem_post(sem_mutex);
					}else{ //se houver lugares entra

					//se nao ouver portas livres esperar ate existir portas livres
					sem_wait(sem_portas);
					
					data->lugaresDisponiveis--; // reducao aos lugares disponiveis
					
					//entra
					printf("Entrei sou o processo %d \n",getpid());
					fflush(stdout);
					
					
					sem_post(sem_portas); //liberta a porta por onde entrou
					
					sem_post(sem_mutex); //liberta a memoria
					
					sem_post(sem_entrada);	//diz ao comboio que entrou
				
					///////////////////

					sem_wait(sem_saida); //espera para poder sair
					
					sem_wait(sem_portas); //usa uma porta
					
					//saida
					printf("Saí sou o processo %d \n",getpid());
					fflush(stdout);
					
					
					sem_wait(sem_mutex); //espera para poder mexer na memoria
					
					data->lugaresDisponiveis++; //aumenta o numero de lugares disponiveeis
					data->numeroPessoas++;//conta como uma pessoa que ja passou pelo comboio
					
					sem_post(sem_mutex); //liberta a memoria
					
					sem_post(sem_portas); // liberta uma porta
					
					exit(EXIT_SUCCESS);
					
					}
				}
			}
		}
		
		exit(EXIT_SUCCESS);
	}
	
	//comboio
	do{
		
	sem_wait(sem_entrada); //espera pela entrada de um cliente
	
	sleep(1);//tempo no comboio
	
	sem_post(sem_saida); //indica a saida do comboio
	
	}while(data->numeroPessoas <300);
	
	for (int i = 0; i < 300; i++)
	{
		wait(NULL);
	}
	
	//terminating memory and semaphores
	fail_unmap(munmap(data,size)); //unmapping of the memory
	fail_close(close(fd)); // closing of the memory
	fail_unlink(shm_unlink("/shm")); // unlinking of the memory
	fail_Sunlink(sem_unlink("semaforo1"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo2"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo3"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo4"));//Unlinking of the semaphore

	return 0;
}
