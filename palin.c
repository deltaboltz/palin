/*
 * Connor Schultz
 * cdstyd@umsystem.edu
 * GitHub: @deltaboltz
 *
 * CS 4760 - 02
 * Date Created: 10/5/2020
 *
 * Files Needed: master.c ; test.txt ; palin.c ; makefile
 * Files Created: output.log
 *
 * Usage: Using shared memory and congruent processes in C to read in a text file to check if a text is a palindrome or not
 *
 * Sources: https://iq.opengenus.org/boolean-in-c/
 *          https://stackoverflow.com/questions/42258485/detaching-from-shared-memory-before-removing-it
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <getopt.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <time.h>


//shared memory struct---------------
typedef struct
{
    int id;
    int turn;
    int children;
    int flags[20];
    char chars[80][80];
    pid_t ppid;
}sharedMemory;
sharedMemory* ptr;
//-----------------------------------

void handler(int signal, int shmid); //ctrl-c handler for the user to abort the program
int palinCheck(char chars[]);
void freeshm();
void waitingRoom();
void criticalSection();

int main(int argc, char** argv)
{

    printf("We're now in palin.c");
    key_t key = ftok("./master", 'j'); //key generator for shared memory
    int shmid = shmget(key, 1024, 0600 | IPC_CREAT);
    ptr = (sharedMemory*) shmat(shmid, NULL, 0);
    if((int) ptr == -1)
    {
        perror("Shared Memory failed: attachment fault");
    }
    signal(SIGINT, handler);

    int id = atoi(argv[1]);
    if(palinCheck(ptr->chars[id]) == 1)
    {
        printf("done checking");
    }

    return 0;
}

int palinCheck(char chars[])
{
    printf("We're in palinCheck now");
    return 1;
}

void waitingRoom()
{
    printf("Waiting room");
    criticalSection();
}

void criticalSection()
{
    printf("Critical Section");
}

void handler(int signal, int shmid)
{
    exit(1);
    freeshm(shmid);
}

void freeshm(int shmid)
{
    if(shmdt(ptr) == -1)
    {
        perror("Memory error: detaching fault");
    }
    if(shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("Memory error: delete fault");
    }
}