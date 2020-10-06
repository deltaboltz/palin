/*
 * Connor Schultz
 * cdstyd@umsystem.edu
 * GitHub: @deltaboltz
 *
 * CS 4760 - 02
 * Date Created: 10/5/2020
 *
 * Files Needed: master.c ; test.txt ; palin.c ; makefile
 * Files Created: N/A
 *
 * Usage: Using shared memory and congruent processes in C to read in a text file to check if a text is a palindrome or not
 *
 * Sources: https://www.geeksforgeeks.org/ipc-shared-memory/
 *          https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_shared_memory.htm
 *          https://www.programiz.com/c-programming/c-file-input-output
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include <sys/types.h>
#include <getopt.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <time.h>

void create_child(int id);

//Shared memory struct-------------------------
typedef struct
{
    int id;
    int turn;
    int children;
    int flags[20];
    char chars[128][128];
    pid_t ppid;
}sharedMemory;
sharedMemory* ptr;
//---------------------------
int MAX_CANON = 10; //max total number of children to be created
int MAX_CHILD = 5; //max total number of children allowed
int MAX_TIME = 60; //max total time (seconds) before sys time out
int procCounter;

int main(int argc, char **argv) {
    int opts;
    int i = 0;
    while((opts = getopt(argc, argv,"hn:s:t:")) != -1)
    {
        switch(opts)
        {
            case 'h':
                    printf("Usage: master -h -n x -s x -t x");
                    printf("Where:");
                    printf("-h: prints help/usage message");
                    printf("-n x: x being the max total number of children to be created");
                    printf("-s x: x being the max total number of children allowed at one time ");
                    printf("-t x: x being the max total time allowed before system time out");
            case 'n':
                MAX_CANON = atoi(optarg);
                if(MAX_CANON > 20)
                {
                    MAX_CANON = 20;
                }
                break;
            case 's':
                MAX_CHILD = atoi(optarg);
                break;
            case 't':
                MAX_TIME = atoi(optarg);
                break;
            default:
                printf("no arguments given, type master -h for usage/help message");
                exit;
        }
    }
    key_t key = ftok("./master", 'j'); //key generator for shared memory

    printf("%d\n", key);


    int shmid = shmget(key, 1024, 0600 | IPC_CREAT); //set the shared memory id


    //attach shared memory -------------------
    ptr = (sharedMemory*) shmat(shmid, NULL, 0);
    if(ptr == -1)
    {
        perror("Shared Memory failed: attachment fault");
    }
    //----------------------------------------



    //Open and read file only with "r" --------------
    FILE *fp = fopen(argv[optind], "r");
    if(fp == NULL)
    {
        perror("File: opening fault");
        exit(1);
    }

    while(fgets(ptr->chars[i], 128, fp))
    {
        ptr->chars[i][strlen(ptr->chars[i]) - 1] = NULL; //get the length of the strings with char sizes
        i += 1;
    }
    //--------------------------------------------------
    return 0;
}

void create_child(int id)
{
    if(((procCounter < MAX_CHILD) && (id < MAX_CANON)) || ((MAX_CHILD == 1) && id < MAX_CANON))
    {
        procCounter++;
        pid_t pid;
        pid = fork();

        if(pid == 0)
        {
            if(id == 0)
            {
                ptr->ppid = getpid();
            }
       }
        setpgid(0, ptr->ppid);
        char buffer[256];
        sprintf(buffer, "%d", id);
        sleep(2);
        execl("./palin", "palin", buffer, (char*) NULL);
        exit(1);
    }
}
