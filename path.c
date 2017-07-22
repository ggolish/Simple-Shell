#include "hash.h"
#include "path.h"

static gid_t grps[NGROUPS];
static int ngrps = NGROUPS;
static uid_t usr;

char **getPathDirectories()
{
   char **fields;
   char *buffer;
   char *p;
   int i, nf;

   p = getenv(TARGET);
   
   if(!p) {
      p = DEFAULT_PATH;
   }
   
   buffer = strdup(p);
   nf = 1;
   
   for(i = 0 ; buffer[i] ; i++) {
      if(buffer[i] == PSEP) {
         nf++;
      }
   }
   
   fields = malloc((nf + 1) * sizeof(char *));
   nf = split(fields, buffer, PSEP);  
   fields[nf] = 0;

   getusrinfo();
   
   return fields;
}

int split(char *p[], char *var, char psep)
{
    char *s, *t;
    int n;

    s = var;
    n = 0;

    while((t = strchr(s, psep))) {
        *t = 0;
        if(strcmp(s, ""))
            p[n++] = s;
        s = t + 1;
    }

    if(strcmp(s, ""))
        p[n++] = s;

    return n;
}

int processpath(char *dirname)
{
    DIR *dd;
    char path[MAXPATHLEN]; 
    int numx;             
    struct dirent *dptr; 

    dd = opendir(dirname);
    numx = 0;

    if(dd == NULL) {
        fprintf(stderr, "cannot open %s\n", dirname);
        return -1;
    }

    while((dptr = readdir(dd))) {
        if(dptr == NULL) break;
        sprintf(path, "%s/%s", dirname, dptr->d_name);
        numx += findexecutables(dptr->d_name, path);
    }

    closedir(dd);

    return numx;
}

int findexecutables(char *fname, char *path)
{
    if(executable(path)) {
        return insert(fname, path);
    } else {
        return 0;
    }
}

int executable(char *path)
{
    int i;
    int rv;            
    struct stat sbuff; 
    
    rv = lstat(path, &sbuff);

    if(rv < 0) {
        return 0;
    }

    if(S_ISREG(sbuff.st_mode)) {
        if(usr == sbuff.st_uid && sbuff.st_mode & S_IXUSR) {
            #ifdef DEBUG_STAT
                printf("owner: %s\n", path);
            #endif
            return 1;
        } else if(usr == sbuff.st_uid) {
            return 0;
        }

        for(i = 0; i < ngrps; i++) {
            if(grps[i] == sbuff.st_gid && sbuff.st_mode & S_IXGRP) {
                #ifdef DEBUG_STAT
                    printf("group: %s\n", path);
                #endif
                return 1;
            } else if(grps[i] == sbuff.st_gid) {
                return 0;
            }
        }

        if(sbuff.st_mode & S_IXOTH) {
            #ifdef DEBUG_STAT
                printf("other: %s\n", path);
            #endif
            return 1;
        }
    }

    #ifdef DEBUG_STAT
        printf("not regular file: %s\n", path);
    #endif

    return 0;
}

void getusrinfo()
{
    int rv;
    struct passwd *pw; 

    usr = geteuid();
    pw = getpwuid(usr);

    if(pw == NULL) {
        perror("getpwuid");
        exit(1);
    }

    rv = getgrouplist(pw->pw_name, pw->pw_gid, grps, &ngrps);

    if(rv == -1) {
        fprintf(stderr, "getgrouplist failed\n");
        exit(1);
    }
}

