//master.c
//Riley Freels

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h> //IPC
#include <sys/msg.h> //message queue
#include <sys/shm.h> //shared memory
#include <sys/sem.h> //semaphores
#include <sys/wait.h> //wait

#define MAX 256

// structure for message queue
struct mesg_buffer {
	long mesg_type;
	char mesg_text[256];
} message;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};

//integers for semaphore ids
int empty, full;


int main()
{

  //final string
  char final[256];
  
  //-------------------------------------------------------------------
  //initialize semaphores
  key_t empkey = ftok("empty",65);
  key_t fulkey = ftok("full",65);
  
  empty = semget(empkey, 1, 0666 | IPC_CREAT);
  //printf("emptyid: %d\n", empty);
  if(empty < 0)
    {
      perror("semget"); exit(1);
    }
  union semun e;
    e.val = 1;
    if(semctl(empty, 0, SETVAL, e) < 0)
    {
        perror("semctl"); exit(1);
    }

    full = semget(fulkey, 1, 0666 | IPC_CREAT);
    //printf("fullid: %d\n", full);
  if(full < 0)
    {
      perror("semget"); exit(1);
    }
  union semun f;
    f.val = 0;
    if(semctl(full, 0, SETVAL, f) < 0)
    {
        perror("semctl"); exit(1);
    }

    char emptychar[2], fullchar[2];
    sprintf(emptychar, "%d\\0", empty);
    sprintf(fullchar, "%d\\0", full);
    
  //---------------------------------------------------------------------
  //shared memory segment
      
  // ftok to generate unique key
  key_t key2 = ftok("shmfile",65);
  
  // shmget returns an identifier in shmid
  int shmid = shmget(key2,1024,0666|IPC_CREAT);
  char shmidchar[10];
  sprintf(shmidchar, "%d\\0", shmid);
  printf("%d\r", shmid);
  printf("%s\r", shmidchar);
  //---------------------------------------------------------------------
  //message queue
  key_t key;
  int msgid;
  
  // ftok to generate unique key
  key = ftok("revmes", 65);
  
  // msgget creates a message queue
  // and returns identifier
  msgid = msgget(key, 0666 | IPC_CREAT);
  char msgidchar[10];
  sprintf(msgidchar, "%d\\0", msgid);
  message.mesg_type = 1;

  printf("Enter Message> ");
  fgets(message.mesg_text,MAX,stdin);
      
  // msgsnd to send message
  msgsnd(msgid, &message, sizeof(message), 0);
      
  // display the message
  printf("Message sent: %s \n", message.mesg_text);
  //---------------------------------------------------------------------
  //pipes

  int fd[2];

  if (pipe(fd)==-1)
    {
      fprintf(stderr, "Pipe Failed" );
      return 1;
    }

  //---------------------------------------------------------------------

  
  //fork pids
  pid_t reverse, upper;

  reverse = fork();

  if(reverse == 0){
    
      //exec reverse
      //printf("exec reverse\n");
      execlp("./reverse", msgidchar, shmidchar, emptychar, fullchar);
      //printf("after exec rev\n");
    } else {
    upper = fork();

    if(upper == 0){
      //exec upper
      //printf("exec upper\n");
      dup2(fd[1], STDOUT_FILENO);
      close(fd[0]);
      close(fd[1]);
      execlp("./upper", shmidchar, emptychar, fullchar);
      //printf("after exec up\n");
    }else{
      close(fd[1]);
      read(fd[0], final, sizeof(final));
      close(fd[0]);
      printf("New Message: %s\n", final);
      wait(&upper);
      wait(&reverse);
      shmctl(shmid,IPC_RMID,NULL);
      //to destroy the message queue
      msgctl(msgid, IPC_RMID, NULL);
    }
  } 
  
  return 0;
}

