#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void ParentProcess(int []);
void ChildProcess(int []);

int main(int argc, char *argv[])
{
  int shmID;
  int status;
  int *shmPTR;
  
  pid_t pid;

  shmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
  if (shmID < 0) {
    printf("*** shmget error (main) ***\n");
    exit(1);
  }
  printf("Process has received a shared memory of four integers...\n");

  shmPTR = (int *) shmat(shmID, NULL, 0);
  if (*shmPTR == -1){
    printf("*** shmat error (main) ***\n");
    exit(1);
  }
  printf("Process has attached the shared memory...\n");

  shmPTR[0] = 0;
  shmPTR[1] = 0;
  printf("Bank initalized to: %d\n", shmPTR[0]);

  pid = fork();
  if (pid < 0) {
    printf("*** fork error (main) ***\n");
    exit(1);
  } else if (pid == 0) {
    ChildProcess(shmPTR);
    exit(0);
  } else {
    ParentProcess(shmPTR);
  }
  wait(&status);
  printf("Process has detected completion of child process...\n");
  shmdt((void *) shmPTR);
  printf("Process has detached its shared memory...\n");
  shmctl(shmID, IPC_RMID, NULL);
  printf("Process has removed its shared memory...\n");
  printf("Process exits...\n");
}

void ParentProcess(int sharedMem[]){
  int acc, randBalance;
  srand(getpid());

  int i;
  for(i = 0; i < 25; i++){
    sleep(rand()%6);
    acc = sharedMem[0];
    while(sharedMem[1] != 0);
    if (acc <= 100) {
      randBalance = rand()%101;
      if (randBalance % 2) {
        acc += randBalance;
        printf("Dear old Dad: Deposits $%d / Balance = $%d\n", randBalance, acc);
        sharedMem[0] = acc;
        } else {
          printf("Dear old Dad: Doesn't have any money to give\n");
        }
    } else {
      printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", acc);
    }
    sharedMem[1] = 1;
  }
}

void ChildProcess(int sharedMem[]){
  int acc, randBalance;
  srand(getpid());

  int i;
  for(i = 0; i < 25; i++){
    sleep(rand()%6);
    while(sharedMem[1] != 1);
    acc = sharedMem[0];
    randBalance = rand() % 51;

    printf("Poor Student needs $%d\n", randBalance);
    if (randBalance <= acc){
      acc -= randBalance;
      printf("Poor Student: Withdraws $%d / Balance = $%d\n", randBalance, acc);
      sharedMem[0] = acc;
    } else {
      printf("Poor Student: Not Enough Cash ($%d)\n", acc);
    }
    sharedMem[1] = 0;
  }
}