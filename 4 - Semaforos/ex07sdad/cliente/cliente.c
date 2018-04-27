#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>

int main(void) {

	int* sharedmem;
	int fd, i, j, bilhete, size = sizeof(int);
	sem_t *sem, *sem2;
	time_t t;
	pid_t p;

	if((sem = sem_open("sem", O_CREAT))==SEM_FAILED) {
		perror("Sem_open falhou");
		exit(1);
	}
	
	if((sem2 = sem_open("sem2", O_CREAT))==SEM_FAILED) {
		perror("Sem_open 2 falhou");
		exit(1);
	}
	
	fd = shm_open("/shm", O_RDWR, S_IRUSR|S_IWUSR);
	ftruncate(fd, size);
	sharedmem = (int *) mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
	
	srand((unsigned)time(&t));
	
	for(i = 0; i < 3; i++) {
		p = fork();
		
		if(p == 0) break;
	}
	
	if(p == 0) {
	
		for(i = 0; i < 3; i++) {
			sem_wait(sem);
		
			sleep(1 + rand() % 10);
		
			bilhete = *sharedmem;
		
			printf("Fiquei com o bilhete %d.\n", bilhete);
		
			sem_post(sem2);
			
			if(i < 2) {
				sem_post(sem);
			}
		
			exit(bilhete);
		}
	} else {
	
		for(j = 0; j < 3; j++) {
			wait(NULL);
		}
		
	}
	
	return 0;
	
}
