// FORTOV EGOR, BPI214                        //
// Variant 20                                 //
// for each seller we have a separate process //
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int *store;  // *store: 0 => go to 1-st store; 1 => go to 2-nd store

void handler(int sig)
{
   printf("Hehe terminated: %d\n", sig);
}


int main() {
   signal(SIGINT, handler);
   sem_t *sem1, *sem2;
   const char* NAME_1 = "sem1", *NAME_2 = "sem2";
   sem1 = sem_open(NAME_1, O_CREAT | O_EXCL, 0666 , 0);  // for the 1-st seller
   sem2 = sem_open(NAME_2, O_CREAT | O_EXCL, 0666 , 0);  // for the 2-nd seller

   /* each customer has to go through 3 store sections, i.e.:
     1-st store -> 2-nd store -> 1-st store (or vice-versa)
   */
   // in order to optimize the work of sellers, they will be performed in parallel

   store = mmap(NULL, sizeof *store, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
   *store = 0;  // at the beginning customers goes to the 1st store

   pid_t pid1, pid2, pid3;
   pid1 = fork();
   if (pid1 == 0) {  // customers' process (mutual for all of them)
       /*
       for (int i = 0; i < n; i++) {
       fork();
       }
       */
       pid3 = fork();  // for scaling just make one more fork in else statement!
       if (pid3 == 0) {  // 1-st client
           printf("1st seller is busy!\n");
           /* 1st seller is busy with the 1-st customer during 2 secs
             (during this time 1-st seller's process is blocked)    */
           sleep(2);
           *store = 1;  // now go to the 2-nd store
           sem_post(sem1);  // free 1-st seller
           printf("2nd seller is busy!\n");
           /* 2nd seller is busy with the 1-st customer during 2 secs
             (during this time 2-st seller's process is blocked)    */
           sleep(2);
           *store = 0;  // now go back to the 1-st store
           sem_post(sem2);  // free 2-nd seller
           printf("1st seller is busy!\n");
           /* AGAIN: 1st seller is busy with the 1-st customer during 2 secs
             (during this time 2-st seller's process is blocked)    */
           sleep(2);
           *store = 1;  // finito
           sem_post(sem1);  // free 1-st seller again
           printf("1st seller is busy!\n");
       } else {  // 2-nd client (vice-versa work: if 1st customer goes to the 1st store, the 2nd goes to the second)
           sleep(2);
           // printf("freed!\n");
           *store = 0;  // now go to the 2-nd store
           sem_post(sem2);  // free 1-st seller
           sleep(2);
           printf("busy again\n");
           *store = 1;  // now go back to the 1-st store
           sem_post(sem1);  // free 2-nd seller
           sleep(2);
           *store = 0;  // finito
           sem_post(sem2);  // free 2-nd seller again
       }

   } else {
       // sem_wait(sem1);  // lock a semaphore, wait until it is unlocked by someone else
       pid2 = fork();
       if (pid2 == 0) {  // 1st seller
           sem_wait(sem1);  // 1st seller is busy
           printf("1-st seller is free! store where client is going to: %d\n", (*store + 1));
           sem_wait(sem1);  // 1st seller is busy
           printf("1-st seller is free (2nd time)! store where client is going to: %d\n", (*store + 1));

       } else {  // 2nd seller
           sem_wait(sem2);
           printf("2-nd seller is free! store where client is going to: %d\n", (*store + 1));
       }
       sleep(10);  // in order queue of customers ends
       // End of sellers working, freeing system resources
       munmap(store, sizeof *store);
   }
   // all sellers are freed, no more customers!

   sem_close(sem1);
   sem_close(sem2);
   sem_unlink(NAME_1);
   sem_unlink(NAME_2);
   return 0;
}

