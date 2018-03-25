#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include "user.h"

#define SIZE 50

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
	
	char name[SIZE];
	char pass[SIZE];
	
	int exitValue = 0,i;
	
	user u1,u2,u3,u4,u5;
	strcpy(u1.id , "Norberto");
	strcpy(u1.password, "Sousa");
	strcpy(u2.id, "Hugo");
	strcpy(u2.password, "Fernandes");
	strcpy(u3.id, "David");
	strcpy(u3.password, "Maia");
	strcpy(u4.id, "Marco");
	strcpy(u4.password, "Carneiro");
	strcpy(u5.id,"Filipe");
	strcpy(u5.password ,"Simao");
	
	user users[5];
	users[0] = u1;
	users[1] = u2;
	users[2] = u3;
	users[3] = u4;
	users[4] = u5;
	
	if((read(STDIN_FILENO,name,SIZE))<0){
			perror("Error with read");
			exit(EXIT_FAILURE);	
		}
	if((read(STDIN_FILENO,pass,SIZE))<0){
			perror("Error with read");
			exit(EXIT_FAILURE);	
		}
	
	for (i = 0; i < 5; i++){
		if(strcmp(users[i].id,name)!=0){
			exitValue = 2;
			return exitValue;
		}
		if(strcmp(users[i].id,name)==0 && strcmp(users[i].password,pass)!=0){
			exitValue = 0;
			return exitValue;
		}
		if(strcmp(users[i].id,name)==0 && strcmp(users[i].password,pass)==0){
			exitValue = 1;
			return exitValue;
		}
	}
	return 0;
	
}
