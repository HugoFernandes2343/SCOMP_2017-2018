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


typedef struct{
	int m[5][5];	
} mx; //estrutura que contem uma matrix de inteiros

typedef struct{
	mx m1;
	mx m2;
	int line;
	int res[5];
} multi; //estrutura que tem as duas estruturas mx em que cada tem uma das duas matrizes que seram multiplicadas, um int que indica a linha que será calculada pelo thread a que foi passada esta estrutura, array que indica os resultados dessa mesma linha da multipliccação

//simples metodo que coloca valores na matriz, vai colocar valores de 1 ate 25 sempre para facilitar testes
void* fill_matrix(void *arg){
	
	//utilizando o malloc guarda espaço na stack para uma estrutura mx 
	mx *matrix = (mx*)malloc(sizeof(mx)); 
	
	int num = 1;
	for (int i = 0; i < 5; i++){ //percorre linhas 
		for (int j = 0; j < 5; j++){ //percorre colunas
			matrix->m[i][j] = num; //introduz o num
			num++; //incrementa o num
		}
	}
	
	
	pthread_exit((void*)matrix);//exit
	
}

//metodo que recebe como arg uma estrutura multi e irá multiplicar as matrizes que estao contidas dentro desta estrutura
void* multiply(void *arg){ 

	multi dados = *((multi*)arg); //estrutura recebida
	
	multi* retur = (multi*)malloc(sizeof(multi)); //aloca espaco para uma estrutura multi e define o pointer para este espaco
	
	retur->m1 = dados.m1; //coloca no espaco alocado para retur a m1 presente em dados
	retur->m2 = dados.m2; //coloca no espaco alocado para retur a m2 presente em dados
	retur->line = dados.line; //coloca no espaco alocado para retur a line presente em dados
	
	//IMPORTANTE:Este for limpa o array presente em retur para ter 0 em todas as posicoes de modo a não dar origem a erros de calculos posteriores... isto foi necessario porque os resultados estavam a ser imprimidos errados porque apartir da primeira thread e mesmo fazendo free á posicao na memoria, os valores iam se somando de thread para thread
	for(int i = 0; i<5 ;i++){	
		retur->res[i] = 0;
	}
	
	//ciclo de contas em que resolve a multiplicação para alinha pedida do thread que corre este metodo
	for (int i = 0; i < 5; i++){
		printf("devia ser 0 %d\n",retur->res[i]);
		
		for (int j = 0; j < 5; j++){
			
			int multiplicador = dados.m1.m[dados.line][j];// numero da primeira matriz que multiplica pelo numero da segunda, este numero da primeira matriz pertence sempre á line cujos 
			printf("multiplicador %d\n",multiplicador);
			int num=dados.m2.m[j][i]; //numero da segunda matriz que ira multipplicar pelo da primeira
			// á medida que J é iterado o numero da m1 dentro da mm linha muda de coluna e o m2 dentro  da mesma coluna muda de linha
			printf("segunda matriz num %d \n",num);
			retur->res[i] += (num*multiplicador);//coloca o resultado no array de resultados alocado na memoria
		}
		printf("resultado desta pos %d \n",retur->res[i]);
	}
	
	pthread_exit((void*)retur);//exit
}

int main(){
	
	void* ret;//retorno da funcao fill_matrix
	
	mx matrix[2]; //array de mx's que serao as iniciais, e vao ser postas dentro de uma struct multi
	
	mx resultado; //mx resultado que contem a matriz de resultados
	
	multi dados; //estrutura para ter as duas matrizes a linha a ser multiplicada e os resultados dessa linha
	
	pthread_t threads[7];//threads
	
	/**
	 * inicia threads iniciais para encherem as matrizes
	 **/
	 pthread_create(&threads[0], NULL, fill_matrix, NULL);
	
	 pthread_create(&threads[1], NULL, fill_matrix, NULL);
	
	/**
	 * join dos threads iniciais de encher as matrizes
	 **/
	 pthread_join(threads[0],(void*)&ret);
	 matrix[0] = *((mx*)ret);
	 free(ret);
	 
	 pthread_join(threads[1],(void*)&ret);
	 matrix[1] = *((mx*)ret);
	 free(ret);
	 
	//imprime a primeira matriz
	for (int i = 0; i < 5; i++){
		for (int j = 0; j < 5; j++){
			printf("%d ; ",matrix[0].m[i][j]);
		}
		printf("\n");
	}
	
	//imprime a segunda matriz
	for (int i = 0; i < 5; i++){
		for (int j = 0; j < 5; j++){
			printf("%d ; ",matrix[1].m[i][j]);
		}
		printf("\n");
	}
	
	//colocar as matrizes na estrutura multi
	dados.m1 = matrix[0];
	dados.m2 = matrix[1];
	//iniciar a line a 0 para a primeira vez que a funcao multiply é usada
	dados.line = 0;
	 
	 
	/**
	 * execcução threads para a multipliccação
	**/
	for(int i = 0; i <5; i++){

		pthread_create(&threads[i+2], NULL, multiply, (void*)&dados);//cria um thread que corre a funcao de multiplicar e recebe a estrutura multi
		
		void* retur;//void para o return da função multiply
		
		pthread_join(threads[i+2],(void*)&retur);//join
		multi pf = *((multi*)retur);//cast de retur para a variavel pf
		free(retur); //free á memoria
		
		//colocar o resultado na matriz
		for(int j = 0; j<5;j++){
			resultado.m[i][j] = pf.res[j];
		}
		dados.line +=1;
		
	}
	
	//print da matriz final
	for (int i = 0; i < 5; i++){
		for (int j = 0; j < 5; j++){
			printf("%d ; ",resultado.m[i][j]);
		}
		printf("\n");
	}
	
	return 0;
}
