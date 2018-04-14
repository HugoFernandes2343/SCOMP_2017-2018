#ifndef SEARCH_H
#define SEARCH_H
typedef struct{
		char path[30]; //caminho para o ficheiro correspondente a uma devida struct
		char word[30]; // palavra a ser procurada
		int appearances;	// numero de vezes que a palavra aparece
	} shared;

int search( char *word, char *file, shared *data, int proc_num);
#endif
