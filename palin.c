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
    int chars[80][80];
    pid_t ppid;
}sharedMemory;
sharedMemory* ptr;
//-----------------------------------

void handler(int signal); //ctrl-c handler for the user to abort the program
int palinCheck(char chars[]);
void freeshm();
void handler(int signal);

int main(int argc, char** argv)
{

    printf("We're now in palin.c");
    signal(SIGINT, handler);

    return 0;
}

void handler(int signal)
{
    exit(1);
    freeshm();
}

void freeshm()
{

}