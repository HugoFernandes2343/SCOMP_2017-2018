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

#define SIZE 1000

int array[SIZE]; //array of balances
int lowest; //lowest value
int highest;//highest value
int average;//average value

//finds the lowest value and places it in the global variable
void *lowestBalance(){
	int smallestValueYet=948315; //giant number to make sure its replaced right away
	
	//goes through the array and keeps replacing the smallestValueYet when a smaller one is found
	for(int i=0; i<SIZE;i++){
		
		if(array[i] < smallestValueYet){
			
			smallestValueYet = array[i]; // replaces the value
		}
		
	}
	
	//placing in the global variable
	lowest = smallestValueYet;
	
	//exit
	pthread_exit((void*)NULL);
}

//finds the highest value and places it in the global variable
void *highestBalance(){
	int biggestValueYet=0; // set as 0 so it is replaced right away
	
	//goes through the array and keeps replacing the biggestValueYet when a bigger one is found
	for(int i=0; i<SIZE;i++){
		
		if(array[i] > biggestValueYet){
		
			biggestValueYet = array[i];// replaces the value

		}
	}
	
	//placing in the global variable
	highest = biggestValueYet;
	
	//exit
	pthread_exit((void*)NULL);
}

//finds the average and places in the global variable
void *averageBalance(){
	int sum = 0;
	
	//goes through the array and adds up all the values
	for(int i=0; i<SIZE;i++){
		sum += array[i];
	}
	
	//calculates and saves the average to the global variable
	average = sum/SIZE;
	
	//exit
	pthread_exit((void*)NULL);
}

int main(){
	
	//threads
	pthread_t threads[3];
	
	//place incremental values in the array, currently placing numbers from 1 to 1000 could be changed to be random numbers using srand
	for (int i = 0; i < SIZE; i++){
		array[i]=i+1;
	}
	
	//Creating and starting the 3 thread operations
	pthread_create(&threads[0], NULL, lowestBalance, NULL);
	pthread_create(&threads[1], NULL, highestBalance, NULL);
	pthread_create(&threads[2], NULL, averageBalance, NULL);
	
	//waiting for the end of the 3 threads
	for(int i=0;i<3;i++){
		pthread_join(threads[i], NULL);
	}
	
	//print of the global variables
	printf("O valor mais baixo é %d, o mais alto é %d e o medio é %d\n",lowest,highest,average);
	
	return 0;
}
