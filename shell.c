#include "hash.h"
#include "path.h"
#include "proc.h"
#include "shell.h"

// #define DEBUG_PROC

#define ABS     '/'
#define CD      "cd"
#define EXIT    "exit"
#define MOTD    "/u1/class/cs45602/Shell/motd.py"
#define MOTDTXT "/u1/class/cs45602/Shell/motd.txt"

#define MAX     1024

#define BG      '&'
#define PIPE    '|'
#define REL     '.'
#define RIN     '<'
#define ROUT    '>'
#define SPACE   ' '

void runshell(char *envp[])
{
    char prompt[MAX];
    char *input;

    init_procs();
    printmotd();

    for(;;) {
        proc_check();

        sprintf(prompt, "%s> ", getcwd(NULL, MAX));
        input = readline(prompt);

        if(input == NULL) {
            fprintf(stderr, "logout\n");
            exit(0);
        }

        #ifdef DEBUG_SHELL
            printf("input: '%s'\n", input);
        #endif

        if(strcmp(input, "") == 0)
        continue;

        add_history(input);
        processcmd(input, envp);
        free(input);
    }
}

void printmotd()
{
    FILE *fd;
    char *motd, *motdtxt;
    char buffer[MAX];
    
    motd = strdup(MOTD);
    motdtxt = strdup(MOTDTXT);
    fd = fopen(motdtxt, "r");

    printf("\n\n");
    processcmd(motd, 0);
    printf("\n\n");

    if(fd == NULL) {
        fprintf(stderr, "MOTD text not found\n");
        return;
    }

    while(fgets(buffer, MAX, fd) != NULL) {
        printf("%s", buffer);
    }
    printf("\n\n");
}

void processcmd(char *line, char *envp[])
{
    CMD **cmds;
    bool append, bg;
    char *fname, *infile;
    char *s, *t, *f, *g;
    int nc;

    s = t = line;
    nc = 0;
    fname = infile = NULL;
    append = bg = false;
    cmds = (CMD **)malloc(sizeof(CMD *));

    while(*t) {
        switch(*t) {
            case BG:
                bg = true;
            case PIPE:
                *t = 0;
                cmds[nc++] = makecmd(s, infile);
                cmds = (CMD **)realloc(cmds, (nc + 1) * sizeof(CMD *));
                if(infile) {
                    free(infile);
                    infile = NULL;
                }
                s = t = t + 1;
                break;
            case ROUT:
                *t = 0;
                cmds[nc++] = makecmd(s, infile);
                cmds = (CMD **)realloc(cmds, (nc + 1) * sizeof(CMD *));
                if(infile) {
                    free(infile);
                    infile = NULL;
                }
                if(*(t + 1) == ROUT) {
                    append = true;
                    t++;
                }
                s = t + 1;
                while(isspace(*s) && *s)
                    s++;
                fname = strdup(s);
                while(*t)
                    t++;
            case RIN:
                *t = 0;
                f = g = t = t + 1;
                while(isspace(*f) && *f)
                    f++;
                while(*g != PIPE && *g != ROUT && *g && *(g + 1) != SPACE && *g != BG) 
                    g++;
                while(*t != PIPE && *t != ROUT && *t && *t != BG)
                    t++;
                infile = strndup(f, g - f + 1);
                break;
            default:
                t++;
                break;
        }
    }

    if(!fname && !bg) {
        cmds[nc++] = makecmd(s, infile);
        cmds = (CMD **)realloc(cmds, (nc + 1) * sizeof(CMD *));
    }

    if(infile) {
        free(infile);
        infile = NULL;
    }

    #ifdef DEBUG_SHELL
        int i, j;

        for(i = 0; i < nc; i++) {
            printf("command %d:\n", i);
            for(j = 0; j < cmds[i]->numargs; j++) {
                printf("args %d: '%s'\n", j, cmds[i]->args[j]);
            }
            if(cmds[i]->infile) {
                printf("infile: '%s'\n", cmds[i]->infile);
            }
        }

        if(fname) {
            printf("output file: '%s'\n", fname);
        }
    #endif

    runcmd(cmds, nc, fname, append, bg, envp);

    #ifdef DEBUG_PROC
        printf("Processes: \n");
        print_procs();
    #endif
}    

