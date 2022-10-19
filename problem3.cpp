// Ward's Unisex bathroom problem 
// edited by Dawson Butts 

//program 3 problem 3

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <unistd.h>
#include <semaphore.h>
#include <mutex>

using namespace std;

//semaphores and one mutex
sem_t malesS,femalesS, bathroom;
mutex multex;

//count, so we can number the processes
int femalen =1, malen=1;

//count for number in bathroom
int fc=0, fwc=0, mc=0, mwc=0;  //count for num in bathroom and waiting.

/*
* use the  multex to get the id number for male and female processes.
*/
 
void male() {
  int i,id,t;
  bool canuse = false;

  //geting an id number
  multex.lock() ;
    id = malen;
    malen++ ;
  multex.unlock();
  printf("male %d:  Starting \n",id);

  while (1==1)  {

   //work for a while
    t = (int) rand() % 10; 
    printf("male %d:  is working %d seconds\n",id,t);
    sleep (t);

    printf("male %d: needs to use the bathrooms\n",id);
    sem_wait (&bathroom); 
    if (fc == 0 && fwc == 0) {  //bathroom has no females, check if females are waiting CHANGED HERE
      mc++; 
      sem_post(&bathroom);  
    } else {
      mwc++;
      sem_post(&bathroom);
      printf("male %d: is line  \n",id);
      sem_wait(&malesS);//the line of males waiting
       
      //finally can use the bathroom, mark the count up.
      // note must do this outside of the bathroom, CR or deadlock will happen
      sem_wait (&bathroom); 
        mc++;
      sem_post(&bathroom);
    }


    //use bathroom
    t = (int) rand() % 10; 
    printf("male %d:  is using the bathroom for %d seconds\n",id,t);
    sleep (t);

    sem_wait(&bathroom);
      mc--;
      if (mc == 0 && fwc != 0) {    //CHANGED HERE
        printf("male %d:  signaling the line to start\n",id);
        for (i=0; i<fwc; i++) 
          sem_post(&femalesS);
        fwc =0;
      }
    sem_post(&bathroom);
    printf("male %d:  is done using the bathroom\n",id);
  } 
}

void female() {
  int i,t,id;

  multex.lock() ;
    id = femalen;
    femalen++ ;
  multex.unlock();
  printf("female %d:  Starting \n",id);

  //initial set to infinite for testing
  while (1==1) {

    t = (int) rand() % 10;
    printf("female %d: is working for %d seconds\n",id,t);
    sleep (t);

    printf("female %d: needs to use the bathrooms\n",id,t);
    sem_wait (&bathroom); 
    if (mc == 0 && mwc == 0) {  //empty     CHANGED HERE
      fc++;
      sem_post(&bathroom);
    }else {
      fwc++;
      sem_post(&bathroom);
      printf("female %d: is line  \n",id,t);
      sem_wait(&femalesS);//the line of females waiting

      //finally can use the bathroom, mark the count up.
      // note must do this outside of the bathroom, CR or deadlock will happen
      sem_wait(&bathroom); 
         fc++;
      sem_post(&bathroom);
    }


    //use bathroom
    t = (int) rand() % 10; 
    printf("female %d:  is using the bathroom for %d seconds\n",id,t);
    sleep (t);

    //done
    sem_wait(&bathroom);
      fc--;
      if (fc == 0 && mwc != 0) {    //CHANGED HERE
        printf("female %d:  signaling the line to start\n",id,t);
        for (i=0; i<mwc; i++)
          sem_post(&malesS);
        mwc =0;
      }
    sem_post(&bathroom);
    printf("female %d:  is done with the bathroom\n",id,t);
  } 
}

int main(int argc, char* argv[]) {
  int i;
  //defaults
  int num_m = 1; 
  int num_f = 1;

  if (argc ==3) {
    num_m = atoi( argv[1] );
    num_f = atoi( argv[2] );
  } 
  printf("m is %d, f is %d\n", num_m, num_f);

  thread males[num_m];
  thread females[num_f];

  srand((unsigned)time(0));

//init semaphore line to 0, since everyone must wait
  sem_init(&malesS,0,0);
//init semaphore line to 0, since everyone must wait
  sem_init(&femalesS,0,0);
//init semaphore bathroom to 1, so first person can go in,  
//we could have just as easy used a mutex instead of semaphore for the "bathroom"
  sem_init(&bathroom,0,1);

// create threads 
  for (i=0; i<num_m; i++) {
     males[i] = thread(male);
  }
  printf ("male threads created\n");

  for (i=0; i<num_f; i++) {
    females[i] = thread(female);
  }
  printf ("all threads created\n");


//wait for threads to finish 
  for(i=0;i<num_m;i++) 
    males[i].join();

  for(i=0;i<num_f;i++) 
    females[i].join();

  return 0;

}