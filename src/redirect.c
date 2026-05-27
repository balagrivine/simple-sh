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

    int flags = O_CREAT | O_WRONLY;

    char *redirect = strchr(command, '>');
    if (!redirect) {
        return 0;
    }

    // check for another occurence of '>' so that we open the redirect file
    // in append mode
    if (*(redirect + 1) == '>') {
        flags |= O_APPEND;
    } else {
        flags |= O_TRUNC;
    }

    *redirect = '\0';

    redirect++;

    // Skip second > in >>
    if (*redirect == '>') {
        redirect++;
    }

    while (isspace((unsigned char)*redirect)) {
        redirect++;
    }

    if (*redirect == '\0') {
        fprintf(stderr, "simple-sh: parse error\n");
        return -1;
    }

    int fd = open(redirect, flags, 0644);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        close(fd);
        return -1;
    }

    close(fd);

    return 0;
}
