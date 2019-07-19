/****************************************************************
 * Name        : Anmol Gondara                                  *
 * Class       : CSC 415                                        *
 * Date        : 10/04/2018                                     *
 * Description :  Writing a simple bash shell program           *
 *                that will execute simple commands. The main   *
 *                goal of the assignment is working with        *
 *                fork, pipes and exec system calls.            *
 ****************************************************************/

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFFERSIZE 256
#define PROMPT "myShell >> "
#define PROMPTSIZE sizeof(PROMPT)

int main(int argc, char** argv) {
    wait(NULL);
    //declare buffer of size BUFFERSIZE, tokens, and myargv array to store series of strings
    char buffer[BUFFERSIZE];
    char *toks;
    char *myargv[84];

    //declare child process ID using pid_t data type
    pid_t c_pID;

    //do-while loop to achieve an infinite loop
    do {
        //print myShell >>
        printf("%s", PROMPT);

        //fill buffer with '\0' to empty it
        memset(buffer, '\0', BUFFERSIZE);

        //do-while loop to clear myargv array elements
        int b;
        b = 0;
        do {
            myargv[b] = NULL;
            b = b + 1;
        }
        while (84 > b);

        //conditional statement that uses fgets function to read line from user. If Null, then loop breaks
        if (NULL != fgets(buffer, BUFFERSIZE, stdin)) {

            //set tokens to strtok function which breaks the string into tokens with " \t\n" delim
            toks = strtok(buffer, " \t\n");
            //conditional statement to skip to next loop if toks are Null
            if (NULL == toks) {
                continue;
            }
            //conditional statement that utilizes strcmp function which compares the string exit. When true, return 0.
            if (0 == strcmp(buffer, "exit")) {
                return 0;
            }

            //do-while loop to break the strings in myargv array into tokens when a new line delim is recognized
            int a;
            a = 0;
            do {
                myargv[a] = toks;
                toks = strtok(NULL, " \n");
                a = a + 1;
            } 
            while (NULL != toks);

            int fDscrptrProcess[2];
            //assign child process ID to the fork function which creates another process to run simultaneously (parent process)
            c_pID = fork();
            //pipe function is used here to pass information from process to process.
            pipe(fDscrptrProcess);

            //declare file descriptor to have access to files
            int fDscrptr;
            //conditional statement for when true/no errors
            if (0 == c_pID) {
                if (2 < a) {
                    //conditional statement which utilizes the strcmp function to recognize the < command
                    if (strcmp(myargv[a - 2], "<") != 0) {}
                    else {
                        //open file and set permission to read only
                        fDscrptr = open(myargv[a - 1], O_RDONLY, 0);
                        //dup2 function to redirect output
                        dup2(fDscrptr, 0);
                        //close file
                        close(fDscrptr);
                    }
                    //conditional statement which utilizes the strcmp function to recognize the > command
                    if (strcmp(myargv[a - 2], ">") != 0) {}
                    else {
                        //open file and set permissions to create, truncate, and write
                        fDscrptr = open(myargv[a - 1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
                        //dup2 function to redirect output
                        dup2(fDscrptr, STDOUT_FILENO);
                        //close file
                        close(fDscrptr);
                    }
                    //conditional statement which utilizes the strcmp function to recognize the >> command
                    if (strcmp(myargv[a - 2], ">>") != 0) {}
                    else {
                        //open file and set permission to read, create, and append
                        fDscrptr = open(myargv[a - 1], O_RDWR | O_CREAT | O_APPEND, 0644);
                        //dup2 function to redirect output
                        dup2(fDscrptr, STDOUT_FILENO);
                        //close file
                        close(fDscrptr);
                    }
                }
                //conditional statement which utilizes the strcmp function to recognize cd command
                if (strcmp(myargv[0], "cd") == 0) {
                    char *index = myargv[1];
                    int pres;
                    //assign presumed to the chdir function which changes the current working directory
                    pres = chdir(index);
                    //conditional statement to output error message when presumed is -1
                    if (pres != -1) {}
                    else {
                        perror("***Error***");
                    }
                }
                //conditional statement which utilizes the strcmp function to recognize pwd command
                if (strcmp("pwd", myargv[0]) == 0) {
                    char outputIdx[BUFFERSIZE];
                    //if-else statement to print the current working directory by utilizing the getcwd function
                    if (getcwd(outputIdx, BUFFERSIZE) == NULL) {
                        printf("***Error***");
                    } 
                    else {
                        printf("%s\n", outputIdx);
                    }
                    continue;
                }
                //series of conditional statements that utilize the strcmp function to recognize
                //the noted commands &, <, >, >>, and |.
                //elements of myargv array are deleted
                if (2 >= a || strcmp(myargv[a - 2], "|") != 0) {
                    if (2 >= a || strcmp(myargv[a - 2], "<") != 0) {
                        if (2 >= a || strcmp(myargv[a - 2], ">") != 0) {
                            if (2 >= a || strcmp(myargv[a - 2], ">>") != 0) {
                                if (1 >= a || strcmp(myargv[a - 1], "&") != 0) {}
                                else {
                                    myargv[a - 1] = NULL;
                                }
                            } 
                            else {
                                myargv[a - 2] = NULL;
                                myargv[a - 1] = NULL;
                            }
                        } 
                        else {
                            myargv[a - 2] = NULL;
                            myargv[a - 1] = NULL;
                        }
                    } 
                    else {
                        myargv[a - 2] = NULL;
                        myargv[a - 1] = NULL;
                    }
                } 
                else {
                    myargv[a - 2] = NULL;
                    myargv[a - 1] = NULL;
                }
                //utilize the execvp function to allow process to run program files
                execvp(myargv[0], myargv);
            }

            //conditional statement which utilizes the strcmp function to recognize when
            //to execute command in the background
            if (0 < c_pID) {
                if (0 != strcmp(myargv[a - 1], "&")) {}
                else {
                    printf("Execute Command in Background\n");
                }
                //declare process ID and position variables
                int pID;
                int pos;
                //assign the process ID to wait function which will block the process until the position is returned
                //used to test
                pID = wait(&pos);
            }

            //conditional statement that outputs error message and exit's the
            //program when the child process ID is less than 0
            if (0 > c_pID) {
                perror("***Error***");
                exit(0);
            }
        }
        else {
            break;
        }
    }
    while(415);

    //return 1 which signals an error when the infinite do-while loop is broken
    return 1;
}
