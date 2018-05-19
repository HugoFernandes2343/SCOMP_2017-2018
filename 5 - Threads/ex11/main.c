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
#include <errno.h>

#define ALUNOS 300
#define MAX_NOTA 100


//estrutura proposta no enunciado
typedef struct {
    int number;//numero de aluno
    int notaG1;
    int notaG2;
    int notaG3;
    int notaFinal;
} prova;

prova arrayProva[ALUNOS];//array de 300 provas

//mutex de thread
pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t criarAluno;//condicao que indica que se pode criar uma prova com os dados de um aluno
pthread_cond_t iniciarT3;	//condicao que indica que o thread T3 pode comecar a calcular notas finais, esta condicao é instanciada pelo T2 quando acabar a sua parte do array 
pthread_cond_t novaProva;	//condicao que indica a T2 e T3 que há uma nova prova para se calcular a nota final
pthread_cond_t notaPositiva; //condicao que indica que uma notaPositiva esta disponivel para ser contada
pthread_cond_t notaNegativa; //condicao que indica que uma notaNegativa esta disponivel para ser contada

int numNovasProvas;//numero de provas cujas notas finais ainda nao foram calculadas
int notaPos;//numero de notas positivas que ainda nao foram contadas para a variavel total
int notaNeg;//numero de notas negativas que ainda nao foram contadas para a variavel total
int flagT2Acabou;//flag que indica que T2 acabou

int proxAlunoASerCriado;//numero do aluno que sera criado aseguir
int totPos;//total de notas positivas
int totNeg;//total de notas negativas

////////////////FUNÇOES//////////////////

void *gerarProvas(void *arg) {//vai receber o numero do aluno e vai gerar aleatoriamente as notas 
    
	//numero do aluno recebido por arg
	int i = *((int *) arg);
    
   /* gerador de numeros random */
	time_t t;
	srand( time(&t));
	
	// bloqueia o acesso dos outros threads porque nós queremos que apenas uma execução atue de cada vez
	pthread_mutex_lock(&mux);

    while(i != proxAlunoASerCriado) {//enquando o aluno que esta a tentar ser criadao por uma execução nao for o proximo a ser criado ficará a espera de alterações á condição criar aluno
        pthread_cond_wait(&criarAluno, &mux);
    }

	//introdução dos dados na prova i
    arrayProva[i].number = i;
    arrayProva[i].notaG1 = 0 + (rand() % MAX_NOTA);
    arrayProva[i].notaG2 = 0 + (rand() % MAX_NOTA);
    arrayProva[i].notaG3 = 0 + (rand() % MAX_NOTA);

	//incremento do numero de provas prontas para serem passadas para os threads T2 e T3
    numNovasProvas++;
	
	//indica que há uma nova prova para ser calculada
    pthread_cond_signal(&novaProva);

	//incremento ao numero que o proximo aluno tem que ter
    proxAlunoASerCriado++;
	
	//desbloqueio da memoria 
    pthread_mutex_unlock(&mux);

	//envio de broadcast para todas as execucoes de criação de alunos a indicar que já se pode criar um novo aluno uma delas irá comecar a sua criação, nomeadamente a que tiver o numero igual ao numero do proxAlunoASerCriado
    pthread_cond_broadcast(&criarAluno);
	
	//exit
    pthread_exit((void*) NULL);
}

void *calcularNotas(void *arg) {
    int t = *((int *) arg);// T2 ou T3

	//ciclo que para t2 percorre Arrayprova de 0 a 149 e para t3 de 150 a 299 
    for(int i = t * 150; i < (t+1) * 150; i++) {
       pthread_mutex_lock(&mux);//lock á memoria

        if(t == 1 && flagT2Acabou == 0) {//se este thread for o T3 e o T2 ainda nao tiver acabado espera por permiçao
            pthread_cond_wait(&iniciarT3, &mux);
        }
        
        while(numNovasProvas <= 0) {//espera por provas para calcular
            pthread_cond_wait(&novaProva, &mux);
        }
		
		//indica que uma prova ja vai ser calculada
        numNovasProvas--;
		
        pthread_mutex_unlock(&mux);// desbloqueio da memoria

		//calculos de notaFinal
        arrayProva[i].notaFinal = (arrayProva[i].notaG1 + arrayProva[i].notaG2 + arrayProva[i].notaG3)/3;

		pthread_mutex_lock(&mux);//lock á memoria
		
		//verifica se a nota é positiva ou negativa 
        if(arrayProva[i].notaFinal < 50) {//se for negativa     
            notaNeg++;
            pthread_cond_signal(&notaNegativa);//indica que uma nota negativa está disponivel para ser contada

        } else {//se for positiva
            notaPos++;
            pthread_cond_signal(&notaPositiva);//indica que uma nota positiva está disponivel para ser contada

        }

        pthread_mutex_unlock(&mux); // desbloqueio da memoria
    }

    flagT2Acabou = 1; //indica que T2 acabou
	
	//manda T3 comecar
    pthread_cond_signal(&iniciarT3);

	//exit
    pthread_exit((void*) NULL);
}

