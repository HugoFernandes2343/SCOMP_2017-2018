#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "search.h"


int search( char *word,char *file, shared *data, int proc_num){
	
	FILE *fp; //objecto file de onde seram lidas as expressoes
	
	
	int findings = 0; //numero de vezes que uma determinada leitura ja encontrou a palavra no ficheiro
	char str[100]; //string para colocar os dados lidos do ficheiro
	char *token; // retorno da funcao strtok que divide str em segmentos ou seja em tokens
	const char del[2] = " "; //utilizado pela funcao strtok para saber por onde dividir str neste caso divide por espacos

	//abertura do ficheiro
	if((fp = fopen(file, "r")) == NULL) {
		printf("Falhou abrir ficheiro\n");
		return(-1);
	}

	
	while(fgets(str, 100, fp) != NULL) {//ciclo para percorrer o ficheiro
		
		token = strtok(str, del);//divide a string em tokens, o pointer token aponta para o primeiro desses tokens
		
		while(token != NULL) { //ciclo para percorrer todos os tokens ate ao fim do ficheiro
			
			if((strcmp(token, word)) == 0) {//condicao que verifica se o token atual é a palavra que queremos contar
				findings++;
			}
			
			token = strtok(NULL, del); //instrução que passa para o proximo token
		
		}
	}
	
	//Impressao de resultados
	if(findings == 0) {
		printf("o filho numero %d não deu com a palavra\n", proc_num);
	}
	
	data->appearances = findings;//Colocação do numero de vezes que a palavra foi encontrada na struct
	
	//fecho do ficheiro
	if(fp) {
		fclose(fp);
	}
	
   	return(0);
}
