#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>


#define SIZE 256

void convert(char* strTemp){
	int i = 0;
	while(strTemp[i] != '\0'){
			
			if(strTemp[i] >= 'A' && strTemp[i]<='Z'){
				strTemp[i] = strTemp[i] + 32;
			}else if(strTemp[i] >= 'a' && strTemp[i] <= 'z'){
				strTemp[i] = strTemp[i] - 32;
			}
				
			i++;
			
			}
	}
close(childRead[i][0]);
int main(){
	
	int up[2];
	int down[2];
	char message[SIZE];
	
	if(pipe(up)<0){
		 perror("Erro creating pipe \n");
		 exit(EXIT_FAILURE);
		 }
		 
	if(pipe(down)<0){
		 perror("Erro creating pipe \n");
		 exit(EXIT_FAILURE);
		 }
  
	
	pid_t p = fork();
  
	if( p < 0 ){
		 perror("Error at the fork \n");
		 exit(EXIT_FAILURE);
		 }
		 
	if( p == 0 ){
		
		close(up[0]);
		close(down[1]);
		
		printf("What is the message? \n");
		fgets(message,sizeof(message),stdin);
  
		printf("The inserted message was: \n %s \n",message);
		
		if(write(up[1],message,SIZE)<0){
			perror("Error writing\n");
			exit(EXIT_FAILURE);
			}
			
		if(read(down[0],message,SIZE)<0){
			perror("Error reading\n");
			exit(EXIT_FAILURE);
			}
		
		printf("The converted message is: %s \n ", message);
		exit(EXIT_SUCCESS);
			
	}else{
		
		close(up[1]);
		close(down[0]);
		
		char strTemp[SIZE];
		
		if(read(up[0],strTemp,SIZE)<0){
			perror("Error Reading \n");
			exit(EXIT_FAILURE);
			}
		
		convert(strTemp);
		
		if(write(down[1],strTemp,SIZE) < 0){
			perror("Error writing");
			exit(EXIT_FAILURE);
			}
			
		wait(NULL);
	}
		
	return 0;	
}