void *notasPos(void *arg) {//conta as notas positivas
   
	//dados para timedwait
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;

    while(1) {//ciclo infinito
	
       pthread_mutex_lock(&mux);//lock á memoria

		//se nao houver notas positivas para contar imediatamente vai entrar neste if
        if(notaPos <= 0) {
			
            int timed = pthread_cond_timedwait(&notaPositiva, &mux, &ts);//espera por uma nota por uns segundos

	    	if (timed == ETIMEDOUT) {//se nao aparecer uma entra neste if que vai libertar a memoria e sair do while pq nao ha mais notas para contar

                pthread_mutex_unlock(&mux);// desbloqueio da memoria

	    		break;
	    	}
        }

		//diminui as notas que faltam contar
        notaPos--;

        pthread_mutex_unlock(&mux);// desbloqueio da memoria
		
		//conta uma nota
        totPos++;
    }
	
	//exit
	pthread_exit((void*) NULL);
}

void *notasNeg(void *arg) {

	//dados para timedwait
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;
	
    while(1) {//ciclo infinito
	
       pthread_mutex_lock(&mux);//lock á memoria

	   //se nao houver notas negativas para contar imediatamente vai entrar neste if
        if(notaNeg <= 0) {
			
			int timed = pthread_cond_timedwait(&notaNegativa, &mux, &ts);//espera por uma nota por uns segundos

			//se nao aparecer uma entra neste if que vai libertar a memoria e sair do while pq nao ha mais notas para contar
			if (timed == ETIMEDOUT){
				
                pthread_mutex_unlock(&mux);// desbloqueio da memoria
				
	    		break;
	    	}
        }

		//diminui as notas que faltam contar
        notaNeg--;

		// desbloqueio da memoria
        pthread_mutex_unlock(&mux);
		
		//conta uma nota
        totNeg++;
    }

	//exit
    pthread_exit((void*) NULL);
}


int main(void) {
	
	//threads	
    pthread_t T1;//thread de criação de provas
    pthread_t T2andT3[2];//threads para calculos de notas finais 
    pthread_t T4;//thread de contagem de notas positivas
    pthread_t T5;//thread de contagem de notas negativas
	
	//iniciar condições
    pthread_cond_init(&criarAluno, NULL);
    pthread_cond_init(&iniciarT3, NULL);
    pthread_cond_init(&novaProva, NULL);
    pthread_cond_init(&notaPositiva, NULL);
    pthread_cond_init(&notaNegativa, NULL);
	
	//array de 300 posicoes para passar inteiros ao thread
	int nums[ALUNOS];
	
    
    for(int i=0; i<ALUNOS; i++) {//ciclo que cria T1 para encher ArrayProva enviando como arg o numero de aluno
		
        nums[i] = i;//numero de aluno
		
		pthread_create(&T1, NULL, gerarProvas, (void *)&nums[i]);//criação de T1
    }

	//iniciação de variaveis globais
    totNeg = 0;
    totPos = 0;
    flagT2Acabou = 0;
        
	//variavel apra passar o numero do T aka 2 ou 3
    int t[2];
	
    for(int i=0; i<2; i++) {
		
		//T2 ou T3
        t[i] = i;
		pthread_create(&T2andT3[i], NULL, calcularNotas, (void *)&t[i]);
    }

	//T4
    pthread_create(&T4, NULL, notasPos, (void *)NULL);
    
	//T5
    pthread_create(&T5, NULL, notasNeg, (void *)NULL);

	//Inicio dos joins dos threads
    pthread_join(T1, NULL);
	printf("T1 acabou\n");
	
	 printf("Criação acabou\n");
	
    pthread_join(T2andT3[0], NULL);
	printf("T2 acabou\n");
    
	pthread_join(T2andT3[1], NULL);
	printf("T3 acabou\n");
	
    printf("Calculos acabou\n");

    pthread_join(T4, NULL);
	printf("T4 acabou\n");
	
    pthread_join(T5, NULL);
	printf("T5 acabou\n");

	//destruicao do mutex
    pthread_mutex_destroy(&mux);
	
	//destruicao das condições
    pthread_cond_destroy(&criarAluno);
    pthread_cond_destroy(&iniciarT3);
    pthread_cond_destroy(&novaProva);
    pthread_cond_destroy(&notaPositiva);
    pthread_cond_destroy(&notaNegativa);
	
	//calculo das percentagens
	double percentPos = ((double)totPos*100)/(double)ALUNOS;
	double percentNeg = ((double)totNeg*100)/(double)ALUNOS;
	
	//print dos totais
	printf("Numero de notas positivas: %d \n",totPos);
	printf("Numero de notas negativas: %d \n",totNeg);
	
	//print das percentagens
	printf("Percentagem de notas positivas %.2f%%\n", percentPos);
	printf("Percentagem de notas negativas %.2f%%\n", percentNeg);
	
    return 0;
}




































