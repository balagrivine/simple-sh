#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "builtin.h"

int handle_builtin(char *command){
    if (!command) return -1;

    if (strncmp(command, "cd ", 3) == 0) {
        return handle_chdir_builtin(command);
    }

    return -1;
}

int handle_chdir_builtin(const char *command) {
    if (!command) {
        return -1;
    }

    const char *relative_path = command + 3;

    char *current_dir = getcwd(NULL, 0);
    char absolute_path[1024];

    snprintf(absolute_path, sizeof(absolute_path), "%s/%s", current_dir, relative_path);
    free(current_dir);

    return chdir(absolute_path);

    return -1;
}
