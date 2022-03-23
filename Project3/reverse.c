//reverse.c
//Riley Freels

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>   //message queue
#include <sys/sem.h>   //semaphores
#include <fcntl.h>
#include <sys/shm.h>   //shared memory
#include <stdlib.h>
#include <string.h>    //strlen

struct mesg_buffer {
  long mesg_type;
  char mesg_text[256];
} message;

struct sembuf p = { 0, -1, SEM_UNDO};
struct sembuf v = { 0, +1, SEM_UNDO};

char *reverse(char *str);

int main(int argc, char *argv[])
{
  key_t key;
  int semval;
  char * str, *reversed;
  // msgrcv to receive message
  msgrcv(atoi(argv[0]), &message, sizeof(message), 1, 0);
  
  // display the message
  reversed = reverse(message.mesg_text);
  //printf("Reverse: %s \n", reversed);

  //decrement empty
  if(semop(atoi(argv[2]), &p, 1) < 0)
    {
      perror("rev semop p"); exit(1);
    }
  str = (char*) shmat(atoi(argv[1]),NULL,0);
  sprintf(str, "%s", reversed);
  shmdt(str);
  //increment full
  if(semop(atoi(argv[3]), &v, 1) < 0)
    {
      perror("rev semop v"); exit(1);
    }
  
  msgctl(atoi(argv[0]), IPC_RMID, NULL);
  return 0;
}

char *reverse(char *str){
  char c, *front, *back;
  
  if(!str || !*str)
    return str;
  for(front=str,back=str+strlen(str)-1;front < back;front++,back--){
    c=*front;*front=*back;*back=c;
  }
  return str;
}
