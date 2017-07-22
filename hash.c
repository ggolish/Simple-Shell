#include "hash.h"

static NODE *htbl[HTBLSZ];

static NODE *makenode(char *key, char *path);
static int hash(char *key);

static int hash(char *key)
{
    int rv = 0;

    while(*key)
        rv = rv << 1 ^ *key++;

    if(rv < 0)
        rv = -rv;

    return rv % HTBLSZ;
}

int insert(char *key, char *path)
{
    NODE *curr;
    int i;

    i = hash(key);

    if(htbl[i] == NULL) {
        htbl[i] = makenode(key, path);
        return 1;
    }

    for(curr = htbl[i]; curr != NULL; curr = curr->next) {
        if(strcmp(key, curr->key) == 0) {
            return 0;
        }
    }

    curr = makenode(key, path);
    curr->next = htbl[i];
    htbl[i] = curr;
    return 1;
}

char *search(char *key)
{
    NODE *curr;
    int i;

    i = hash(key);

    if(htbl[i] == NULL)
        return 0;

    for(curr = htbl[i]; curr != NULL; curr = curr->next) {
        if(strcmp(key, curr->key) == 0) {
            return curr->path;
        }
    }

    return 0;
}

static NODE *makenode(char *key, char *path)
{
    NODE *tmp;

    tmp = malloc(sizeof(NODE));

    if(tmp == NULL) {
        perror("malloc");
        exit(1);
    }

    tmp->key = strdup(key);
    tmp->path = strdup(path);
    tmp->next = NULL;

    return tmp;
}

void printhtbl()
{   
    NODE *curr;
    int i;

    for(i = 0; i < HTBLSZ; i++) {
        if(htbl[i]) {
            for(curr = htbl[i]; curr != NULL; curr = curr->next) {
                printf("%d: %s\n", i, curr->key);
            }
        }
    }
}
