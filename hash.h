#ifndef HASH_H_
#define HASH_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HTBLSZ 8192

typedef struct node {
    char *key;
    char *path;
    struct node *next;
}NODE;

int insert(char *key, char *path);
char *search(char *key);
void printhtbl();

#endif
