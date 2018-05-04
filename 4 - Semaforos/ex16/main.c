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

int main(){
	
	sem_t *sem_W_E; //semaforo com o numero atual de carros a entrar/tentar entrar no sentido W -> E
	sem_t *sem_E_W;  //semaforo com o numero atual de carros a entrar/tentar entrar no sentido E -> W
	sem_t *sem_use1; //semaforo que serve como mecanismo de permição para o inicio de marcha de um carro no sentido W->E
	sem_t *sem_use2; //semaforo que serve como mecanismo de permição para o inicio de marcha de um carro no sentido W->E
	sem_t *sem_end; //semaforo que um carro incrementa quando acaba a passagem para que o pai possa esperar que todos os filhos acabem a marcha de um lado antes dde ativar o outro lado

	int carros[20] = {1,1,2,2,1,2,2,2,2,1,1,2,1,2,1,2,1,2,1,2}; //Array com flags para simular 20 carros, 1 significa que o carro vai para W->E 2 vai para E->W
	
	//open dos semaforos
	sem_W_E = sem_open("semaforo1", O_CREAT, 0644, 0);
	fail_sem(sem_W_E);
	
	sem_E_W = sem_open("semaforo2", O_CREAT, 0644, 0);
	fail_sem(sem_E_W);
	
	sem_use1 = sem_open("semaforo3", O_CREAT, 0644, 0);
	fail_sem(sem_use1);
	
	sem_use2 = sem_open("semaforo4", O_CREAT, 0644, 0);
	fail_sem(sem_use2);
	
	sem_end = sem_open("semaforo5", O_CREAT, 0644, 0);
	fail_sem(sem_end);
	
	pid_t p;
	
	//forks
	for (int i = 0; i < 20; i++){ //criacao de filhos que serao cada um  um carro simulado baseado na sua posicao do array
		
		p = fork();
		fail_fork(p);
		
		if(p==0){ //filho
			if(carros[i]==1){ //se o valor na posicao do array equivalente ao numero do filho for 1 entao vai iniciar marcha W->E
				
				sem_post(sem_W_E); //tenta comecar a percorrer neste sentido
				
				sem_wait(sem_use1); //espera por autorização para poder andar pela estrada neste sentido
				
				sleep(i); //sleep para todos os carros demorarem uma quantidadede tempo diferente entre si a entrarem para testar o timing de saida
				
				printf("Entrei na estrada de W para E sou o carro numero %d processo numero %d \n",i,getpid()); //entrada
				
				sleep(30);//travesia
				
				printf("Sai da estrada de W para E sou o %d processo numero %d\n",i,getpid()); //saida
				
				sem_post(sem_end); //acaba a travesia, diz ao pai que um veiculo acabou de passar
			}
			
			if(carros[i]==2){ //se o valor na posicao do array equivalente ao numero do filho for 2 entao vai iniciar marcha E->W
				
				sem_post(sem_E_W);//tenta comecar a percorrer neste sentido
				
				sem_wait(sem_use2); //espera por autorização para poder andar pela estrada neste sentido
				
				sleep(i); //sleep para todos os carros demorarem uma quantidadede tempo diferente entre si a entrarem para testar o timing de saida
				
				printf("Entrei na estrada de E para W sou o carro numero %d processo numero %d \n",i,getpid()); //entrada
				
				sleep(30); //travesia
				
				printf("Sai da estrada de E para W sou o %d processo numero %d\n",i,getpid()); //saida
				
				sem_post(sem_end); //acaba a travesia, diz ao pai que um veiculo acabou de passar
			}
			exit(EXIT_SUCCESS); //fim do filho
		}
	}
	
	//pai
	for (int i = 0; i < 20; i++){
		int value1; //valor no semaforo sem_W_E
		sem_getvalue(sem_W_E,&value1);
		
		int value2; //valor no semaforo sem_E_W
		sem_getvalue(sem_E_W,&value2);

		if(value1 > 0){ //se um carro estiver a tentar entrar na ponte por este lado
			
			int count1 = 0; //numero de carros que passam na estrada nesta "ronda"
			
			while(value1>0){ //enquanto estiver algum carro a tentar entrar nesta direcao
			
				sem_wait(sem_W_E); //espera que um carro tente entrar neste lado
				
				sem_post(sem_use1); //autoriza a entrada
				
				sem_getvalue(sem_W_E,&value1); //atualiza value1
				
				count1++; //aumenta o numerod de carros que passou
			}
			
			for (i = 0; i < count1; i++)//ciclo que espera que todos os carros que já a comecaram acabem a travesia
			{
				sem_wait(sem_end); //espera qeu um carro acabe a travessia
			}
			
		}else if(value2>0){ //se um carro estiver a tentar entrar na ponte por este lado
			
			int count2 = 0; //numero de carros que passam na estrada nesta "ronda"
			
			while(value2>0){ //enquanto estiver algum carro a tentar entrar nesta direcao
				
				sem_wait(sem_E_W); //espera que um carro tente entrar neste lado
				
				sem_post(sem_use2);//autoriza a entrada
				
				sem_getvalue(sem_E_W,&value2); //atualiza value2
				
				count2++; //aumenta o numerod de carros que passou
			}
			
			for (i = 0; i < count2; i++) //ciclo que espera que todos os carros que já a comecaram acabem a travesia
			{
				sem_wait(sem_end); //espera qeu um carro acabe a travessia
			}
		}
		
		
		
	}
	
	//wait pelos filhos
	for (int i = 0; i < 20; i++){
		wait(NULL);
	}
	
	printf("End.\n");//termino do programa
	
	//unlink
	fail_Sunlink(sem_unlink("semaforo1"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo2"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo3"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo4"));//Unlinking of the semaphore
	fail_Sunlink(sem_unlink("semaforo5"));//Unlinking of the semaphore
	
	return 0;
}
