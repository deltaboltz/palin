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
#include <stdbool.h>


//shared memory struct---------------
typedef struct
{
    unsigned int id;
    unsigned int turn;
    unsigned int children;
    unsigned int flags[20];
    char chars[64][64];
    pid_t ppid;
}sharedMemory;
sharedMemory* ptr;
enum status {idling, want_in, incrit};
//-----------------------------------

void handler(int signal); //ctrl-c handler for the user to abort the program
int palinCheck(char chars[]); //check if the characters are a palindrome
void freeshm(); //free memory function
void waitingRoom( unsigned int id, bool palinFile); //waiting room for the critical section
void sleepTime(unsigned int id, bool palinFile, unsigned int children);
void criticalSection(unsigned int id, bool palinFile, unsigned int children); //critical section

int shmid; //had to make shmid a global in order to use signal checking

int main(int argc, char** argv)
{

    printf("We're now in palin.c\n");
    int key = ftok(".", 'a'); //key generator for shared memory
    shmid = shmget(key, sizeof(sharedMemory), 0600 | IPC_CREAT);

    if(shmid == -1)
    {
        perror("SHMID");
        freeshm();
    }
    ptr = (sharedMemory*) shmat(shmid, NULL, 0);

    if(ptr == (void*)-1)
    {

        perror("Shared Memory failed\n");
        freeshm();
        return 1;
    }

    if((int*) ptr == (int*)-1)
    {
        perror("Shared Memory failed: attachment fault");
    }
    signal(SIGINT, handler);

    int id = atoi(argv[1]);

    bool palinFile = palinCheck(ptr->chars[id]);

    waitingRoom(id, palinFile);

    return 0;
}

int palinCheck(char chars[])
{
    int i = 0;
    int j = strlen(chars)-1;
    while (i < j)
    {
        if(chars[i++] != chars[j--])
        {
            return false;
        }
    }
    return true;
}

void waitingRoom(unsigned int id, bool palinFile)
{
    printf("We're in the lobby\n");
    int i , j;
    int children = ptr->children; //pointer to the child process wanting into the critsection
    int turnPointer = ptr->turn;
     //status of child process

    //set the next child into the want_in status
    do{
        printf("inside do-while loop");
        ptr->flags[id] = want_in;
        i = turnPointer;
        while(i != id)
        {
            i = (ptr->flags[i] != idling) ? turnPointer : (i + 1) % children;
        }

        ptr->flags[id] = incrit;
        j = 0;

        //Check to see if a child is in the critical section already
        for(i = 0; i < children; i++)
        {
            if(i != id && ptr->flags[i] == incrit)
            {
                break;
            }
        }
        //----------------------------------------------------------
    }while((i < children) || ((turnPointer != id) && (ptr -> flags[turnPointer] != idling)));


    //Call the sleep timer
    printf("Sleeping time...\n");
    sleepTime(id, palinFile, children);
}

void sleepTime(unsigned int id, bool palinFile, unsigned int children)
{
    //set up random sleep time
    time_t sleepTime;
    srand((int)time(&sleepTime));
    sleep(rand() % 3);

    printf("set up sleep time\n");

    //Call the critical section

    printf("critsec time\n");
    criticalSection(id, palinFile, children);
}

void criticalSection(unsigned int id, bool palinFile, unsigned int children)
{
    ptr->turn = id;
    unsigned int turnPointer = ptr->turn;

    printf("opening file...\n");
    FILE *fp = fopen(palinFile ? "palin.out" : "nopalin.out" , "+a");

    printf("fprintf time\n");
    fprintf(fp, "$s\n", ptr->chars[id]);

    if(fp == NULL)
    {
        perror("Opening file error: palinFile fault");
    }
    printf("closing first file\n");
    fclose(fp);

    printf("opening another file :)\n");
    FILE *fpp = fopen("out.log", "a+");
    if(fpp == NULL)
    {
        perror("Opening file error: out.log fault");
    }

    printf("closing second file :(\n");
    fclose(fpp);



    printf("turn pointer change\n");
    turnPointer = (turnPointer + 1) % children;

    while(ptr->flags[turnPointer] == idling)
    {
        turnPointer = (turnPointer + 1) % children;
    }
    ptr->flags[id] = idling;
}

void handler(int signal)
{
    exit(1);
}

void freeshm()
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