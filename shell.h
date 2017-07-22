#ifndef SHELL_H_
#define SHELL_H_

#include <ctype.h>
#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct cmd {
    char **args;
    char *infile;
    char *path;
    int numargs;
}CMD;

CMD *makecmd(char *cmd, char *infile);
char **memsep(char *s, char fsep, int inc);
int builtins(CMD *cmds[], int nc); 
int pathtype(char *arg); 
void changedir(char *args[], int numargs);
void makeprocess(int in, int out, CMD *cmd, char *envp[]); 
void printmotd();
void processcmd(char *cmd, char *envp[]);
void runcmd(CMD *cmds[], int nc, char *fname, bool append, bool bg, char *envp[]);
void runshell(char *envp[]);

#endif
