#include "proc.h"

#define NUMPROC 100

static pid_t *procs;
static int np;

void init_procs()
{
    procs = (pid_t *)malloc(NUMPROC * sizeof(pid_t));
    np = 0;
}

int proc_push(pid_t pid)
{
    if(np < NUMPROC) {
        procs[np++] = pid;
        return 1;
    }
    return 0;
}

pid_t proc_pop()
{
    if(np > 0)
        return procs[np--];
    return 0;
}

void print_procs()
{
    int i;

    for(i = 0; i < np; i++) {
        printf("proc %d: %d\n", i, procs[i]);
    }
}

void proc_check()
{
    while(np > 0) {
        if(kill(procs[np - 1], 0) == -1) {
            proc_pop();
        }
    }
}
