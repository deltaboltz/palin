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

int main(int argc, char** argv)
{
    //checking palindrome
    int i = 0;
    char str[];
    int j = strlen(str) - 1;
    while(j > i)
    {
        if(str[i++] != str(j--))
        {
            printf("%s is not a palindrome");
            break;
        }
    }
    printf("%s is a palindrome");
    return 0;
}