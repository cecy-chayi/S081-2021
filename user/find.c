#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
    static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
    p++;

    // Return blank-padded name.
    if(strlen(p) >= DIRSIZ)
    return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    return buf;
}

void find(char *path, char *key) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if(fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type) {
        case T_FILE:
            if(strcmp(fmtname(path), key) == 0) {
                fprintf(1, "%s\n", path);
            }
        break;
        
        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ > sizeof buf) {
                fprintf(2, "find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)) {
                if(de.inum == 0) {
                    continue;
                }
                if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                    continue;
                }
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0) {
                    fprintf(2, "find: cannot stat %s\n", buf);
                    continue;
                } 
                find(buf, key);
            }
        break;
    }
    close(fd);

}

int
main(int argc, char *argv[]) {
    char key[DIRSIZ + 1];
    if(argc < 3) {
        fprintf(2, "usage: find <path> <key>\n");
        exit(0);
    }
    if(strlen(argv[2]) >= DIRSIZ) {
        fprintf(2, "find: fileName too long\n");
        exit(0);
    }
    memmove(key, argv[2], strlen(argv[2]));
    memset(key + strlen(key), ' ', DIRSIZ - strlen(key));
    find(argv[1], key);
    exit(0);
}