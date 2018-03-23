#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "struct.h"

#define CHILDS 5

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
	
	int writeAllChilds[2];
	int childRead [5][2];
	
	fail_pipe(pipe(writeAllChilds));
	int i;
	for (i = 0; i<5; i++){
		int temp[2];
		fail_pipe(pipe(temp));
		childRead[i][0] = temp[0];
		childRead[i][1] = temp[1];
	}
	
	product a;
	a.product_code = 1;
	strcpy(a.product_name, "Produto A");
	a.price = 10;
	
	product b;
	b.product_code = 2;
	strcpy(b.product_name, "Produto B");
	b.price = 20;
	
	product c;
	c.product_code = 3;
	strcpy(c.product_name, "Produto C");
	c.price = 30;
	
	product list[3];
	list[0] = a;
	list[1] = b;
	list[2] = c;
	
	pid_t p;
	for(i = 0; i<5; i++){
		p = fork();
		fail_fork(p);
		
		if(p == 0){
			int j;
			close(writeAllChilds[0]);
			for(j = 0; j < 5; j++){
				close(childRead[j][1]);
				if(j != i){
					close(childRead[j][0]);
				}
			}
			
			query q;
			q.child = i;
			q.code = (i % 3)+1;
			
			if(write(writeAllChilds[1], &q, sizeof(q)) < 0){
				perror("Error writing query");	
			}
			
			product prod;
			
			if(read(childRead[i][0], &prod, sizeof(prod)) <0){
					perror("Error reading product");
			}
			
			printf("\n-------\nProduto: %s, preço: %i, child: %i\n-----------\n", prod.product_name, prod.price, i);
			close(childRead[i][0]);
			close(writeAllChilds[1]);
			exit(0);
		}
	}
	
	int j;
	close(writeAllChilds[1]);
	for(j = 0; j < 5; j++){
		close(childRead[j][0]);
	}
	
	for(j = 0; j<5; j++){
		query quer;
		if(read(writeAllChilds[0], &quer, sizeof(quer)) <0){
				perror("Error reading query");
		}
		
		product prod;
		int k;
		for(k =0; k<3; k++){
			if(list[k].product_code == quer.code){
				prod = list[k];
			}
		}
		
		if(write(childRead[quer.child][1], &prod, sizeof(prod)) < 0){
			perror("Error writing query");	
		}
		
	}
	
	close(writeAllChilds[0]);
	for(j = 0; j < 5; j++){
		close(childRead[j][1]);
	}
	
	wait(0);
		
			
}

