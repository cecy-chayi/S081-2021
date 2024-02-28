#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void dfs(int *p) {
    close(p[1]);

    int newP[2];
    pipe(newP);
    int prime = -1, x = -1, siz;
    siz = read(p[0], &prime, 4);

    if(siz == 0) {
        close(p[0]);
        return ;
    }

    fprintf(2, "prime %d\n", prime);
    if(fork() == 0) {
        dfs(newP);
        close(newP[0]);
        close(newP[1]);
        exit(0);
    } else {
        close(newP[0]);
        while((read(p[0], &x, 4)) > 0) {
            if(x % prime != 0) {
                write(newP[1], &x, 4);
            }
        }
        close(p[0]);
        close(newP[1]);
        wait(0);
    }
}

int
main(int argc, char *argv[]) {
    if(argc != 1) {
        fprintf(2, "too more parament for primes");
        exit(1);
    }
    int p[2];
    pipe(p);
    if(fork() == 0) {
        dfs(p);
    } else {
        close(p[0]);
        for(int i = 2; i <= 35; i++) {
            write(p[1], &i, 4);
        }
        close(p[1]);
        wait(0);
    }
    exit(0);
}