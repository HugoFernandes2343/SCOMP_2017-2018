#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

/* este programa cria um loop de processos em que cada processo cria um filho e um pipe com esse filho, de forma a enviar a esse mesmo filho o seu PID e PPID.
 * O filho vai imprimir esta informaçao.
 * O ultimo processo criado no loop ira apresentar a informacao do processo original.
 */

int main(){
	int i, status, error, fd[2];
	pid_t pid;
	char message[100], ler[100];

	pipe(fd);								
	dup2(fd[0],0);							/*pipe criado para o ultimo processo do loop comunicar com o processo original*/
	dup2(fd[1],1);
	close(fd[0]);
	close(fd[1]);

	for(i=1;i<5;i++){						/*para cada processo*/
		pipe(fd);							/*cria um pipe para comunicar com o seu filho*/
		pid = fork();						/*faz fork*/
		if(pid > 0){						/*se for o processo pai*/
			error = dup2(fd[1], 1);			/*altera o output para o pipe acabado de criar*/
		}else{								/*se for filho*/
			error = dup2(fd[0], 0);			/*altera input para o pipe*/
		}
		close(fd[0]);
		close(fd[1]);

		if(pid){							/*se for processo pai faz break*/
			break;
		}
	}

		/*prepara a mensagem para o seu filho com o proprio PID e o PId do seu processo pai*/
	sprintf(message,"This is process %d with PID %d and its Parrent PID%d\n", i, (int)getpid(),(int)getppid()); 
		/*escreve essa mensagem no pipe que existe entre ele e o seu filho*/
	write(1, message, strlen(message) + 1);
		/*le a mensagem que o seu pai criou*/
	read(0, ler, strlen(message));
	
		/*espera que o filho acabe*/
	wait(&status);
		/*escreve na consola a informacao do seu processo e a mensagem do seu pai*/
	fprintf(stderr, "Current process = %d, data =%s", (int)getpid(), ler);
	exit(0);
}
