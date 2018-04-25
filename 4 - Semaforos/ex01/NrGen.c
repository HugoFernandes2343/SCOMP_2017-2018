#include <stdio.h>

#define NR 1600


/**
 * Verifica se o ficheiro a abrir existe
 **/
void checkFile(FILE *f){
		if( f == NULL ){
			perror("File does not exist teste");
		}
}

int main(){
	
	FILE *out;
	out = fopen("Numbers.txt", "w");
	checkFile(out);
	
	for(int i = 1; i <= NR; i++){
		fprintf (out, "%i\n", i);
	}
	
	fclose(out);
	return 0;
}
