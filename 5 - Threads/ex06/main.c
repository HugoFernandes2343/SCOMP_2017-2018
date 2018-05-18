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

//prints all the prime numbrs until the limit given
void* printPrime(void* arg){
		
	int maxNumber = *((int*)arg); //places the agr in the limit number
	
	printf("2\n");//since asking for a real number smaller than 2 is nonsensical, we ask for a number biggar than the number 2 as a result we know that the number 2 will always have to be printed
	
	//this for goes through each number from 3 to the max number
	for (int i =3; i <= maxNumber; i += 2){
		
		int nonPrimeflag = 0;//if the number is not prime the flag is 1
		
		//this for provides the divisor j
        for (int j= 2; j < i; j++){
	 
	        if (i%j == 0){//if the division of the number i by the divisor j has no remainder it means the number is not prime the divisor goes from 2 till i-1 each iterration
				
	            nonPrimeflag = 1;
	            break;
	    
	           }
		}
	    
		if (nonPrimeflag == 0){//if the number is prime print it
			
	        printf ("%d \n", i);
	    }
	}
	
	//exit
	pthread_exit((void*)NULL);
}

int main(){
	
	int maxNumber = 0;//variable in which to place the inputed number
	
	//thread
	pthread_t thread;
	
	//asking for the number
	printf("Enter a positive number equal to or higher than 2, this number will act as a limit, all the prime numbers smaler than it will be printed\n");
	scanf("%d",&maxNumber);
	
	//create the thread passing the number as arg
	pthread_create(&thread, NULL, printPrime, (void*)&maxNumber);
	
	//join
	pthread_join(thread,NULL);
	
	return 0;
}
