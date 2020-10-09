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
 *          https://stackoverflow.com/questions/5656530/how-to-use-shared-memory-with-linux-in-c#:~:text=Steps%20%3A%201%20Use%20ftok%20to%20convert%20a,the%20memory%20area.%205%20Detach%20using%20shmdt.%20
 *          https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_shared_memory.htm
 *          https://www.programiz.com/c-programming/c-file-input-output
 *          https://stackoverflow.com/questions/4217037/catch-ctrl-c-in-c
 *          https://codeforwin.org/2016/02/c-program-to-find-maximum-and-minimum-using-functions.html#:~:text=%20C%20program%20to%20find%20maximum%20and%20minimum,numbers%3A%2010%2020%20%20Maximum%20%3D...%20More%20
 *          https://stackoverflow.com/questions/42258485/detaching-from-shared-memory-before-removing-it
 *          https://www.gnu.org/software/libc/manual/html_node/Setting-an-Alarm.html
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
#include <sys/time.h>

#define PERMS (S_IRUSR | S_IWUSR)

void create_child(int index);
void handler(int signal); //ctrl-c handler for the user to abort the program

void freeshm();
void alarmTimer(int secs);
void timer(int signal);

//Shared memory struct-------------------------
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
//---------------------------
int MAX_CANON = 10; //max total number of children to be created
int MAX_CHILD = 2; //max total number of children allowed
int MAX_TIME = 60; //max total time (seconds) before sys time out
unsigned int procCounter = 0;
unsigned int procIndex;

int shmid;

int main(int argc, char **argv) {

    signal(SIGINT, handler); //catches ctrl-c and needs free the shared memory

    int opts;
    int i = 0;


    printf("making files\n");


    printf("key\n");
    int key = ftok("master", 'j'); //key generator for shared memory

    while((opts = getopt(argc, argv,"hn:s:t:")) != -1)
    {
        switch(opts)
        {
            case 'h':
                    printf("Usage: master -h -n x -s x -t x\n");
                    printf("Where:\n");
                    printf("-h: prints help/usage message\n");
                    printf("-n x: x being the max total number of children to be created (must be >0)\n");
                    printf("-s x: x being the max total number of children allowed at one time (must be >0)\n");
                    printf("-t x: x being the max total time allowed before system time out\n");
                    exit(0);
            case 'n':
                MAX_CANON = atoi(optarg);
                if(MAX_CANON > 20)
                {
                    MAX_CANON = 20;
                }
                if(MAX_CANON < 0)
                {
                    printf("invalid argument, type 'master -h' for usage/help message\n");
                    exit(1);
                }
                break;
            case 's':
                MAX_CHILD = atoi(optarg);
                if(MAX_CHILD < 0)
                {
                    printf("invalid argument, type 'master -h' for usage/help message\n");
                    exit(1);
                }
                break;
            case 't':
                MAX_TIME = atoi(optarg);
                if(MAX_TIME < 0)
                {
                    printf("Invalid argument, type 'master -h' for usage/help message\n");
                }
                if(MAX_TIME > 20)
                {
                    MAX_TIME = 20;
                }
                break;
            default:
                printf("no arguments given, type master -h for usage/help message\n");
                exit(1);
        }
    }




    printf("%d\n", key);

    shmid = shmget(key, sizeof(sharedMemory), 0600 | IPC_CREAT | IPC_EXCL); //set the shared memory id
    printf("shmget\n");

    if(shmid == -1)
    {
        freeshm();
        perror("Failed to get shmid");

        return 1;
    }


    printf("created key\n");

    //attach shared memory -------------------
    ptr = (sharedMemory*) shmat(shmid, NULL, 0);
    printf("HELLOLOOOOOOO\n\n\n\n\n");

    if(ptr == (void*)-1)
    {
        freeshm();
        perror("Shared Memory failed\n");
        return 1;
    }
    //----------------------------------------



    //Open and read file only with "r" --------------
    printf("opening file\n" );
    FILE *fp = fopen(argv[optind], "r");
    if(fp == NULL)
    {
        perror("File: opening fault");
        exit(1);
    }

    while(fgets(ptr->chars[i], 128, fp))
    {
        printf("getting all chars");
        ptr->chars[i][strlen(ptr->chars[i]) - 1] = '\0'; //get the length of the strings with char sizes
        i++;
    }
    fclose(fp);
    //--------------------------------------------------


    alarmTimer(MAX_TIME);
    procIndex = 0;

    printf("getting procIndex\n");
    while (procIndex < MAX_CANON)
    {
        create_child(procIndex++);
    }
    while(procCounter > 0)
    {
        wait(NULL);
        if(procIndex < i)
        {
            create_child(procIndex++);
        }
        procCounter -= 1;
    }

    freeshm();
    return 0;
}

void create_child(int index)
{
    printf("creating a child\n");
    if(((procCounter < MAX_CHILD) && (index < MAX_CANON)) || ((MAX_CHILD == 1) && index < MAX_CANON))
    {
        printf("We're in\n");
        procCounter++;
        pid_t pid;
        pid = fork();
        if(pid == 0)
        {
            printf("FORKING\n");
            if(index == 0)
            {
                printf("getting pid");
                ptr->ppid = getpid();
            }
       }
        printf("we forked :)\n");
        setpgid(0, ptr->ppid);

        printf("we setpgid\n");
        char buffer[256];

        printf("made buffer\n");

        printf("time to sprintf\n");
        sprintf(buffer, "%d", index);

        printf("done with sprintf\n");

        printf("sleeping the program\n");

        sleep(1);


        printf("starting ./palin");


        execl("./palin", "palin", buffer, (char*) NULL);
        exit(1);
    }
    else
    {
        printf("Process failed\n");
        procIndex -= 1;
    }
}

void handler(int signal)
{
    killpg(ptr->ppid, SIGTERM); //kill the child process
    //will need to wait for the child process to end to not leave an orphan


    //empty while loop didn't work last time in project 1, trying an empty do-while
    do{}while(wait(NULL) > 0);

    freeshm(shmid);
}

void freeshm() {
    if (shmdt(ptr) == -1) {
        perror("Memory error: detaching fault");
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Memory error: delete fault");
    }
}

    void alarmTimer(int secs) {
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_handler = &timer;
        sa.sa_flags = 0;
        if (sigaction(SIGALRM, &sa, NULL) < 0) {
            perror("");
            exit(1);
        }

        struct itimerval new;
        new.it_interval.tv_usec = 0;
        new.it_interval.tv_sec = 0;
        new.it_value.tv_usec = 0;
        new.it_value.tv_sec = (long int) secs;
        if (setitimer(ITIMER_REAL, &new, NULL) < 0) {
            perror("ERROR TIMER");
            exit(1);
        }
    }

    void timer(int signal) {

        killpg(ptr->ppid, SIGUSR1);
        //wait for children to end
        while (wait(NULL) > 0);
        freeshm();
        exit(1);
    }
