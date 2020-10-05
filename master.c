/*
 * Connor Schultz
 * cdstyd@umsystem.edu
 * GitHub: @deltaboltz
 *
 * CS 4760 - 02
 * Date Created: 10/5/2020
 *
 * Files Needed: master.c ; test.txt ; palin.c
 * Files Created: N/A
 *
 * Usage: Using shared memory and congruent processes in C to read in a text file to check if a text is a palindrome or not
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

int main(int argc, char **argv) {
    int opts;
    int MAX_CANON; //max total number of children to be created
    int MAX_CHILD; //max total number of children allowed
    int MAX_TIME; //max total time before sys time out

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
                exit(1);
        }
    }
    return 0;
}
