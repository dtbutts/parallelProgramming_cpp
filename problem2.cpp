//Dawson Butts 
//program 3 problem 2

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <unistd.h>
#include <semaphore.h>
#include <mutex>

using namespace std;

//create semaphores and mutex for 
sem_t savGo, cookGo, enter;
mutex idMutex;

//count to keep the number of processes
int savageIDs = 1;

//count for number of servings, initalized later based on input 
int servings;
int refillNum;  //this is to save input M

//method for savage threads 
void eat(){
    int id, t;
    //first set up savage IDs
    idMutex.lock();
        id = savageIDs;
        savageIDs++;
    idMutex.unlock();
    printf("Created savage %d \n", id);

    //start loop for savage to eat
    while(1){
        //sleep for random amount of time each loop
        t = (int) rand() % 10;
        printf("Savage %d SLEEPING for %d seconds\n", id, t);
        sleep(t);

        //have savage get food
        sem_wait (&enter);
        if (servings == 0){
            sem_post(&cookGo);  //start the cook
            sem_wait(&savGo);   //wait for cook to finish
        }
        servings--;     //eat a serving
        printf("Savage %d ate a serving. Now there are %d servings\n", id, servings);
        sem_post(&enter);  
        
    }
}

//method for cook
void cook(){
    int id = 1;     //only 1 cook so id is just 1
    int t;
    printf("Created cook\n");

    //start loop for cook 
    while(1){

        //have cook wait for savages, then cook
        sem_wait(&cookGo);
        printf("Cook %d has been woken up\n", id);
        servings = refillNum;
        printf("Pot has been refilled back to %d\n", servings);
        
        //sleep for random amount of time each loop
        t = (int) rand() % 10;
        printf("Cook %d SLEEPING for %d seconds\n", id, t);
        sleep(t);

        sem_post(&savGo);
        //savagesWaiting = 0;
        

    }
}



int main(int argc, char* argv[]) {

    //default servings is 8, just in case no argument
    servings = 8;

    if (argc == 2){
        servings = atoi( argv[1] ); //read in the user's number of servings
    }
    refillNum = servings;   //save the input of servings for refill

    thread savageThreads[10];
    thread cookThread;

    //initialize semaphores
    sem_init(&savGo, 0, 0); 
    sem_init(&cookGo, 0, 0);    //don't start cook right away
    sem_init(&enter, 0, 1);    //let first person access servings

    //create threads, problem gives how many to make
    for(int i = 0; i < 10; i++){
        savageThreads[i] = thread(eat);
    }
    printf("Savage threads created\n");

    cookThread = thread(cook);

    //wait for threads to finish
    for(int i = 0; i < 10; i++){
        savageThreads[i].join();
    }
    cookThread.join();

    return 0;
}