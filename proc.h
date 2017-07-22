#ifndef PROC_H_
#define PROC_H_

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int proc_push(pid_t pid);
pid_t proc_pop();
void init_procs();
void print_procs();
void proc_check();

#endif