void changedir(char *args[], int numargs)
{
    if(numargs > 2) {
        fprintf(stderr, "usage: cd <directory>\n");
        return;
    } 

    if(numargs == 1) {
        if(chdir(getenv("HOME")) == -1) {
            fprintf(stderr, "cannot open %s\n", args[1]);
        }
        return;
    }

    if(chdir(args[1]) == -1) {
        fprintf(stderr, "cannot open %s\n", args[1]);
        return;
    }
}

int builtins(CMD *cmds[], int nc) 
{
    if(nc != 1)
        return 1;

    if(strcmp(cmds[0]->args[0], CD) == 0) {
        changedir(cmds[0]->args, cmds[0]->numargs);
        return 0;
    }

    if(strcmp(cmds[0]->args[0], EXIT) == 0 || strcmp(cmds[0]->args[0], "logout") == 0) {
        exit(0);
    }

    return 1;
}

void runcmd(CMD *cmds[], int nc, char *fname, bool append, bool bg, char *envp[])
{
    int i;
    int in;
    int pd[2];
    int status;
    int tmpfd;
    pid_t pid;

    if(!builtins(cmds, nc))
        return;

    for(i = 0; i < nc; i++) {
        if(cmds[i]->path == NULL) {
            fprintf(stderr, "%s: command not found\n", cmds[i]->args[0]);
            return;
        }
        if(cmds[i]->infile && i > 0) {
            fprintf(stderr, "ambiguous input redirect\n");
            return;
        }
    }

    if(cmds[0]->infile) {
        tmpfd = open(cmds[0]->infile, O_RDONLY);
        
        if(tmpfd < 0) {
            fprintf(stderr, "cannot open input file\n");
            return;
        }
       
        in = tmpfd;
    } else {
        in = 0;
    }

    pid = fork();

    if(pid == 0) {
        for(i = 0; i < nc - 1; i++) {
            pipe(pd);
            makeprocess(in, pd[1], cmds[i], envp);
            close(pd[1]);
            in = pd[0];
        }

        if(in != 0) {
            dup2(in, 0);
            close(in);
        }

        if(fname) {
            if(append) {
                tmpfd = open(fname, O_CREAT | O_WRONLY | O_APPEND, 0644);
            } else {
                tmpfd = open(fname, O_CREAT | O_WRONLY, 0644);
            }
            
            if(tmpfd < 0) {
                fprintf(stderr, "%s: cannot open\n", fname);
                exit(1);
            }
            dup2(tmpfd, 1);
        }

        execve(cmds[i]->path, cmds[i]->args, envp);
        exit(1);
    } else {
        wait(&status);
        proc_push(pid);
        #ifdef SHELL_DEBUG
            fprintf(stderr, "command exit status: %d\n", status);
        #endif
    }
}

void makeprocess(int in, int out, CMD *cmd, char *envp[]) 
{
    pid_t pid;

    pid = fork();

    if(pid == 0) {
        if(in != 0) {
           dup2(in, 0);
           close(in);
        }

        if(out != 1) {
            dup2(out, 1);
            close(out);
        }

        execve(cmd->path, cmd->args, envp);
        exit(1);
    } else {
        proc_push(pid);
    }
}

char **memsep(char *s, char fsep, int inc)
{
    int i, cnt;

    for(i = 0, cnt = 1; s[i]; i++) {
        if(s[i] == fsep) {
            cnt++;
        }
    }

    return (char **)malloc((cnt + inc) * sizeof(char *));
}

int pathtype(char *arg) 
{
    if(arg[0] == ABS)
        return 2;
    else if(arg[0] == REL)
        return 1;
    else
        return 0;
}

CMD *makecmd(char *cmd, char *infile)
{
    CMD *tmp;
    char *s;
    char fsep;

    fsep = SPACE;
    tmp = malloc(sizeof(CMD));
    tmp->args = memsep(cmd, fsep, 1);
    tmp->numargs = split(tmp->args, cmd, fsep);
    tmp->args[tmp->numargs] = 0;

    if(infile) {
        tmp->infile = strdup(infile);
    } else {
        tmp->infile = NULL;
    }
    
    switch(pathtype(tmp->args[0])) {
        case 0:
            s = search(tmp->args[0]);
            if(s == NULL)
                tmp->path = NULL;
            else
                tmp->path = strdup(s);
            break;
        case 1:
        case 2:
            if(executable(tmp->args[0])) {
                tmp->path = strdup(tmp->args[0]);
            } else {
                tmp->path = NULL;
            }
            break;
    }

    return tmp;
}
