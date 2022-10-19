//Dawson Butts 
//program 3 problem 4

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <unistd.h>
#include <semaphore.h>
#include <mutex>
#include <queue>

using namespace std;

//create semaphores 
sem_t clientGo, helpDeskGo, techGo; 
sem_t indTechWait[5];

//using mutex for assigning IDs, simpler 
//all IDs run 0 - n
mutex clientIDs, techIDs;

//mutex for counting techs
mutex testForTechs;

//global variables
int clientIDnum = 0;
int techIDnum = 0;
queue<int> readyTechs;

//client process
void runClient(){
    int t, id;

    //set IDs
    clientIDs.lock();
        id = clientIDnum;
        clientIDnum++;
        printf("Created client %d \n", id);
    clientIDs.unlock();

    //start infinite loop
    while(1){
        //do something for random time
        t = (int) rand() % 10;
        printf("Client %d WORKING for %d seconds\n", id, t);
        sleep(t);

        //call help desk
        sem_post(&helpDeskGo);

        //wait for problem to be fixed
        sem_wait(&clientGo);

        //start working again till next problem
        //printf("Client %d's problem has been FIXED\n", id);
    }
}

//help desk process, said in Discord it didn't need to wait
void runHelpDesk(){
    //start infinite loop
    int id = 1;
    printf("Created help desk %d\n", id);

    while(1){
        //wait for problems from clients
        sem_wait(&helpDeskGo);

        //tell physical plant tech
        printf("Help Desk %d recieved client's problem\n", id);
        sem_post(&techGo);
    }
}

//physical plant tech process
void runPlant(){
    int id, t;

    //set IDs
    techIDs.lock();
        id = techIDnum;
        techIDnum++;
        printf("Created tech %d \n", id);
    techIDs.unlock();

    //start infinite loop
    while(1){
        //have coffee for random amount of time
        t = (int) rand() % 10;
        printf("Tech %d on BREAK for %d seconds\n", id, t);
        sleep(t);

        //ready up this tech
        testForTechs.lock();
            readyTechs.push(id);
            printf("Tech %d is ready to help\n", id);

            //see how many are ready
            if(readyTechs.size() == 3){
                //wait for problem 
                testForTechs.unlock();  //release lock so other techs can ready up
                sem_wait(&techGo);      //wait for problem signal
                testForTechs.lock();    //lock again to change readyTechs queue

                //signal waiting  techs to continue and pop them from queue
                for(int i = 0; i < 2; i++){
                    sem_post(&indTechWait[readyTechs.front()]); //free process waiting
                    readyTechs.pop();
                }

                //remove this tech from the queue
                readyTechs.pop();

                //fix the problem and tell the client
                printf("Problem has been FIXED\n");
                testForTechs.unlock();
                sem_post(&clientGo);  
                
            } else{
                // wait for others 
                testForTechs.unlock();
                sem_wait(&indTechWait[id]);

            }
    }
}



int main(){

    //create threads
    thread client[2];   //two client threads
    thread helpDesk;    //one help desk thread
    thread plant[5];    //five physical plant threads

    //initialize semaphores
    sem_init(&clientGo, 0, 0);      //have client stop on problem
    sem_init(&helpDeskGo, 0, 0);    //have help desk wait to start
    sem_init(&techGo, 0, 0);        //have tech wait to fix
    for(int i = 0; i < 5; i++){
        sem_init(&indTechWait[i], 0, 0); 
    }

    //start threads
    for(int i = 0; i < 2; i++){
        client[i] = thread(runClient);
    }

    helpDesk = thread(runHelpDesk);

    for(int i = 0; i < 5; i++){
        plant[i] = thread(runPlant);
    }

    //wait for threads to finish
    for(int i = 0; i < 2; i++){
        client[i].join();
    }

    helpDesk.join();

    for(int i = 0; i < 5; i++){
        plant[i].join();
    }

    return 0;
}