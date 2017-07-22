#include "hash.h"
#include "shell.h"
#include "path.h"

int main(int argc, char *argv[], char *envp[])
{
    char **paths;
    int i;

    paths = getPathDirectories();

    for(i = 0; paths[i] != 0; i++) {
        processpath(paths[i]);
    }

    #ifdef DEBUG_HTBL
        printhtbl();
    #endif
    
    runshell(envp); 

    return 0;
}


