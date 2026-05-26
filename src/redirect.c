#include "redirect.h"

#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int
setup_redirect(char *command){
    if (!command){
        return -1;
    }

    char *redirect = strchr(command, '>');
    if (!redirect){
        return 0;
    }

    *redirect = '\0';

    redirect++;
    while (isspace((unsigned char)*redirect)){
        redirect++;
    }

    if (*redirect == '\0'){
        fprintf(stderr, "simple-sh: parse error\n");
        return -1;
    }

    int fd = open(redirect, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1){
        perror("open");
        return -1;
    }

    int ret = dup2(fd, STDOUT_FILENO);
    if (ret == -1){
        close(fd);
        perror("dup2");
        return -1;
    }

    close(fd);

    return 0;
}
