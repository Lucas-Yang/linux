#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 80
#define BUFFER_SIZE 50
#define buffer "\nCommand History:\n"

char history[10][BUFFER_SIZE];
int cnt = 0;
int caught = 0;


void handle_SIGINT()//用于接收ctrl+C信号并别打印历史信号
{
    int i;
    int j = 0;
    int historyCount = cnt;

    write(STDOUT_FILENO, buffer, sizeof(buffer));

   for (i = 0; i<10;i++)
     {
          printf("%d.  ", i+1);
          while (history[historyCount-i-1][j] != '\n' && history       [historyCount-i-1][j] != '\0')
          {
               printf("%c", history[historyCount-i-1][j]);
               j++;
          }
          printf("\n");
          j = 0;
          if (historyCount ==  i+1)
          break;
     }

    //printf("COMMAND-> \n");
    caught = 1;
}

void setup(char inputBuffer[], char *args[], int *background)
{
    int len, i, start, ct = 0;

    len  = read(STDIN_FILENO, inputBuffer, MAX_LINE);

    if (caught == 1) {
        len = read(STDIN_FILENO, inputBuffer, MAX_LINE);
        caught = 0;
    }


    start = -1;

    if (len == 0) {
        printf("\excit\n");
        exit(0);    // Ctr+D, exit
    }
    if (len < 0) {
        perror("command error\n");
        exit(-1);    // command error
    }

    for (i = 0; i < len; ++i) {
        switch(inputBuffer[i]) {
            case ' ':
            case '\t': {
                if (start != -1) {
                    args[ct] = &inputBuffer[start];
                    ++ct;
                }
                inputBuffer[i] = '\0';
                start = -1;
                break;
            }
            case '\n': {
                if (start != -1) {
                    args[ct] = &inputBuffer[start];
                    ++ct;
                }
                inputBuffer[i] = '\0';
                args[ct] = NULL;
                break;
            }
            default: {
                if (start == -1) start = i;
                if (inputBuffer[i] == '&') {
                    *background = 1;
                    inputBuffer[i] = '\0';
                }
            }
        }
    }
    args[ct] = NULL;
    if (args[0][0] == 'r') {
        if (args[0][1] == '_') {
            int find = 0;
            for (i = 9; i >= 0; --i) {
                if (history[i][0] == args[0][2]) {
                    printf(0);
                    strcpy(inputBuffer, history[i]);
                    find = 1;
                    break;
                }
            }
            if (find == 0) strcpy(inputBuffer, "no such command");
        }
        if (args[0][1] == '\0') strcpy(inputBuffer, history[0]);
    }

    for (i = 9; i > 0; --i) strcpy(history[i], history[i-1]);

    strcpy(history[0], inputBuffer);
    ++cnt;
    if (cnt > 10) cnt = 10;
}


int main()
{
    char inputBuffer[MAX_LINE];
    int background;
    char *args[MAX_LINE/2 + 1];

    pid_t pid;
    //int i;

    struct sigaction handler;
    handler.sa_handler = handle_SIGINT;
    sigaction(SIGINT, &handler, NULL);

    while (1) {
        background = 0;
        printf("COMMAND-> \n");
        setup(inputBuffer, args, &background);

        pid = fork();        // fork a child process
        if (pid < 0) {
            printf("Fork Failed.\n");
            exit(1);
        }
        else if (pid == 0) {
            if (execvp(args[0], args) == -1) printf("No such Command .\n");
        }
        else {
            if (background == 0) wait(NULL);
        }
        }
        }
