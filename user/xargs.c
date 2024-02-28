#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

void delete(int argc, char *argv[]) {
    for(int i = 0; i < argc; i++) if(argv[i] != 0) {
        free(argv[i]);
    }
}

void xargs(int argc, char *argv[]) {
    if(argc == 0) {
        delete(MAXARG, argv);
        exit(1);
    }
    if(fork() == 0) {
        exec(argv[0], argv);
        fprintf(2, "exec %s failed\n", argv[0]);
        delete(MAXARG, argv);
        exit(1);
    } else {
        wait(0);
    }
}



int
main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(2, "usage: <acmd> | xargs <cmd>");
        exit(0);
    }
    char buf[512] = {0};
    char *line[MAXARG + 1];
    for(int i = 0; i <= MAXARG; i++) {
        line[i] = (char *) malloc(128);
    }
    for(int i = 1; i < argc; i++) {
        strcpy(line[i - 1], argv[i]);
    }
    argc--;
    int lineCnt = argc, offset = 0, i = 0, n;
    
    while((n = read(0, buf + i, 1)) != 0) {
    
        if(buf[i] == '\n' || buf[i] == '\r' || buf[i] == '\0') {
            line[lineCnt++][offset] = 0;
            free(line[lineCnt]);
            line[lineCnt] = 0;
            xargs(lineCnt, line);
            line[lineCnt] = (char *) malloc(128);
            lineCnt = argc; 
            offset = 0;
        }
        else if(buf[i] == ' ') {
            line[lineCnt++][offset] = 0;

            offset = 0;
        }
        else {
            if(lineCnt >= MAXARG) {
                fprintf(2, "exec: too many arguments\n");
                delete(MAXARG, line);
                exit(1);
            }
            line[lineCnt][offset++] = buf[i];
        }
        i++;
    }
    
    
    
    
    delete(MAXARG, line);
    
    exit(0);
}