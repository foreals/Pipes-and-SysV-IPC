//upper.c
//Riley Freels

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <ctype.h>

struct sembuf p = { 0, -1, SEM_UNDO};
struct sembuf v = { 0, +1, SEM_UNDO};

int main(int argc, char *argv[])
{

  char *str;
  char *upper;
  int semval;

  //semval = atoi(argv[2]);
  //printf("full semval %d\n", semval);

  //decrement full
  if(semop(atoi(argv[2]), &p, 1) < 0)
    {
      perror("up semop p"); exit(1);
    }
  // ftok to generate unique key
  //key_t key = ftok("shmfile",65);
  
  // shmget returns an identifier in shmid
  //int shmid = shmget(5678,1024,0666|IPC_CREAT);
  
  // shmat to attach to shared memory
  str = (char*) shmat(atoi(argv[0]),NULL,0);
  upper = str;
  while (*upper) {
    *upper = toupper((unsigned char) *upper);
    upper++;
  }
  printf("%s", str);
  //printf("Data read from memory: %s\n", str);
  
  //detach from shared memory 
  shmdt(str);
  
  // destroy the shared memory
  //shmctl(atoi(argv[0]),IPC_RMID,NULL);
  //increment empty
  if(semop(atoi(argv[1]), &v, 1) < 0)
    {
      perror("up semop v"); exit(1);
    }
 
  shmctl(atoi(argv[0]),IPC_RMID,NULL);
  
  return 0;
}
