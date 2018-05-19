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

#define TRIPTIME 2

typedef struct{ //A city has a number id and a letter that represents it
	int id;
	char city;
}city;

typedef struct{ //A train has the said trains serial number, the city in which it is/was in most recently, the city it has as a final destination, and the total trip time until the destination
	int train_number;
	city current_city;
	city destination_city;
	int trip_time;
}train;

typedef struct{ //A line is composed of two cities and a mutex to regulate that only one train is in a road at one time
	city c1;
	city c2;
	pthread_mutex_t mux;
}line;

typedef struct{ //A trip's data has the train and the lines of the rail system
	train tr;
	line lines[3];
}tripData;

void* trip(void* arg){
	
	//trip passed as arg
	tripData* trip = (tripData*)arg;
	
	//beginning
	printf("The train number %d is ready to begin its jorney\n", trip->tr.train_number);
	
	
	if(trip->tr.current_city.id == 2){//if the train is at city B the train will only go through one line because city B connects to all others, this caused the output to show the 3rd train go in and out before all the others even go threw the first time, this however is not an error its just because this case goes through less code
		
		line path;//line to go through to get to the destination
		
		for(int i=0; i<3; i++) {//for to find the correct line
			
			//since we know the train is in city 2 we just need to figure out the line that takes us to the destination city
			if (trip->tr.destination_city.id == trip->lines[i].c1.id) {//if the destination of this trips train is the city 1 of a line it means this is the correct path
			
			path = trip->lines[i];	//places this line as the path
			
			} 
			
		}
		
		pthread_mutex_lock(&path.mux);//tries to access the line in the path
		
		sleep(TRIPTIME); // trip simulation
		
		trip->tr.trip_time += TRIPTIME; //set of the triptime to the struct
		
		//print
		printf("The train %d, in line %c -- %c from %c going to %c and took %d seconds\n", trip->tr.train_number, path.c1.city, path.c2.city, trip->tr.current_city.city, trip->tr.destination_city.city,trip->tr.trip_time);
		
		
		pthread_mutex_unlock(&path.mux);// allows access to the line that was the path
		
	}else{//the train will neet to  go through two lines
		
		line path[2]; //lines to go from the start to the destination
		
		for(int i=0; i<3; i++) {//for to find the first line
			
			if (trip->tr.current_city.id == trip->lines[i].c1.id) {//if the current city of this trips train is the city 1 of a line it means this is the correct path
			
				path[0] = trip->lines[i]; //places this line as the path
			
				for(int j=0; j<3; j++) { //for to find the second line
				
					if(path[0].c2.id == trip->lines[j].c2.id && trip->lines[j].c1.id == trip->tr.destination_city.id) {//if the city that the train is at after the first line is the city 1 of the line at lines[i] then this line is the second part of the path
						
						path[1] = trip->lines[j];//places this line as the path
					}
				}	
			}
		}
		
		//runs two time one for each of the lines the train will traversse
		for(int i=0; i<2; i++) {
			
			pthread_mutex_lock(&path[i].mux);//tries to access the line in the path[i]
			
			sleep(TRIPTIME); // trip simulation
			
			trip->tr.trip_time += TRIPTIME; //set of the triptime to the struct
			
			//print
			printf("The train %d, in line %c -- %c from %c going to %c and took %d seconds\n", trip->tr.train_number, path[i].c1.city, path[i].c2.city, trip->tr.current_city.city, trip->tr.destination_city.city,trip->tr.trip_time);
		
			pthread_mutex_unlock(&path[i].mux);// allows access to the line that was the path[i]
		}
	}
	
	//if the train is at the destination print a message
	if(trip->tr.current_city.id == trip->tr.destination_city.id){
		printf("The train is at the destination");
		
	}
	
	//end
	printf("End of the road %d \n", trip->tr.train_number);
	
	//exit
	pthread_exit((void*)NULL);
}

int main(){
	
	//simulate city D- city C ; city A - city D ; city B - city A
	
	//threads
	pthread_t threads[3];
	
	//the 3 lines will be placed in this array with city B always as the second city since its the middle point
	line lines[3];
	
	//the array contaning the all contaning structures for the 3 trips we will simulate
	tripData trips[3];
	
	//the array that contains the 4 city stuctures
	city cities[4];
	
	//CITY A
	cities[0].id = 1;
	cities[0].city='A';
	
	//CITY B
	cities[1].id = 2;
	cities[1].city = 'B';
	
	//CITY C
	cities[2].id = 3;
	cities[2].city = 'C';
	
	//CITY D
	cities[3].id = 4;
	cities[3].city = 'D';
	
	//NOTE THAT CITY 2 IS ALWAYS THE c2
	//CITY A - CITY B
	lines[0].c1 = cities[0];
	lines[0].c2 = cities[1];
	pthread_mutex_init(&lines[0].mux, NULL);
	
	//CITY C - CITY B
	lines[1].c1 = cities[2];
	lines[1].c2 = cities[1];
	pthread_mutex_init(&lines[1].mux, NULL);
	
	//CITY D - CITY B
	lines[2].c1 = cities[3];
	lines[2].c2 = cities[1];
	pthread_mutex_init(&lines[2].mux, NULL);
	
	//array that contains the 3 trains
	train trains[3];
	
	//TRAIN FROM D TO C
	trains[0].train_number = 1;
	trains[0].current_city = cities[3];
	trains[0].destination_city = cities[2];
	trains[0].trip_time = 0;

	//TRAIN FROM A TO D
	trains[1].train_number = 2;
	trains[1].current_city = cities[0];
	trains[1].destination_city = cities[3];
	trains[1].trip_time = 0;
	
	//TRAIN FROM B TO A
	trains[2].train_number = 3;
	trains[2].current_city = cities[1];
	trains[2].destination_city= cities[0];
	trains[2].trip_time = 0;
	
	//Placing the lines into the trip informations trip 1
	trips[0].tr = trains[0];
	trips[0].lines[0] = lines[0];
	trips[0].lines[1] = lines[1];
	trips[0].lines[2] = lines[2];
	
	//Placing the lines into the trip informations trip 2
	trips[1].tr = trains[1];
	trips[1].lines[0] = lines[0];
	trips[1].lines[1] = lines[1];
	trips[1].lines[2] = lines[2];
	
	//Placing the lines into the trip informations trip 3
	trips[2].tr = trains[2];
	trips[2].lines[0] = lines[0];
	trips[2].lines[1] = lines[1];
	trips[2].lines[2] = lines[2];

	//creation of the threads 
	pthread_create(&threads[0], NULL, trip, (void*)&trips[0]);
	pthread_create(&threads[1], NULL, trip, (void*)&trips[1]);
	pthread_create(&threads[2], NULL, trip, (void*)&trips[2]);
	
	//join of the threads 
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	pthread_join(threads[2], NULL);
	
	//end
	printf("All Threads are over\n");
	
	//destroy mutex's
	pthread_mutex_destroy(&lines[0].mux);
	pthread_mutex_destroy(&lines[1].mux);
	pthread_mutex_destroy(&lines[2].mux);
	
	return 0;
	
}
