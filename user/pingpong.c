#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) 
{
    if(argc != 1) {
        fprintf(2, "too more parament for pingpong");
        exit(1);
    }
    int p[2];
    pipe(p);
    if(fork() == 0) {
        char x;
        read(p[0], &x, 1);
        fprintf(1, "%d: received ping\n", getpid());
        close(p[0]);
        x = 's';
        write(p[1], &x, 1);
        close(p[1]);
        exit(0);
    } else {
        char x = 'p';
        write(p[1], &x, 1);
        close(p[1]);
        read(p[0], &x, 1);
        wait(0);
        fprintf(1, "%d: received pong\n", getpid());
        close(p[0]);
        exit(0);
    }
}