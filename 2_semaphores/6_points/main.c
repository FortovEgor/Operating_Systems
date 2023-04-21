// FORTOV EGOR, BPI214      //
// Variant 20            //
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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

static int *store;  // *store: 0 => go to 1-st store; 1 => go to 2-nd store

int main() {
   sem_t *sem1, *sem2;
   const char *NAME_1 = "sem1", *NAME_2 = "sem2";
   // sem1 = sem_open(NAME_1, O_CREAT | O_EXCL, 0666, 0);  // for the 1-st seller
   int SEM1 = semget(ftok("ahahaha", 1), 1, O_CREAT | O_EXCL | 0666);
   int *count_array = (int*) shmat(SEM1, NULL, 0);
   int SEM2 = semget(ftok("bebebebe", 1), 1, O_CREAT | O_EXCL | 0666);
   int *count_array_2 = (int*) shmat(SEM2, NULL, 0);
   // sem2 = sem_open(NAME_2, O_CREAT | O_EXCL, 0666, 0);  // for the 2-nd seller

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
       pid3 = fork();    // for scaling just make one more fork in else statement!
       if (pid3 == 0) {  // 1-st client
           printf("1st seller is busy!\n");
           /* 1st seller is busy with the 1-st customer during 2 secs
             (during this time 1-st seller's process is blocked)    */
           sleep(2);
           *store = 1;      // now go to the 2-nd store
           // sem_post(sem1);  // free 1-st seller
           struct sembuf mybuf;
           mybuf.sem_num = 0;
           mybuf.sem_flg = 0;
           mybuf.sem_op = 1;
           semop(SEM1, &mybuf, 1);
           printf("2nd seller is busy!\n");
           /* 2nd seller is busy with the 1-st customer during 2 secs
             (during this time 2-st seller's process is blocked)    */
           sleep(2);
           *store = 0;      // now go back to the 1-st store
           mybuf.sem_num = 0;
           mybuf.sem_flg = 0;
           mybuf.sem_op = 1;
           semop(SEM2, &mybuf, 1);
           // sem_post(sem2);  // free 2-nd seller
           printf("1st seller is busy!\n");
           /* AGAIN: 1st seller is busy with the 1-st customer during 2 secs
             (during this time 2-st seller's process is blocked)    */
           sleep(2);
           *store = 1;      // finito
           // sem_post(sem1);  // free 1-st seller again
           // struct sembuf mybuf;
           mybuf.sem_num = 0;
           mybuf.sem_flg = 0;
           mybuf.sem_op = 1;
           semop(SEM1, &mybuf, 1);
           printf("1st seller is busy!\n");
       } else {  // 2-nd client (vice-versa work: if 1st customer goes to the 1st store, the 2nd
                 // goes to the second)
           sleep(2);
           // printf("freed!\n");
           *store = 0;      // now go to the 2-nd store
           struct sembuf mybuf;
           mybuf.sem_num = 0;
           mybuf.sem_flg = 0;
           mybuf.sem_op = 1;
           semop(SEM2, &mybuf, 1);
           // sem_post(sem2);  // free 1-st seller
           sleep(2);
           printf("busy again\n");
           *store = 1;      // now go back to the 1-st store
           // sem_post(sem1);  // free 2-nd seller
           mybuf.sem_num = 0;
           mybuf.sem_flg = 0;
           mybuf.sem_op = 1;
           semop(SEM1, &mybuf, 1);
           sleep(2);
           *store = 0;      // finito
           // sem_post(sem2);  // free 2-nd seller again
           mybuf.sem_num = 0;
           mybuf.sem_flg = 0;
           mybuf.sem_op = 1;
           semop(SEM2, &mybuf, 1);
       }

   } else {
       // sem_wait(sem1);  // lock a semaphore, wait until it is unlocked by someone else
       pid2 = fork();
       if (pid2 == 0) {     // 1st seller
           // sem_wait(sem1);  // 1st seller is busy
           struct sembuf mybuf;
           mybuf.sem_num = 0;
           mybuf.sem_flg = 0;
           mybuf.sem_op = 0;
           semop(SEM1, &mybuf, 1);
           printf("1-st seller is free! store where client is going to: %d\n", (*store + 1));
           mybuf.sem_num = 0;
           mybuf.sem_flg = 0;
           mybuf.sem_op = 0;
           semop(SEM1, &mybuf, 1);
           // sem_wait(sem1);  // 1st seller is busy
           printf("1-st seller is free (2nd time)! store where client is going to: %d\n",
                  (*store + 1));
       } else {  // 2nd seller
           // sem_wait(sem2);
           struct sembuf mybuf;
           mybuf.sem_num = 0;
           mybuf.sem_flg = 0;
           mybuf.sem_op = 0;
           semop(SEM1, &mybuf, 1);
           printf("2-nd seller is free! store where client is going to: %d\n", (*store + 1));
       }
       sleep(10);  // in order queue of customers ends
       // End of sellers working, freeing system resources
       munmap(store, sizeof *store);
   }
   // all sellers are freed, no more customers!

   // sem_close(sem1);
   (void) shmdt(count_array);
   (void) shmdt(count_array_2);
   // sem_close(sem2);
   sem_unlink(NAME_1);
   sem_unlink(NAME_2);
   return 0;
}
