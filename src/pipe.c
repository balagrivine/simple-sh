#include "pipe.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int setup_pipe(int pipefd[2]){
    if(pipe(pipefd) != 0){
        perror("pipe");
        return -1;
    }

    return 0;
}
