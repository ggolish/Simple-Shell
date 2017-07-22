#ifndef PATH_H_
#define PATH_H_

#include <ctype.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DEFAULT_PATH    "/bin"
#define MAXBUFF         1024
#define MAXPATHLEN      1024
#define NGROUPS         100
#define PSEP            ':'
#define TARGET          "PATH456"

char **getPathDirectories();
int executable(char *path);
int findexecutables(char *fname, char *path);
int processpath(char *dirname);
int runpipe(char *args[], int numargs);
int split(char *p[], char *var, char psep);
void getusrinfo();

#endif

