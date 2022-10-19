//Dawson Butts 
//program 3 problem 1

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <unistd.h>
#include <semaphore.h>
#include <mutex>

using namespace std;


/*
    PROBLEM IN CLASS SAYS 5 PHILOSOPHERS AND 5 FORKS, USING THAT MODEL
*/

//global variables 
int philosopherIDs = 0;

//declare mutex and semaphores
mutex idMutex;
sem_t forks[5];   


//thinking function, really just sleeping 
void thinking(int id){
    int t = (int) rand() % 10;
    printf("Philosopher %d is THINKING for %d seconds\n", id, t);
    sleep(t);
}

void eating(int id){
    int t = (int) rand() % 10;
    printf("Philosopher %d is EATING for %d seconds\n", id, t);
    sleep(t);
}

/*
    This function incorporates the thinking, eating, and fork checking 
*/
void runPhilosopher(){
    //assign philosipher id
    int id;
    idMutex.lock();
        id = philosopherIDs;
        philosopherIDs++;
        printf("Created philosopher %d \n", id);
    idMutex.unlock();

    //sleep for a second, just easier to read output for me
    sleep(2);

    //start while loop
    while(1){

        //think for a random amount of time
        thinking(id);
    
        /*
        CHECK THE FORKS
        find if ID is even or odd, this lets then pick up left or right.
        With this technique some forks are always open, so there can be 
        no deadlock.

        NOTE:  IDs are 0-4 and so we use them to access forks[] 0-4 
        */
        printf("Philosopher %d is HUNGRY\n", id);
        if((id % 2) == 0 ){     //then it is even, pick up right first
            sem_wait(&forks[id]);       //pick up right
            sem_wait(&forks[(id+1)%5]);  //pick up left
            eating(id);                 //eat for random amount of time
            sem_post(&forks[id]);       //release right
            sem_post(&forks[(id+1)%5]); //release left

        }
        else{   //then it is odd, pick up left first 
            sem_wait(&forks[(id+1)%5]);  //pick up left
            sem_wait(&forks[id]);       //pick up right
            eating(id);                 //eat for random amount of time
            sem_post(&forks[(id+1)%5]); //release left
            sem_post(&forks[id]);       //release right
        }
        printf("Philosopher %d is FINISHED eating\n", id);

    }


}


int main(){

    //declare threads 
    thread philosophers[5];

    //initialize semaphores to 1 since they start not in use
    sem_init(&forks[0], 0, 1);
    sem_init(&forks[1], 0, 1);
    sem_init(&forks[2], 0, 1);
    sem_init(&forks[3], 0, 1);
    sem_init(&forks[4], 0, 1);

    //start threads
    for(int i = 0; i < 5; i++){
        philosophers[i] = thread(runPhilosopher);
    }
    printf("All philosopher threads created\n");

    //wait for threads to finish
    for(int i = 0; i < 5; i++){
        philosophers[i].join();
    }

    return 0;
}