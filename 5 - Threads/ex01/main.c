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
#include <pthread.h>

void* thread_func(void *arg){
	printf("Sou a thread %u e estou a executar\n", pthread_self());
	pthread_exit((void*)NULL);
}



int main() {
	
	pid_t pid = fork();

	if(pid == 0) {
		fork();
		pthread_t thread_id;
		pthread_create(&thread_id, NULL, thread_func, NULL);
		pthread_join(thread_id, NULL);
	}
	fork();
}
