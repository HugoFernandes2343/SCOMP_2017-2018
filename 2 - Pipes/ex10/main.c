#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

int main(){
	
	time_t t;	  
	
	int fd_a[2];
	int fd_b[2];
	fail_pipe(pipe(fd_a));
	fail_pipe(pipe(fd_b));
	pid_t p = fork();
	fail_fork(p);
	

	if(p==0){
		sleep(1);
		srand ((unsigned) time (&t));
		close(fd_a[1]);
		close(fd_b[0]);
		int bet;
		if(read(fd_a[0], &bet, sizeof(bet)) <0){
				perror("F- Error reading bet");	
		}
		while(bet > 0){
			int guess = rand()%5;
			if(write(fd_b[1], &guess, sizeof(guess)) < 0){
				perror("F- Error writing guess");	
			}
		
			int res;
			if(read(fd_a[0], &res, sizeof(res)) <0){
					perror("F- Error reading result");
			}
			printf("Result: %i\n", res);
			
			if(read(fd_a[0], &bet, sizeof(bet)) <0){
				perror("F- Error reading bet");	
			}
		}
		close(fd_a[0]);
		close(fd_b[1]);
		exit(0);
	}
	
	close(fd_a[0]);
	close(fd_b[1]);
	srand ((unsigned) time (&t));
	int balance = 20;
	printf("Starting balance: %i\n", balance);
	int bet = 1;
	
	while (balance>0){
		if(write(fd_a[1], &bet, sizeof(bet)) < 0){
				perror("P- Error writing bet");	
		}
		
		int value = rand()%5;
		int guess;
		if( read(fd_b[0], &guess, sizeof(guess)) < 0){
			perror("P- Error reading guess");
		}
		if(value == guess){
			balance += 10;
			if(write(fd_a[1], &balance, sizeof(balance)) < 0){
				perror("P- Error writing balance");
			}
		} else {
			balance -= 5;
			if(write(fd_a[1], &balance, sizeof(balance)) < 0){
				perror("P- Error writing balance");
			}
		}
	}
	
	bet = 0;
	if(write(fd_a[1], &bet, sizeof(bet)) < 0){
			perror("P- Error writing bet");
	}
	
	wait(0);
	close(fd_b[0]);
	close(fd_a[1]);
	
}


