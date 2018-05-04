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

#define PROC 400

void fail_sem(sem_t *sem){
	if(sem==SEM_FAILED){
			perror("sem_open falhou");
		exit(1);
	}
}


typedef struct{
	int nr_total;
	int nr_vip;
	int nr_special;
	int nr_normal;
} data;


int main(){
	
	data *th;
	int fd, data_size = sizeof(data);	
	
	int value;
	int priority;
	
	/**
	 * cria memorias partilhadas
	 **/
	/*Criar memoria*/
	fd = shm_open("/shmem", O_CREAT|O_EXCL|O_RDWR, S_IWUSR|S_IRUSR);
	/*Definir tamanho da memoria*/
	ftruncate (fd, data_size);
	/*mapear objeto de memoria partilhada*/
	th = (data*) mmap(NULL,data_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	
	
	th.nr_total = 0;
	th.nr_vip = 0;
	th.nr_special = 0;
	th.nr_normal = 0;

	/**
	 * Abre semaforos
	 **/
	sem_t *lugares, *full, *vip, *special, *normal, *mutex, *entry;
	lugares = sem_open("lugares", O_CREAT, 0644, 300);
	fail_sem(lugares);
	full = sem_open("full", O_CREAT, 0644, 0);				
	fail_sem(full);
	vip = sem_open("vip", O_CREAT, 0644, 0);		//semaforo da fila dos vip
	fail_sem(vip);
	special = sem_open("special", O_CREAT, 0644, 0);	//semaforo da fila dos special
	fail_sem(special);
	normal = sem_open("normal", O_CREAT, 0644, 0);		//semaforo da fila dos normal
	fail_sem(normal);
	mutex = sem_open("mutex", O_CREAT, 0644, 1);
	fail_sem(mutex);
	entry = sem_open("entry", O_CREAT, 0644, 0);
	fail_sem(entry);
	spectator = sem_open("spectator", O_CREAT, 0644, 0);
	fail_sem(spectator);
	
	
	
	for (int i = 0; i < num_proc; i++){
		if(i == 300){
			sem_post(full);
		}
		
		p = fork();
		fail_fork(p);
		
		if(p==0){
			
			srand(time(NULL));
			priority = (rand() % 3) + 1; 
			sem_wait(mutex);
			int value;
			sem_getvalue(full, &value);
			if(value ==0){
				sem_wait(lugares);
				sem_post(spectator);
				sem_wait(entry);
				printf("Entrei\n");
				
				
			}
			
			
			
			//verifica se a sala ta aberta quando estiver entra, e para isso durante uns momentos impede outro visitante de entrar
			sem_wait(sem_room);
			
			//faz post depois de entrar para que outros visitantes possam entrar
			sem_post(sem_room);
			
			printf("I'm in %d\n",getpid());
			
			//fica num estado de visitar
			sem_wait(sem_visiting);
			
			printf("I'm out %d\n",getpid());
			
			exit(EXIT_SUCCESS);

		
		}
	}
	
	
	
}

















