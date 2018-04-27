#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(void) {

	int* sharedmem, i;
	int fd, size = sizeof(int);
	sem_t *sem, *sem2;

	if((sem = sem_open("sem", O_CREAT|O_EXCL, 0644, 1))==SEM_FAILED) {
		perror("Sem_open falhou");
		exit(1);
	}
	
	if((sem2 = sem_open("sem2", O_CREAT|O_EXCL, 0644, 0))==SEM_FAILED) {
		perror("Sem_open 2 falhou");
		exit(1);
	}
	
	fd = shm_open("/shm", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	ftruncate(fd, size);
	sharedmem = (int*) mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	*sharedmem = 1357;
	
	for(i = 0; i < 3; i++) {
		sem_wait(sem2);
		*sharedmem += 1;
	}
	
	int r = munmap(sharedmem, size);
	if(r == -1) {
		perror("Unmap falhado");
	}
	
	r = close(fd);
	if(r == -1) {
		perror("Close falhado");
	}
	
	r = shm_unlink("/shm");
	if(r == -1) {
		perror("Shm unlink falhou");
	}
	
	sem_unlink("sem");
	sem_unlink("sem2");
	
	return 0;
	
}
